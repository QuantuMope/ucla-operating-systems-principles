# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: lab1a

lab1a:
	gcc -g -Wall -Wextra lab1a.c -o lab1a

clean:
	rm -rf lab1a lab1a-205348339.tar.gz

dist:
	tar -zcvf lab1a-205348339.tar.gz lab1a.c Makefile README \
	                                 backtrace.png breakpoint.png
