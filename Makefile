#CC=gcc -O2
CC=gcc -ggdb
#objects = src/query.o src/testing.o
#objects = src/hve1.o src/testing_hve.o
objects = src/testing.o src/schemas/ksw_predicate.o

testit : $(objects)
	$(CC) -o testit -lgmp -lpbc $(objects)

src/schema/ksw_predicate.o : src/schemas/ksw_predicate.h

src/testing.o : src/schemas/ksw_predicate.h

.PHONY: clean run
clean : 
	rm -f ./testit $(objects)

run: testit
	./testit