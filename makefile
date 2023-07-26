all: hello

clean:
	-rm hello

hello: main.c
	arm-linux-gnueabihf-g++ -ggdb main.c -o hello
