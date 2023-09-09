/*
 * main.c
 *
 * Author: Erich Styger
 * License: PDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdint.h>

static int readBytes(FILE *fp, unsigned char *buf, size_t nofBytes) {
  if (fread(buf, 1, nofBytes, fp)!=nofBytes) {
    return -1;
  }
  return 0; /* ok */
}

static int read64u(FILE *fp, uint64_t *val) {
  unsigned char buf[sizeof(uint64_t)];
  uint32_t vh, vl;

  if (fread(buf, 1, sizeof(uint64_t), fp)!=sizeof(uint64_t)) {
    return -1;
  }
  vh = (buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3];
  vl = (buf[4]<<24) + (buf[5]<<16) + (buf[6]<<8) + buf[7];
  *val = (((uint64_t)vh)<<32) + vl;
  return 0; /* ok */
}

static int read32u(FILE *fp, uint32_t *val) {
  unsigned char buf[sizeof(uint32_t)];

  if (fread(buf, 1, sizeof(uint32_t), fp)!=sizeof(uint32_t)) {
    return -1;
  }
  *val = (buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3];
  return 0; /* ok */
}

static int read16u(FILE *fp, uint16_t *val) {
  unsigned char buf[sizeof(uint16_t)];

  if (fread(buf, 1, sizeof(uint16_t), fp)!=sizeof(uint16_t)) {
    return -1;
  }
  *val = (buf[0]<<8) + buf[1];
  return 0; /* ok */
}

static int readMagicNumber(FILE *fp) {
  /* Magic number (4 bytes): field used to verify if the imported file is an Energy Measurement file. */
  uint32_t magic;

  if (read32u(fp, &magic) != 0) {
    printf("failed reading magic number\n");
    return -1;
  }
  printf("magic number: 0x%x\n", magic);
  /* check for "$EMF": 0x24454D46 */
  if (magic!=0x24454D46) {
    printf("wrong magic number: 0x%x\n", magic);
    return -1;
  }
  return 0;
}

static int readVersionNumber(FILE *fp) {
  /* Version (2 bytes): the file version. */
  uint16_t version;

  if (read16u(fp, &version) != 0) {
    printf("failed reading version number\n");
    return -1;
  }
  printf("version: 0x%x\n", version);
  if (version!=0x0001) {
    printf("wrong version number: 0x%x\n", version);
    return -1;
  }
  return 0;
}

static int readBase(FILE *fp) {
  /* Base (4 bytes): the number of values that are registered as individual values. */
  uint32_t base;

  if (read32u(fp, &base) != 0) {
    printf("failed reading base\n");
    return -1;
  }
  printf("base: 0x%x\n", base);
  return 0;
}

static int readStep(FILE *fp) {
  /* Step (8 bytes): the number of samples used to calculate a summary value. */
  uint64_t step;

  if (read64u(fp, &step) != 0) {
    printf("failed reading version number\n");
    return -1;
  }
  printf("step: 0x%08x%08x\n", (uint32_t)(step>>32), (uint32_t)(step));
  return 0;
}

static int readSourceID(FILE *fp) {
  /* SourceID (4 bytes): the measurement source ID associated with the registered data.
   * Examples of measurement sources: target current, target voltage, target power.
   */
  uint32_t id;

  if (read32u(fp, &id) != 0) {
    printf("failed reading sourceID\n");
    return -1;
  }
  printf("sourceID: 0x%x\n", id);
  return 0;
}

typedef struct dataItem_t {
  uint64_t timeStamp; /* timestamp (8 bytes): the timestamp associated with a value (timestamp 1 corresponds to the first value). This value is in microseconds. Example: 2.0358867E7 [us]. The first two timestamps can be used to calculate the sampling period. The other timestamp values are calculated based on the sampling period.  */
  uint32_t value; /* value (4 bytes): the value reported by ADC. Example: 4095. This value is further processed according to the used probe, see Data processing for more details.  */
  struct { /* summary values (16 bytes): Sequences of [min, avg, max] values. The summary values are calculated for each i-th value in the sequence, if “i” is a multiple of “base”. The number of summary values is adjusted logarithmically according to the number of values registered so far. */
    uint32_t min;
    uint64_t avg;
    uint32_t max;
  } summary;
} dataItem_t;

static int readDataItem(FILE *fp, size_t nofItems) {
  dataItem_t data;

  if (read64u(fp, &data.timeStamp) != 0) {
    printf("failed reading timetamp\n");
    return -1;
  }
  printf("timestamp: 0x%08x%08x\n", (uint32_t)(data.timeStamp>>32), (uint32_t)(data.timeStamp));

  if (read32u(fp, &data.value) != 0) {
    printf("failed reading value\n");
    return -1;
  }
  printf("value: 0x%x\n", data.value);

  if (read32u(fp, &data.summary.min) != 0) {
    printf("failed reading min\n");
    return -1;
  }
  printf("min: 0x%x\n", data.summary.min);

  if (read64u(fp, &data.summary.avg) != 0) {
    printf("failed reading avg\n");
    return -1;
  }
  printf("avg: 0x%08x%08x\n", (uint32_t)(data.summary.avg>>32), (uint32_t)(data.summary.avg));

  if (read32u(fp, &data.summary.max) != 0) {
    printf("failed reading max\n");
    return -1;
  }
  printf("max: 0x%x\n", data.summary.max);
  return 0;
}

static int convertDataFile(const char *filename) {
  /* see NXP MCUXpresso User Guide, MCUXpresso IDE LinkServer Energy Measurement Guide -> Appendix -> Raw data export format and data processing information */
  FILE *fp;
  int res = 0;

  printf("open file '%s'\n", filename);
  fp = fopen(filename, "r");
  if (fp==NULL) {
     printf("file '%s' open failed\n", filename);
    return -1;
  }
  for(;;) { /* at the end or in case of error */
    res = 0;
    if (readMagicNumber(fp)!=0) {
      printf("failed reading magic number\n");
      res = -1;
      break;
    }
    if (readVersionNumber(fp)!=0) {
      printf("failed reading version number\n");
      res = -1;
      break;
    }
    if (readBase(fp)!=0) {
      printf("failed reading base\n");
      res = -1;
      break;
    }
    if (readStep(fp)!=0) {
      printf("failed reading step\n");
      res = -1;
      break;
    }
    if (readSourceID(fp)!=0) {
      printf("failed reading SourceID\n");
      res = -1;
      break;
    }
    if (readDataItem(fp, 1)!=0) {
      printf("failed reading data item(s)\n");
      res = -1;
      break;
    }
    break;
  } /* for */
  fclose(fp);
  printf("file '%s' closed\n", filename);
  return res; /* ok */
}

int main(void) {
  printf("Program to read data file.\n");
  //convertDataFile("./rawData.bin");
  convertDataFile("/home/pi/mse/c/app/rawData.bin");
  return 0;
}
