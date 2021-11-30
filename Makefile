TARGETS = simulator
CC := gcc
CFLAGS += -std=gnu99 -g -Wall
OBJS = simulator.o link_list.o os2021_thread_api.o function_libary.o

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(TARGETS)

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

# $(TARGETS):$(OBJS)
# 	$(CC) $(CFLAGS) -o simulator $(OBJS)

simulator:simulator.o os2021_thread_api.o link_list.o function_libary.o
	$(CC) $(CFLAGS) -o simulator simulator.o os2021_thread_api.o link_list.o function_libary.o -ljson-c

simulator.o:simulator.c os2021_thread_api.h
	$(CC) $(CFLAGS) -c simulator.c

os2021_thread_api.o:os2021_thread_api.c os2021_thread_api.h link_list.h function_libary.h
	$(CC) $(CFLAGS) -c os2021_thread_api.c

link_list.o: link_list.c link_list.h
	$(CC) $(CFLAGS) -c link_list.c

function_libary.o: function_libary.c function_libary.h
	$(CC) $(CFLAGS) -c function_libary.c

.PHONY: clean
clean:
	rm *.o simulator 
	