#!/bin/bash

echo "Memory analysis..."
for i in {0..10}; do
	echo -e "\e[31m Now checking with i = $i Original:\e[0m"
	valgrind --tool=memcheck ./memory_orig $i
	
	echo -e '\e[31m Modified:\e[0m'
	valgrind --tool=memcheck ./memory_mod $i
done

# Slightly more precise: valgrind --tool=massif ./memory_orig i
# valgrind --tool=massif ./memory_mod i
# and analyze heap allocation with ms_print but crappy to automate