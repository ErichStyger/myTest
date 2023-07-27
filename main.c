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

static int read32(FILE *fp, uint32_t *val) {
  unsigned char buf[4];

  if (fread(buf, 1, 4, fp)!=4) {
    return -1;
  }
  *val = (buf[0]<<24) + (buf[1]<<16) + (buf[2]<<8) + buf[3];
  return 0; /* ok */
}

static int readMagicNumber(FILE *fp) {
  uint32_t magic;

  if (read32(fp, &magic) != 0) {
    printf("failed reading magic number\n");
    return -1;
  }
  /* check for "$EMF": 0x24454D46 */
  if (magic!=0x24454D46) {
    printf("wrong number: 0x%x\n", magic);
    return -1;
  }
  return 0;
}

static int convertDataFile(const char *filename) {
  /* see NXP MCUXpresso User Guide, MCUXpresso IDE LinkServer Energy Measurement Guide -> Appendix -> Raw data export format and data processing information */
  FILE *fp;

  printf("open file '%s'\n", filename);
  fp = fopen(filename, "r");
  if (fp==NULL) {
    printf("file open failed\n");
    return -1;
  }
  if (readMagicNumber(fp)!=0) {
    printf("failed magic number\n");
  }
  fclose(fp);
  printf("file '%s' closed\n", filename);
  return 0; /* ok */
}

int main(void) {
  printf("Hello world!\n");
  convertDataFile("rawData.bin");
  return 0;
}
