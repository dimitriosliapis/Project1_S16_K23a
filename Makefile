all:Project

Project: main.o index.o buffer.o search.o hash.o scc.o cc.o grail.o 
	gcc -o Project main.c index.c buffer.c search.c hash.c scc.c cc.c grail.c -lpthread

main.o: main.c
	gcc -c main.c

index.o: index.c
	gcc -c index.c

buffer.o: buffer.c
	gcc -c buffer.c

search.o: search.c
	gcc -c search.c

hash.o: hash.c
	gcc -c hash.c

scc.o: scc.c
	gcc -c scc.c

cc.o: cc.c
	gcc -c cc.c

grail.o: grail.c
	gcc -c grail.c

clean:
	rm -f *.o
