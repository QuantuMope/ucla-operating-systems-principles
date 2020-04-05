.SILENT:

default: build

build:
	gcc -Wall -Wextra -pthread lab2_add.c -o lab2_add
	gcc -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

clean:
	rm -rf lab2_add lab2_list lab2a-205348339.tar.gz

graphs: tests
	./lab2_add.gp
	./lab2_list.gp

dist: graphs
	tar -zcvf lab2a-205348339.tar.gz lab2_add.c lab2_list.c SortedList.c SortedList.h \
			  Makefile README lab2_add.gp lab2_list.gp lab2_add.csv lab2_list.csv \
			  lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png \
			  lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png

tests: build
	# lab2_add-1.png
	./lab2_add --t 1 --i 100 > lab2_add.csv
	./lab2_add --t 1 --i 1000 >> lab2_add.csv
	./lab2_add --t 1 --i 10000 >> lab2_add.csv
	./lab2_add --t 1 --i 100000 >> lab2_add.csv
	-./lab2_add --t 2 --i 100 >> lab2_add.csv
	-./lab2_add --t 2 --i 1000 >> lab2_add.csv
	-./lab2_add --t 2 --i 10000 >> lab2_add.csv
	-./lab2_add --t 2 --i 100000 >> lab2_add.csv
	-./lab2_add --t 4 --i 100 >> lab2_add.csv
	-./lab2_add --t 4 --i 1000 >> lab2_add.csv
	-./lab2_add --t 4 --i 10000 >> lab2_add.csv
	-./lab2_add --t 4 --i 100000 >> lab2_add.csv
	-./lab2_add --t 8 --i 100 >> lab2_add.csv
	-./lab2_add --t 8 --i 1000 >> lab2_add.csv
	-./lab2_add --t 8 --i 10000 >> lab2_add.csv
	-./lab2_add --t 8 --i 100000 >> lab2_add.csv
	-./lab2_add --t 12 --i 100 >> lab2_add.csv
	-./lab2_add --t 12 --i 1000 >> lab2_add.csv
	-./lab2_add --t 12 --i 10000 >> lab2_add.csv
	-./lab2_add --t 12 --i 100000 >> lab2_add.csv

	-./lab2_add --y --t 1 --i 10 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 20 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 40 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 80 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 100 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 1000 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 10000 >> lab2_add.csv
	-./lab2_add --y --t 1 --i 100000 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 10 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 20 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 40 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 80 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 100 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 1000 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 10000 >> lab2_add.csv
	-./lab2_add --y --t 2 --i 100000 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 10 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 20 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 40 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 80 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 100 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 1000 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 10000 >> lab2_add.csv
	-./lab2_add --y --t 4 --i 100000 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 10 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 20 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 40 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 80 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 100 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 1000 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 10000 >> lab2_add.csv
	-./lab2_add --y --t 8 --i 100000 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 10 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 20 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 40 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 80 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 100 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 1000 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 10000 >> lab2_add.csv
	-./lab2_add --y --t 12 --i 100000 >> lab2_add.csv

	# lab2_add-3.png
	./lab2_add --t 1 --i 1 >> lab2_add.csv
	./lab2_add --t 1 --i 10 >> lab2_add.csv

	# lab2_add-4.png
	./lab2_add --y --t 2 --i 10000 --s m >> lab2_add.csv
	./lab2_add --y --t 2 --i 10000 --s c >> lab2_add.csv
	./lab2_add --y --t 2 --i 1000 --s s >> lab2_add.csv
	./lab2_add --y --t 4 --i 10000 --s m >> lab2_add.csv
	./lab2_add --y --t 4 --i 10000 --s c >> lab2_add.csv
	./lab2_add --y --t 4 --i 1000 --s s >> lab2_add.csv
	./lab2_add --y --t 8 --i 10000 --s m >> lab2_add.csv
	./lab2_add --y --t 8 --i 10000 --s c >> lab2_add.csv
	./lab2_add --y --t 8 --i 1000 --s s >> lab2_add.csv
	./lab2_add --y --t 12 --i 10000 --s m >> lab2_add.csv
	./lab2_add --y --t 12 --i 10000 --s c >> lab2_add.csv
	./lab2_add --y --t 12 --i 1000 --s s >> lab2_add.csv

	# lab2_add-5.png
	-./lab2_add --t 1 --i 10000 >> lab2_add.csv
	-./lab2_add --t 1 --i 10000 --s m >> lab2_add.csv
	-./lab2_add --t 1 --i 10000 --s c >> lab2_add.csv
	-./lab2_add --t 1 --i 10000 --s s >> lab2_add.csv
	-./lab2_add --t 2 --i 10000 >> lab2_add.csv
	-./lab2_add --t 2 --i 10000 --s m >> lab2_add.csv
	-./lab2_add --t 2 --i 10000 --s c >> lab2_add.csv
	-./lab2_add --t 2 --i 10000 --s s >> lab2_add.csv
	-./lab2_add --t 4 --i 10000 >> lab2_add.csv
	-./lab2_add --t 4 --i 10000 --s m >> lab2_add.csv
	-./lab2_add --t 4 --i 10000 --s c >> lab2_add.csv
	-./lab2_add --t 4 --i 10000 --s s >> lab2_add.csv
	-./lab2_add --t 8 --i 10000 >> lab2_add.csv
	-./lab2_add --t 8 --i 10000 --s m >> lab2_add.csv
	-./lab2_add --t 8 --i 10000 --s c >> lab2_add.csv
	-./lab2_add --t 8 --i 10000 --s s >> lab2_add.csv
	-./lab2_add --t 12 --i 10000 >> lab2_add.csv
	-./lab2_add --t 12 --i 10000 --s m >> lab2_add.csv
	-./lab2_add --t 12 --i 10000 --s c >> lab2_add.csv
	-./lab2_add --t 12 --i 10000 --s s >> lab2_add.csv

	# lab2_list-1.png
	-./lab2_list --t 1 --i 10 > lab2_list.csv
	-./lab2_list --t 1 --i 100 >> lab2_list.csv
	-./lab2_list --t 1 --i 1000 >> lab2_list.csv
	-./lab2_list --t 1 --i 10000 >> lab2_list.csv
	-./lab2_list --t 1 --i 20000 >> lab2_list.csv

	# lab2_list-2.png
	-./lab2_list --t 2 --i 1 >> lab2_list.csv
	-./lab2_list --t 2 --i 10 >> lab2_list.csv
	-./lab2_list --t 2 --i 100 >> lab2_list.csv
	-./lab2_list --t 2 --i 1000 >> lab2_list.csv
	-./lab2_list --t 4 --i 1 >> lab2_list.csv
	-./lab2_list --t 4 --i 10 >> lab2_list.csv
	-./lab2_list --t 4 --i 100 >> lab2_list.csv
	-./lab2_list --t 4 --i 1000 >> lab2_list.csv
	-./lab2_list --t 8 --i 1 >> lab2_list.csv
	-./lab2_list --t 8 --i 10 >> lab2_list.csv
	-./lab2_list --t 8 --i 100 >> lab2_list.csv
	-./lab2_list --t 8 --i 1000 >> lab2_list.csv
	-./lab2_list --t 12 --i 1 >> lab2_list.csv
	-./lab2_list --t 12 --i 10 >> lab2_list.csv
	-./lab2_list --t 12 --i 100 >> lab2_list.csv
	-./lab2_list --t 12 --i 1000 >> lab2_list.csv
	-./lab2_list --t 2 --i 1 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 1 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 1 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 1 --y dl >> lab2_list.csv
	-./lab2_list --t 2 --i 2 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 2 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 2 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 2 --y dl >> lab2_list.csv
	-./lab2_list --t 2 --i 4 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 4 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 4 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 4 --y dl >> lab2_list.csv
	-./lab2_list --t 2 --i 8 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 8 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 8 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 8 --y dl >> lab2_list.csv
	-./lab2_list --t 2 --i 16 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 16 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 16 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 16 --y dl >> lab2_list.csv
	-./lab2_list --t 2 --i 32 --y i >> lab2_list.csv
	-./lab2_list --t 2 --i 32 --y d >> lab2_list.csv
	-./lab2_list --t 2 --i 32 --y il >> lab2_list.csv
	-./lab2_list --t 2 --i 32 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 1 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 1 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 1 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 1 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 2 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 2 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 2 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 2 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 4 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 4 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 4 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 4 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 8 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 8 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 8 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 8 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 16 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 16 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 16 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 16 --y dl >> lab2_list.csv
	-./lab2_list --t 4 --i 32 --y i >> lab2_list.csv
	-./lab2_list --t 4 --i 32 --y d >> lab2_list.csv
	-./lab2_list --t 4 --i 32 --y il >> lab2_list.csv
	-./lab2_list --t 4 --i 32 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 1 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 1 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 1 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 1 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 2 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 2 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 2 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 2 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 4 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 4 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 4 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 4 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 8 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 8 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 8 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 8 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 16 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 16 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 16 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 16 --y dl >> lab2_list.csv
	-./lab2_list --t 8 --i 32 --y i >> lab2_list.csv
	-./lab2_list --t 8 --i 32 --y d >> lab2_list.csv
	-./lab2_list --t 8 --i 32 --y il >> lab2_list.csv
	-./lab2_list --t 8 --i 32 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 1 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 1 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 1 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 1 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 2 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 2 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 2 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 2 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 4 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 4 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 4 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 4 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 8 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 8 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 8 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 8 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 16 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 16 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 16 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 16 --y dl >> lab2_list.csv
	-./lab2_list --t 12 --i 32 --y i >> lab2_list.csv
	-./lab2_list --t 12 --i 32 --y d >> lab2_list.csv
	-./lab2_list --t 12 --i 32 --y il >> lab2_list.csv
	-./lab2_list --t 12 --i 32 --y dl >> lab2_list.csv

	# lab2_list-3.png
	./lab2_list --t 12 --i 32 --s m --y i >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s m --y d >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s m --y il >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s m --y dl >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s s --y i >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s s --y d >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s s --y il >> lab2_list.csv
	./lab2_list --t 12 --i 32 --s s --y dl >> lab2_list.csv

	# lab2_list-4.png
	-./lab2_list --s m --t 1 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 2 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 4 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 8 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 12 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 16 --i 1000 >> lab2_list.csv
	-./lab2_list --s m --t 24 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 1 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 2 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 4 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 8 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 12 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 16 --i 1000 >> lab2_list.csv
	-./lab2_list --s s --t 24 --i 1000 >> lab2_list.csv

