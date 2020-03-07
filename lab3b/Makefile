# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: build

build:
	printf "python3 lab3b.py" > lab3b
	printf " \044" >> lab3b
	printf "@" >> lab3b
	chmod +x lab3b

clean:
	rm -rf lab3b lab3b-205348339.tar.gz

dist:
	tar -zcvf lab3b-205348339.tar.gz lab3b.py README Makefile
