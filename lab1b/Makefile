.SILENT:

default: client server

client:
	gcc -g -Wall -Wextra lab1b-client.c -o lab1b-client -lz

server:
	gcc -g -Wall -Wextra lab1b-server.c -o lab1b-server -lz

clean:
	rm -rf lab1b-client lab1b-server lab1b-205348339.tar.gz

dist:
	tar -zcvf lab1b-205348339.tar.gz lab1b-client.c lab1b-server.c \
							         Makefile README
