all: hello

clean:
	-rm hello

hello: main.c
	g++ -ggdb main.c -o hello
