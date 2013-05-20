# what flags you want to pass to the C compiler & linker
#CFLAGS = --pedantic -Wall -g0 -O2
CFLAGS = --pedantic -Wall -ggdb -O2
LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
bin_PROGRAMS = cryptoquery benchmarkhve generatehve testing
cryptoquery_SOURCES = common.h config.h configuration.c configuration.h cryptoquery.c cryptoquery.h database/sqlite.h database.c database.h encryption.c encryption.h schemas/hve.h schemas/hve.c schemas/lostw.c schemas/lostw.h schemas/hve_ip.c schemas/hve_ip.h
benchmarkhve_SOURCES = schemas/hve_ip.c schemas/hve_ip.h test/benchmark_hve.c
generatehve_SOURCES = schemas/hve_ip.c schemas/hve_ip.h test/generate_hve.c

