#CC=gcc -O2
CC=gcc -ggdb
#objects = src/query.o src/testing.o
objects = src/hve1.o src/testing_hve.o

hve : $(objects)
	$(CC) -o hve -lgmp -lpbc $(objects)

hve1.o : hve1.h
testing_hve.o : hve1.h

.PHONY: clean run
clean : 
	rm -f hve1 $(objects)

run: hve1
	./hve1