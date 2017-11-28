.phony all:
all: simple

simple:
	gcc -Wall common.c diskinfo.c -o diskinfo
	gcc -Wall common.c disklist.c -o disklist
	gcc -Wall common.c diskget.c -o diskget
#	gcc -Wall common.c diskput.c -o diskput

.PHONY clean:
clean:
	-rm -rf *.o *.exe
