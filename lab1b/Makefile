# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: clean client server

client:
	gcc -g -Wall -Wextra lab1b-client.c -o lab1b-client

server:
	gcc -g -Wall -Wextra lab1b-server.c -o lab1b-server

clean:
	rm -rf lab1b-client lab1b-server lab1b-205348339.tar.gz

dist:
	tar -zcvf lab1b-205348339.tar.gz lab1b-client.c lab1-server.c \
							         Makefile README
