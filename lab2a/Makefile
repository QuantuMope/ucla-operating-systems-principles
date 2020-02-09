# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: lab2_list

lab2_add:
	gcc -g -Wall -Wextra -pthread lab2_add.c -o lab2_add

lab2_list:
	gcc -g -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

clean:
	rm -rf lab2_add lab2_list lab2a-205348339.tar.gz

dist:
	tar -zcvf lab2a-205348339.tar.gz lab2a.c Makefile README
