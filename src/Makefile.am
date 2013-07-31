# what flags you want to pass to the C compiler & linker
CFLAGS = -g0 -O2
#CFLAGS = -ggdb -DTESTING
#CFLAGS = -ggdb -DDEBUG
#CFLAGS = --pedantic -Wall -ggdb -DTESTING
#CFLAGS = --pedantic -Wall -ggdb -DDEBUG
LDFLAGS =

# this lists the binaries to produce, the (non-PHONY, binary) targets in
# the previous manual Makefile
#bin_PROGRAMS = cryptoquery benchmarkhve generatehve testing testingA
bin_PROGRAMS = testing testingA testsize
cryptoquery_SOURCES = common.h config.h configuration.c configuration.h cryptoquery.c cryptoquery.h database/sqlite.h database.c database.h encryption.c encryption.h schemas/hve.h schemas/hve.c schemas/lostw.c schemas/lostw.h schemas/hve_ip.c schemas/hve_ip.h schemas/utils.c schemas/utils.h schemas/aes.h schemas/aes.c
benchmarkhve_SOURCES = schemas/hve_ip.c schemas/hve_ip.h test/benchmark_hve.c schemas/utils.h schemas/utils.c schemas/aes.h schemas/aes.c
generatehve_SOURCES = schemas/hve_ip.c schemas/hve_ip.h test/generate_hve.c schemas/utils.h schemas/utils.c schemas/aes.h schemas/aes.c
testing_SOURCES = testing.c schemas/utils.c schemas/hve_ip.h schemas/hve_ip.c
testingA_SOURCES = testingA.c schemas/utils.c schemas/hve_ip_amortized.h schemas/hve_ip_amortized2.c
testsize_SOURCES = test/testsize.c schemas/utils.c schemas/hve_ip.h schemas/hve_ip.c schemas/hve_ip_serialization.c
