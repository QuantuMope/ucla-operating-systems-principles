.SILENT:

default: build

build:
	gcc -g -Wall -Wextra -pthread -lprofiler lab2_list.c SortedList.c -o lab2_list

clean:
	rm -rf lab2_list lab2b-205348339.tar.gz

graphs: tests
	./lab2b_list.gp

dist: graphs
	tar -zcvf lab2b-205348339.tar.gz lab2_list.c SortedList.c SortedList.h lab2b_list.csv \
									 lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png \
									 lab2b_list.gp profile.out Makefile README

profile: build
	env CPUPROFILE=lab2b.prof ./lab2_list --t 12 --i 1000 --s s
	pprof --text ./lab2_list lab2b.prof > profile.out
	pprof --list=spin_lock_and_time ./lab2_list lab2b.prof >> profile.out
	rm -rf lab2b.prof

tests: build
	# lab2b_1.png
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

	# lab3b_3.png
	-./lab2_list --y id --t 1 --i 1 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 1 --i 2 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 1 --i 4 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 1 --i 8 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 1 --i 16 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 4 --i 1 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 4 --i 2 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 4 --i 4 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 4 --i 8 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 4 --i 16 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 8 --i 1 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 8 --i 2 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 8 --i 4 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 8 --i 8 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 8 --i 16 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 12 --i 1 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 12 --i 2 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 12 --i 4 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 12 --i 8 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 12 --i 16 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 16 --i 1 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 16 --i 2 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 16 --i 4 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 16 --i 8 --l 4 >> lab2b_list.csv
	-./lab2_list --y id --t 16 --i 16 --l 4 >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 10 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 20 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 40 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 80 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 10 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 20 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 40 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 80 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 10 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 20 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 40 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 80 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 10 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 20 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 40 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 80 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 10 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 20 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 40 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 80 --l 4 --s m >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 10 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 20 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 40 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 1 --i 80 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 10 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 20 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 40 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 4 --i 80 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 10 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 20 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 40 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 8 --i 80 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 10 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 20 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 40 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 12 --i 80 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 10 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 20 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 40 --l 4 --s s >> lab2b_list.csv
	./lab2_list --y id --t 16 --i 80 --l 4 --s s >> lab2b_list.csv

	# lab2b_4.png
	./lab2_list --t 1 --i 1000 --l 4 --s m >> lab2b_list.csv
	./lab2_list --t 1 --i 1000 --l 8 --s m >> lab2b_list.csv
	./lab2_list --t 1 --i 1000 --l 16 --s m >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 4 --s m >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 8 --s m >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 16 --s m >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 4 --s m >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 8 --s m >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 16 --s m >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 4 --s m >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 8 --s m >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 16 --s m >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 4 --s m >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 8 --s m >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 16 --s m >> lab2b_list.csv

	# lab2b_5.png
	./lab2_list --t 1 --i 1000 --l 4 --s s >> lab2b_list.csv
	./lab2_list --t 1 --i 1000 --l 8 --s s >> lab2b_list.csv
	./lab2_list --t 1 --i 1000 --l 16 --s s >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 4 --s s >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 8 --s s >> lab2b_list.csv
	./lab2_list --t 2 --i 1000 --l 16 --s s >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 4 --s s >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 8 --s s >> lab2b_list.csv
	./lab2_list --t 4 --i 1000 --l 16 --s s >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 4 --s s >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 8 --s s >> lab2b_list.csv
	./lab2_list --t 8 --i 1000 --l 16 --s s >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 4 --s s >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 8 --s s >> lab2b_list.csv
	./lab2_list --t 12 --i 1000 --l 16 --s s >> lab2b_list.csv
