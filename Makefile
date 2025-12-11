all: basenamedirname.exe

clean:
	rm -rf basenamedirname.exe

basenamedirname.exe: basenamedirname.h basenamedirname.c main.c
	gcc basenamedirname.c main.c -g -o $@
