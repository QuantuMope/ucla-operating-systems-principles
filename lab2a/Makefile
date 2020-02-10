# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: lab2_list lab2_add

lab2_add:
	gcc -g -Wall -Wextra -pthread lab2_add.c -o lab2_add

lab2_list:
	gcc -g -Wall -Wextra -pthread lab2_list.c SortedList.c -o lab2_list

clean:
	rm -rf lab2_add lab2_list lab2_add.csv lab2_list.csv lab2a-205348339.tar.gz

dist:
	tar -zcvf lab2a-205348339.tar.gz lab2a.c Makefile README

test:
	# lab2_add-1.png
	./lab2_add --t 1 --i 100
	./lab2_add --t 1 --i 1000
	./lab2_add --t 1 --i 10000
	./lab2_add --t 1 --i 100000
	./lab2_add --t 2 --i 100
	./lab2_add --t 2 --i 1000
	./lab2_add --t 2 --i 10000
	./lab2_add --t 2 --i 100000
	./lab2_add --t 4 --i 100
	./lab2_add --t 4 --i 1000
	./lab2_add --t 4 --i 10000
	./lab2_add --t 4 --i 100000
	./lab2_add --t 8 --i 100
	./lab2_add --t 8 --i 1000
	./lab2_add --t 8 --i 10000
	./lab2_add --t 8 --i 100000
	./lab2_add --t 12 --i 100
	./lab2_add --t 12 --i 1000
	./lab2_add --t 12 --i 10000
	./lab2_add --t 12 --i 100000

	./lab2_add --y --t 1 --i 10
	./lab2_add --y --t 1 --i 20
	./lab2_add --y --t 1 --i 40
	./lab2_add --y --t 1 --i 80
	./lab2_add --y --t 1 --i 100
	./lab2_add --y --t 1 --i 1000
	./lab2_add --y --t 1 --i 10000
	./lab2_add --y --t 1 --i 100000
	./lab2_add --y --t 2 --i 10
	./lab2_add --y --t 2 --i 20
	./lab2_add --y --t 2 --i 40
	./lab2_add --y --t 2 --i 80
	./lab2_add --y --t 2 --i 100
	./lab2_add --y --t 2 --i 1000
	./lab2_add --y --t 2 --i 10000
	./lab2_add --y --t 2 --i 100000
	./lab2_add --y --t 4 --i 10
	./lab2_add --y --t 4 --i 20
	./lab2_add --y --t 4 --i 40
	./lab2_add --y --t 4 --i 80
	./lab2_add --y --t 4 --i 100
	./lab2_add --y --t 4 --i 1000
	./lab2_add --y --t 4 --i 10000
	./lab2_add --y --t 4 --i 100000
	./lab2_add --y --t 8 --i 10
	./lab2_add --y --t 8 --i 20
	./lab2_add --y --t 8 --i 40
	./lab2_add --y --t 8 --i 80
	./lab2_add --y --t 8 --i 100
	./lab2_add --y --t 8 --i 1000
	./lab2_add --y --t 8 --i 10000
	./lab2_add --y --t 8 --i 100000
	./lab2_add --y --t 12 --i 10
	./lab2_add --y --t 12 --i 20
	./lab2_add --y --t 12 --i 40
	./lab2_add --y --t 12 --i 80
	./lab2_add --y --t 12 --i 100
	./lab2_add --y --t 12 --i 1000
	./lab2_add --y --t 12 --i 10000
	./lab2_add --y --t 12 --i 100000

	# lab2_add-3.png
	./lab2_add --t 1 --i 1
	./lab2_add --t 1 --i 10

	# lab2_add-4.png
	./lab2_add --y --t 2 --i 10000 --s m
	./lab2_add --y --t 2 --i 10000 --s c
	./lab2_add --y --t 2 --i 1000 --s s
	./lab2_add --y --t 4 --i 10000 --s m
	./lab2_add --y --t 4 --i 10000 --s c
	./lab2_add --y --t 4 --i 1000 --s s
	./lab2_add --y --t 8 --i 10000 --s m
	./lab2_add --y --t 8 --i 10000 --s c
	./lab2_add --y --t 8 --i 1000 --s s
	./lab2_add --y --t 12 --i 10000 --s m
	./lab2_add --y --t 12 --i 10000 --s c
	./lab2_add --y --t 12 --i 1000 --s s

	# lab2_add-5.png
	./lab2_add --t 1 --i 10000
	./lab2_add --t 1 --i 10000 --s m
	./lab2_add --t 1 --i 10000 --s c
	./lab2_add --t 1 --i 10000 --s s
	./lab2_add --t 2 --i 10000
	./lab2_add --t 2 --i 10000 --s m
	./lab2_add --t 2 --i 10000 --s c
	./lab2_add --t 2 --i 10000 --s s
	./lab2_add --t 4 --i 10000
	./lab2_add --t 4 --i 10000 --s m
	./lab2_add --t 4 --i 10000 --s c
	./lab2_add --t 4 --i 10000 --s s
	./lab2_add --t 8 --i 10000
	./lab2_add --t 8 --i 10000 --s m
	./lab2_add --t 8 --i 10000 --s c
	./lab2_add --t 8 --i 10000 --s s
	./lab2_add --t 12 --i 10000
	./lab2_add --t 12 --i 10000 --s m
	./lab2_add --t 12 --i 10000 --s c
	./lab2_add --t 12 --i 10000 --s s

	# lab2_list-1.png
	./lab2_list --t 1 --i 10
	./lab2_list --t 1 --i 100
	./lab2_list --t 1 --i 1000
	./lab2_list --t 1 --i 10000
	./lab2_list --t 1 --i 20000

	# lab2_list-2.png
	./lab2_list --t 2 --i 1
	./lab2_list --t 2 --i 10
	./lab2_list --t 2 --i 100
	./lab2_list --t 2 --i 1000
	./lab2_list --t 4 --i 1
	./lab2_list --t 4 --i 10
	./lab2_list --t 4 --i 100
	./lab2_list --t 4 --i 1000
	./lab2_list --t 8 --i 1
	./lab2_list --t 8 --i 10
	./lab2_list --t 8 --i 100
	./lab2_list --t 8 --i 1000
	./lab2_list --t 12 --i 1
	./lab2_list --t 12 --i 10
	./lab2_list --t 12 --i 100
	./lab2_list --t 12 --i 1000
	./lab2_list --t 2 --i 1 --y i
	./lab2_list --t 2 --i 1 --y d
	./lab2_list --t 2 --i 1 --y il
	./lab2_list --t 2 --i 1 --y dl
	./lab2_list --t 2 --i 2 --y i
	./lab2_list --t 2 --i 2 --y d
	./lab2_list --t 2 --i 2 --y il
	./lab2_list --t 2 --i 2 --y dl
	./lab2_list --t 2 --i 4 --y i
	./lab2_list --t 2 --i 4 --y d
	./lab2_list --t 2 --i 4 --y il
	./lab2_list --t 2 --i 4 --y dl
	./lab2_list --t 2 --i 8 --y i
	./lab2_list --t 2 --i 8 --y d
	./lab2_list --t 2 --i 8 --y il
	./lab2_list --t 2 --i 8 --y dl
	./lab2_list --t 2 --i 16 --y i
	./lab2_list --t 2 --i 16 --y d
	./lab2_list --t 2 --i 16 --y il
	./lab2_list --t 2 --i 16 --y dl
	./lab2_list --t 2 --i 32 --y i
	./lab2_list --t 2 --i 32 --y d
	./lab2_list --t 2 --i 32 --y il
	./lab2_list --t 2 --i 32 --y dl
	./lab2_list --t 4 --i 1 --y i
	./lab2_list --t 4 --i 1 --y d
	./lab2_list --t 4 --i 1 --y il
	./lab2_list --t 4 --i 1 --y dl
	./lab2_list --t 4 --i 2 --y i
	./lab2_list --t 4 --i 2 --y d
	./lab2_list --t 4 --i 2 --y il
	./lab2_list --t 4 --i 2 --y dl
	./lab2_list --t 4 --i 4 --y i
	./lab2_list --t 4 --i 4 --y d
	./lab2_list --t 4 --i 4 --y il
	./lab2_list --t 4 --i 4 --y dl
	./lab2_list --t 4 --i 8 --y i
	./lab2_list --t 4 --i 8 --y d
	./lab2_list --t 4 --i 8 --y il
	./lab2_list --t 4 --i 8 --y dl
	./lab2_list --t 4 --i 16 --y i
	./lab2_list --t 4 --i 16 --y d
	./lab2_list --t 4 --i 16 --y il
	./lab2_list --t 4 --i 16 --y dl
	./lab2_list --t 4 --i 32 --y i
	./lab2_list --t 4 --i 32 --y d
	./lab2_list --t 4 --i 32 --y il
	./lab2_list --t 4 --i 32 --y dl
	./lab2_list --t 8 --i 1 --y i
	./lab2_list --t 8 --i 1 --y d
	./lab2_list --t 8 --i 1 --y il
	./lab2_list --t 8 --i 1 --y dl
	./lab2_list --t 8 --i 2 --y i
	./lab2_list --t 8 --i 2 --y d
	./lab2_list --t 8 --i 2 --y il
	./lab2_list --t 8 --i 2 --y dl
	./lab2_list --t 8 --i 4 --y i
	./lab2_list --t 8 --i 4 --y d
	./lab2_list --t 8 --i 4 --y il
	./lab2_list --t 8 --i 4 --y dl
	./lab2_list --t 8 --i 8 --y i
	./lab2_list --t 8 --i 8 --y d
	./lab2_list --t 8 --i 8 --y il
	./lab2_list --t 8 --i 8 --y dl
	./lab2_list --t 8 --i 16 --y i
	./lab2_list --t 8 --i 16 --y d
	./lab2_list --t 8 --i 16 --y il
	./lab2_list --t 8 --i 16 --y dl
	./lab2_list --t 8 --i 32 --y i
	./lab2_list --t 8 --i 32 --y d
	./lab2_list --t 8 --i 32 --y il
	./lab2_list --t 8 --i 32 --y dl
	./lab2_list --t 16 --i 1 --y i
	./lab2_list --t 16 --i 1 --y d
	./lab2_list --t 16 --i 1 --y il
	./lab2_list --t 16 --i 1 --y dl
	./lab2_list --t 16 --i 2 --y i
	./lab2_list --t 16 --i 2 --y d
	./lab2_list --t 16 --i 2 --y il
	./lab2_list --t 16 --i 2 --y dl
	./lab2_list --t 16 --i 4 --y i
	./lab2_list --t 16 --i 4 --y d
	./lab2_list --t 16 --i 4 --y il
	./lab2_list --t 16 --i 4 --y dl
	./lab2_list --t 16 --i 8 --y i
	./lab2_list --t 16 --i 8 --y d
	./lab2_list --t 16 --i 8 --y il
	./lab2_list --t 16 --i 8 --y dl
	./lab2_list --t 16 --i 16 --y i
	./lab2_list --t 16 --i 16 --y d
	./lab2_list --t 16 --i 16 --y il
	./lab2_list --t 16 --i 16 --y dl
	./lab2_list --t 16 --i 32 --y i
	./lab2_list --t 16 --i 32 --y d
	./lab2_list --t 16 --i 32 --y il
	./lab2_list --t 16 --i 32 --y dl

	# lab2_list-3.png

