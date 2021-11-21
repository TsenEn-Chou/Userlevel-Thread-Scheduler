TARGETS = main
CC := gcc
CFLAGS += -std=gnu99 -g -Wall
OBJS = main.o list.o

GIT_HOOKS := .git/hooks/applied

all: $(GIT_HOOKS) $(TARGETS)

$(GIT_HOOKS):
	@.githooks/install-git-hooks
	@echo

$(TARGETS):$(OBJS)
	$(CC) $(CFLAGS) -o main $(OBJS)

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm *.o 
	