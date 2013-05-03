# what flags you want to pass to the C compiler & linker
CFLAGS = --pedantic -Wall -std=c99 -O2 -ggdb
LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = cryptoquery
cryptoquery_SOURCES = common.h config.h cryptoquery.c cryptoquery.h database/sqlite.h database.c database.h encryption.c encryption.h schemas/hve.h schemas/hve.c schemas/lostw.c schemas/lostw.h
#testlostw = common.h schemas/lostw.c schemas/lostw.h