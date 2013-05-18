nf-bitindex:	nf-bitindex.o	iv4file.o
	gcc -o nf-bitindex nf-bitindex.o iv4file.o -ggdb -lnfdump `pkg-config --libs glib-2.0` -lz

nf-bitindex.o:	nf-bitindex.c
	gcc -Wall -c nf-bitindex.c -ggdb `pkg-config --cflags glib-2.0`

iv4file.o:	iv4file.c
	gcc -Wall -c iv4file.c -ggdb
clean:
	- rm nf-bitindex
	- rm *.o


