all: hello

test:
	chmod +x hello
	./hello

clean:
	-rm hello

hello: main.c
	g++ -ggdb main.c -o hello
