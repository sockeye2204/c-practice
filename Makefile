# ----------------------------
# Directories / Targets
# ----------------------------
DIRS := cmd-runner mlfq-scheduler paging-policy tcp time-tlb checksum tlpi

.PHONY: all clean concurrency $(DIRS)

all: $(DIRS)

# ----------------------------
# Compiler settings
# ----------------------------
CC := gcc
CFLAGS := -Wall -Wextra -O2

# ----------------------------
# General directories (except checksum and tlpi)
# ----------------------------
$(filter-out checksum tlpi, $(DIRS)):
	$(CC) $(CFLAGS) $(wildcard $@/*.c) -o $@/$@

# ----------------------------
# Checksum executables
# ----------------------------
checksum: $(patsubst checksum/%.c, checksum/%, $(wildcard checksum/*.c))

checksum/%: checksum/%.c
	$(CC) $(CFLAGS) $< -o $@

# ----------------------------
# TLPI executables
# ----------------------------
tlpi: $(patsubst tlpi/%.c, tlpi/%, $(wildcard tlpi/*.c))

tlpi/%: tlpi/%.c
	$(CC) $(CFLAGS) $< -o $@

# ----------------------------
# Concurrency executables
# ----------------------------
concurrency:
ifndef TARGET
	$(error Please specify TARGET=lock-ds or TARGET=mt-demo !!!)
endif
	$(CC) $(CFLAGS) concurrency/$(TARGET).c -o concurrency/$(TARGET)

# ----------------------------
# Clean
# ----------------------------
clean:
	for dir in $(filter-out tlpi checksum, $(DIRS)) concurrency; do \
		rm -f $$dir/$$dir $$dir/lock-ds $$dir/mt-demo; \
	done
	rm -f $(patsubst checksum/%.c, checksum/%, $(wildcard checksum/*.c))
	rm -f $(patsubst tlpi/%.c, tlpi/%, $(wildcard tlpi/*.c))
