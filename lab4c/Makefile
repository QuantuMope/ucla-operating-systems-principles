# NAME: Andrew Choi
# EMAIL: asjchoi@ucla.edu
# ID: 205348339

.SILENT:

#LIBRARIES = -lmraa -lm
TCP_LIBRARIES = -lm
TLS_LIBRARIES = -lm -lssl -lcrypto

default: build
	
build:
	gcc -Wall -Wextra lab4c_tcp.c $(TCP_LIBRARIES) -o lab4c_tcp
	gcc -Wall -Wextra lab4c_tls.c $(TLS_LIBRARIES) -o lab4c_tls

clean:
	rm -rf lab4c_tcp lab4c_tls lab4c-205348339.tar.gz

dist:
	tar -zcvf lab4c-205348339.tar.gz lab4c_tcp.c lab4c_tls.c README Makefile
