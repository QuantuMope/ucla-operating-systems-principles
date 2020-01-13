# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

default: lab0

lab0:
	gcc -g -Wall -Wextra lab0.c -o lab0

clean:
	rm -rf lab0 lab0-205348339.tar.gz

dist:
	tar -zcvf lab0-205348339.tar.gz lab0.c Makefile README \
	                                backtrace.png breakpoint.png

check: clean default
# Smoke test cases
	echo "-------------------------------------------"
	echo "Executing Test Cases"
	echo "-------------------------------------------"
	touch test.txt
	chmod 666 test.txt
# Case 1: Input file doesn't exist.
	./lab0 --input notafile > /dev/null 2>&1; if [ $$? -eq 2 ]; \
	then echo "Case 1 passed"; else echo "Case 1 failed"; fi

# Case 2: Catch Command Works (Segfault first)
	./lab0 --segfault --catch > /dev/null 2>&1; if [ $$? -eq 4 ]; \
	then echo "Case 2 passed"; else echo "Case 2 failed"; fi

# Case 3: Catch Command Works (Segfault second)
	./lab0 --catch --segfault > /dev/null 2>&1; if [ $$? -eq 4 ]; \
	then echo "Case 3 passed"; else echo "Case 3 failed"; fi

# Case 4: Invalid argument
	./lab0 slkd > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 4 passed"; else echo "Case 4 failed"; fi

# Case 5: Valid arguments with invalid argument at end
	./lab0 --catch slkd > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 5 passed"; else echo "Case 5 failed"; fi

# Case 6: Valid arguments with invalid argument in middle
	./lab0 --input test.txt slkd --catch > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 6 passed"; else echo "Case 6 failed"; fi

# Case 7: Valid arguments with invalid argument in middle
	./lab0 --input slkd test.txt --catch > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 7 passed"; else echo "Case 7 failed"; fi

# Case 8: Short arguments
	./lab0 -i test.txt > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 8 passed"; else echo "Case 8 failed"; fi

# Case 9: No argument to input
	./lab0 --i > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 9 passed"; else echo "Case 9 failed"; fi

# Case 10: No argument to output
	./lab0 --o > /dev/null 2>&1; if [ $$? -eq 1 ]; \
	then echo "Case 10 passed"; else echo "Case 10 failed"; fi

# Case 11: Program terminates successfully
	echo "test" | ./lab0  > /dev/null 2>&1; if [ $$? -eq 0 ]; \
	then echo "Case 11 passed"; else echo "Case 11 failed"; fi

# Case 12: Opening file with improper permission
	touch no_permission.txt
	chmod 444 no_permission.txt
	echo "test" | ./lab0 --o no_permission.txt > /dev/null 2>&1; if [ $$? -eq 3 ]; \
	then echo "Case 12 passed"; else echo "Case 12 failed"; fi
	rm -f no_permission.txt test.txt
