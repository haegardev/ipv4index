nf-bitindex:	nf-bitindex.o	iv4file.o iv4ipc.o nf-reader.o ipv4index.o
	gcc -o nf-bitindex nf-bitindex.o iv4file.o iv4ipc.o nf-reader.o ipv4index.o -ggdb -lnfdump `pkg-config --libs glib-2.0` -lz

nf-bitindex.o:	nf-bitindex.c
	gcc -Wall -c nf-bitindex.c -ggdb `pkg-config --cflags glib-2.0`

iv4file.o:	iv4file.c
	gcc -Wall -c iv4file.c -ggdb

iv4ipc.o:	iv4ipc.c
	gcc -Wall -c iv4ipc.c -ggdb
nf-reader.o:	nf-reader.c
	gcc -Wall -c nf-reader.c -ggdb

ipv4index.o:	ipv4index.c
	gcc -Wall -c ipv4index.c -ggdb

clean:
	- rm nf-bitindex
	- rm *.o


