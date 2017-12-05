#!/bin/bash
mkdir stats outputs
mpicc -Wall -Wextra -Werror -o quicksort_mpi quicksort_mpi.c
for pivmethod in `seq -5 7`; do
	for partmethod in `seq -5 7`; do
		seq 30000 | mpirun -np 32 -mca plm_rsh_no_tree_spawn 1 quicksort_mpi --pivot_method $pivmethod --part_method $partmethod --stats_file `printf "stats/partfun_piv%d_par%d.csv" $pivmethod $partmethod` | tee `printf "outputs/partfun_piv%d_par%d_30000el.out" $pivmethod $partmethod` > /dev/null;
	done;
done
for threads in 1 2 4 8 16 32 64; do
	for partmethod in `seq -5 7`; do
		for setsize in 30000 300000 3000000; do 
			seq $setsize | mpirun -np $threads -mca plm_rsh_no_tree_spawn 1 quicksort_mpi --pivot_method -1 --part_method $partmethod --stats_file `printf "stats/scalability_%dthr_par%d_%del.csv" $threads $partmethod $setsize` | tee `printf "outputs/scalability_%dthr_par%d_%del.out" $threads $partmethod $setsize` > /dev/null;
		done;
	done;
done
