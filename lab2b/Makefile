# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: build

build:
	gcc -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

clean:
	rm -rf lab2_list lab2a-205348339.tar.gz

graphs: tests
	./lab2_list.gp

dist: graphs
	tar -zcvf lab2b-205348339.tar.gz lab2_list.c SortedList.c SortedList.h Makefile README

tests: build
	./lab2_list --t 1 --i 1000 --s m > lab2b_list.csv
	./lab2_list --t 2 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 16 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 24 --i 1000 --s m >> lab2b_list.csv
	./lab2_list --t 1 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 16 --i 1000 --s s >> lab2b_list.csv
	./lab2_list --t 24 --i 1000 --s s >> lab2b_list.csv
