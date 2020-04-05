.SILENT:

LIBRARIES = -lmraa -lm

default: build
	
build:
	gcc $(LIBRARIES) -Wall -Wextra lab4b.c -o lab4b

clean:
	rm -rf lab4b lab4b-205348339.tar.gz

dist:
	tar -zcvf lab4b-205348339.tar.gz lab4b.c README Makefile

check: build
	printf "OFF\n" | ./lab4b > /dev/null 2>&1; if [ $$? -eq 0 ];\
    then echo "Successfully connected to sensors"; else echo "Failed to connect to sensors"; fi
	
