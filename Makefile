target:exe
mld.o:mld.c
	gcc -g -c mld.c -o mld.o
exe:mld.o app.c
	gcc -g app.c mld.o -o exe

clean:
	rm -f *.o
	rm -f exe