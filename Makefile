TARGETS = simulator
CC := gcc
CFLAGS += -std=gnu99 -g -Wall
OBJS = simulator.o list.o os2021_api.o function_libary.o

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(TARGETS)

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

# $(TARGETS):$(OBJS)
# 	$(CC) $(CFLAGS) -o simulator $(OBJS)

simulator:simulator.o os2021_api.o list.o function_libary.o
	$(CC) $(CFLAGS) -o simulator simulator.o os2021_api.o list.o function_libary.o

simulator.o:simulator.c os2021_api.h
	$(CC) $(CFLAGS) -c simulator.c

os2021_api.o:os2021_api.c os2021_api.h list.h function_libary.h
	$(CC) $(CFLAGS) -c os2021_api.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

function_libary.o: function_libary.c function_libary.h
	$(CC) $(CFLAGS) -c function_libary.c

.PHONY: clean
clean:
	rm *.o simulator 
	