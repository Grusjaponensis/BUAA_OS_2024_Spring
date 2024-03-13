all: hello

hello: hello.c
	gcc hello.c -o hello

run:
	./hello

clean:
	rm -f hello
