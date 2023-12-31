CC=g++
CFLAGS=-std=c++11 -g -Wall -pthread -I./
LDFLAGS= -lpthread -lpmemobj -lpmem -lvmem -lmemkind -lurcu spinlock.h spinlock.cc lock.h lock.cc -g
SUBDIRS=core db storeds
SUBSRCS=$(wildcard core/*.cc) $(wildcard db/*.cc) $(wildcard storeds/*.cc)
OBJECTS=$(SUBSRCS:.cc=.o)
EXEC=ycsbc
EXECNONNUMA=ycsbc_non_numa
EXECTEST=ycsbc_test
EXECPARALLEL=ycsbc_parallel
EXECPARALLELNONNUMA=ycsbc_parallel_non_numa
EXECPARALLELTEST=ycsbc_parallel_test

all: $(SUBDIRS) $(EXEC) $(EXECNONNUMA) $(EXECTEST) ${EXECPARALLEL} ${EXECPARALLELNONNUMA} ${EXECPARALLELTEST}

$(SUBDIRS):
	$(MAKE) -C $@

$(EXEC): ycsbc.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECNONNUMA): ycsbc_non_numa.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECTEST): ycsbc_test.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLEL): ycsbc_parallel.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLELNONNUMA): ycsbc_parallel_non_numa.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

$(EXECPARALLELTEST): ycsbc_parallel_test.cc $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir $@; \
	done
	$(RM) $(EXEC)
	$(RM) $(EXECNONNUMA)
	$(RM) $(EXECTEST)
	$(RM) $(EXECPARALLEL)
	$(RM) $(EXECPARALLELNONNUMA)
	$(RM) $(EXECPARALLELTEST)

.PHONY: $(SUBDIRS) $(EXEC) $(EXECNONNUMA) $(EXECTEST) $(EXECPARALLEL) $(EXECPARALLELNONNUMA) $(EXECPARALLELTEST)

