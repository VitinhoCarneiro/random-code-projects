#!/usr/bin/bash

gcc -Wall -Wextra -Werror -fopenmp -o quicksort_seq quicksort_seq.c
gcc -Wall -Wextra -Werror -fopenmp -o quicksort_omp quicksort_omp.c

seq 100 | shuf | ./quicksort_seq --runs 100000 > /dev/null;
seq 3000 | shuf | ./quicksort_seq --runs 3333 > /dev/null;
seq 100000 | shuf | ./quicksort_seq --runs 100 > /dev/null;

echo "\n\n\n\n\n\n------ Parallel tests ------"

for threads in 1 2 4;
	do seq 100 | shuf | ./quicksort_omp --threads $threads --runs 100000 --granulosity 2 > /dev/null;
done

for threads in 1 2 4;
	do seq 3000 | shuf | ./quicksort_omp --threads $threads --runs 3333 --granulosity 2 > /dev/null;
done

for threads in 1 2 4;
	do seq 100000 | shuf | ./quicksort_omp --threads $threads --runs 100 --granulosity 2 > /dev/null;
done

echo "\n\n\n\n\n\n------ Granulosity tests -------"
for threads in 1 2 4;
	do for granulosity in 0 1;
		do seq 100000 | shuf | ./quicksort_omp --threads $threads --runs 100 --granulosity $granulosity > /dev/null;
	done
done
