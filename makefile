all: hello

test:
	-chmod +x hello
	./hello

clean:
	-rm hello
	-rm hello.exe

hello: main.c
	g++ -ggdb main.c -o hello
