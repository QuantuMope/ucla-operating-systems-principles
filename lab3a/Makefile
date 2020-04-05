.SILENT:

default: build

build:
	gcc -Wall -Wextra lab3a.c -lm -o lab3a 

clean:
	rm -rf lab3a lab3a-205348339.tar.gz

dist:
	tar -zcvf lab3a-205348339.tar.gz lab3a.c ext2_fs.h README Makefile
