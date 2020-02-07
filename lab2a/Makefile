# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: lab2a_add

lab2a_add:
	gcc -g -Wall -Wextra -pthread lab2a_add.c -o lab2a_add

clean:
	rm -rf lab2a_add lab2a-205348339.tar.gz

dist:
	tar -zcvf lab2a-205348339.tar.gz lab2a.c Makefile README
