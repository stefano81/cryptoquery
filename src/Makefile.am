# what flags you want to pass to the C compiler & linker
CFLAGS = --pedantic -Wall -std=c99 -O2 -ggdb
LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = cryptoquery benchmarkhve generatehve
cryptoquery_SOURCES = common.h config.h configuration.c configuration.h cryptoquery.c cryptoquery.h database/sqlite.h database.c database.h encryption.c encryption.h schemas/hve.h schemas/hve.c schemas/lostw.c schemas/lostw.h
benchmarkhve_SOURCES = schemas/hve.h schemas/hve.c test/benchmark_hve.c
generatehve_SOURCES = schemas/hve.h schemas/hve.c test/generate_hve.c