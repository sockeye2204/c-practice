DIRS := cmd-runner mlfq-scheduler paging-policy tcp time-tlb

.PHONY: all clean concurrency $(DIRS)

all: $(DIRS)

CC := gcc
CFLAGS := -Wall -Wextra -O2

$(DIRS):
	$(CC) $(CFLAGS) $(wildcard $@/*.c) -o $@/$@

concurrency:
ifndef TARGET
	$(error Please specify TARGET=lock-ds or TARGET=mt-demo !!!)
endif
	$(CC) $(CFLAGS) concurrency/$(TARGET).c -o concurrency/$(TARGET)

clean:
	for dir in $(DIRS) concurrency; do rm -f $$dir/$$dir $$dir/lock-ds $$dir/mt-demo; done
