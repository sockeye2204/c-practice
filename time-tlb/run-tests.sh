#!/bin/bash

for ((i = 1; i <= 4096; i *= 2)); do
    for j in {1..5}; do
	     ./time-tlb $i > "tests/$i-pages-test-$j.txt"
    done
done
