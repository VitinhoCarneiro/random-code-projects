#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

// (sub-)arrays below this size will employ median-of-3 for pivot selection;
// above this threshold, median-of-median-of-3 will be used.
#define ARRAYBIG 256



int three_index_median(int32_t *a, int e1, int e2, int e3)
{
	if(a[e3] >= a[e2])
	{
		if(a[e2] >= a[e1]) return a[e2];
		else if(a[e3] >= a[e1]) return a[e1];
		else return a[e3];
	}
	else if(a[e1] >= a[e2]) return a[e2];
	else if(a[e1] >= a[e3]) return a[e1];
	else return a[e3];
}

int median3(int a1, int a2, int a3)
{
	if(a1 >= a2)
	{
		if(a2 >= a1) return a2;
		else if(a3 >= a1) return a1;
		else return a3;
	}
	else if(a1 >= a2) return a2;
	else if(a1 >= a3) return a1;
	else return a3;
}

int median_of_3(int32_t *a, int start, int end)
{
	return three_index_median(a, start, (start + end) >> 1, end);
}

int ninther(int32_t *a, int start, int end)
{
	int seglen = (end - start) / 3;
	return median3(median_of_3(a, start, start + seglen),
	               median_of_3(a, start + seglen, start + (seglen << 1)),
	               median_of_3(a, start + (seglen << 1), end));
}

int recursive_median_of_3(int32_t *a, int start, int end, int depth)
{
	if(depth <= 1) return median_of_3(a, start, end);	
	int seglen = (end - start) / 3;
	return median3(recursive_median_of_3(a, start, start + seglen, depth - 1),
	               recursive_median_of_3(a, start + seglen, start + (seglen << 1), depth - 1),
	               recursive_median_of_3(a, start + (seglen << 1), end, depth - 1));
}

int recursive_3_median_random_sampl(int32_t *a, int start, int end, int depth)
{
	int alen = end - start;
	if(depth <= 1) 
	{
		if(alen > 0) return three_index_median(a, start + rand() % alen, start + rand() % alen, start + rand() % alen);
		else return a[start];
	}
	int seglen = (end - start) / 3;
	return median3(recursive_3_median_random_sampl(a, start, start + seglen, depth - 1),
	               recursive_3_median_random_sampl(a, start + seglen, start + (seglen << 1), depth - 1),
	               recursive_3_median_random_sampl(a, start + (seglen << 1), end, depth - 1));
}

int select_pivot(int32_t *array, int start, int end, int selection_method)
{
	switch(selection_method)
	{
		case  0: default:
			if((end - start) < ARRAYBIG) return median_of_3(array, start, end);
			return ninther(array, start, end);
		case  1: return median_of_3(array, start, end);
		case  2: return ninther(array, start, end);
		case  3: return recursive_median_of_3(array, start, end, 3);
		case  4: return recursive_median_of_3(array, start, end, 4);
		case -1: if(end - start < ARRAYBIG) return ninther(array, start, end);
			 return recursive_median_of_3(array, start, end, 3);
		case -2: return recursive_3_median_random_sampl(array, start, end, 2);
		case -3: return recursive_3_median_random_sampl(array, start, end, 3);
		case -4: return recursive_3_median_random_sampl(array, start, end, 4);
		case -5: if(end - start < ARRAYBIG) return recursive_3_median_random_sampl(array, start, end, 2);
			 return recursive_3_median_random_sampl(array, start, end, 3);
		case  5: return recursive_3_median_random_sampl(array, start, end, 1);
		case  6: return array[start];
		case  7: return array[(start + end) >> 1];
	}
}

int partition(int32_t *array, int start, int end, int selection_method)
{
	int pivot = select_pivot(array, start, end, selection_method);
	int i = start, j = end, aux;
	while(1)
	{
		while(array[i] < pivot) i++;
		while(array[j] > pivot) j--;
		if(i >= j) return j;
		aux = array[i];
		array[i] = array[j];
		array[j] = aux;
	}
}

void quicksort(int32_t *array, int start, int end, int pivot_selection_method)
{
	if(end <= start) return;
	if(end - start == 1)
	{
		if(array[start] > array[end])
		{
			int aux = array[start];
			array[start] = array[end];
			array[end] = aux;
		}
		return;
	}
	int pivot = partition(array, start, end, pivot_selection_method);
	//fprintf(stderr, "recursing from %d %d\n", start, end);
	quicksort(array, start, pivot - 1, pivot_selection_method);
	quicksort(array, pivot + 1, end, pivot_selection_method);
}

const char *err_oom = "(main) Error: not enough memory to initialize array. Exiting.\n";

int parent(int thread_index)
{
	int i;
	for(i = 1; i <= thread_index; i = i << 1){
		if(thread_index & i) return thread_index ^ i;
	}
	return -1;
}

int first_child(int thread_index, int thread_count)
{
	if(thread_count == 1) return thread_index;
	int i;
	for(i = 1; i < thread_count; i = i << 1){
		if(thread_index & i) return thread_index ^ (i >> 1);
	}
	return thread_index ^ (i >> 1);
}

int next_child(int child_index)
{
	int i;
	for(i = 1; i <= child_index; i = i << 1){
		if(child_index & i) return child_index ^ (i | i >> 1);
	}
	return child_index;
}

int main(int argc, char **argv)
{
	int thread_index, thread_count, arraylen, child_arraylen, child, pivot, i, array_recv_offset, pivot_sel_method_part = 0, pivot_sel_method_sort = 0;
	int32_t *array;
	char *statfilename = NULL;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &thread_count);
	MPI_Comm_rank(MPI_COMM_WORLD, &thread_index);
	srand(time(NULL));
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "--part_method") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%d", &pivot_sel_method_part))
				{
					if(thread_index == 0) fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--part_method\": `%s\', ignoring it.\n", argv[i]);
				}
			}
			else if(strcmp(argv[i], "--pivot_method") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%d", &pivot_sel_method_part))
				{
					if(thread_index == 0) fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--pivot_method\": `%s\', ignoring it.\n", argv[i]);
				}else if(pivot_sel_method_part == 0) pivot_sel_method_part = pivot_sel_method_sort;
			}
			else if(strcmp(argv[i], "--stats_file") == 0)
			{
				i++;
				if(i >= argc) break;
				statfilename = argv[i];
			}
		}
	}
	if(thread_index == 0)
	{
		if((thread_count & (thread_count - 1)) != 0)
		{
			fprintf(stderr, "(main, thread 0) Error: detected %d active threads, but number of threads must be a power of two. Exiting.", thread_count);
			MPI_Abort(MPI_COMM_WORLD, 1);
			exit(1);
		}
		char inputbuf[16];
		array = malloc(sizeof(int32_t) * 8);
		if(!array)
		{
			fprintf(stderr, "%s", err_oom);
			MPI_Abort(MPI_COMM_WORLD, 1);
			exit(1);
		}
		int arraysize = 8;
		arraylen = 0;
		if(!fgets(inputbuf, 16, stdin)) exit(0);
		while(sscanf(inputbuf, "%d", &array[arraylen]) > 0)
		{
			arraylen++;
			if(arraylen == arraysize)
			{
				//fprintf(stderr, "realoccing to %d\n", arraysize << 1);
				arraysize = arraysize << 1;
				array = realloc(array, sizeof(int32_t) * arraysize);
				if(!array)
				{
					fprintf(stderr, "%s", err_oom);
					MPI_Abort(MPI_COMM_WORLD, 1);
					exit(1);
				}
			}
			if(!fgets(inputbuf, 16, stdin)) break;
		}
	}
	else
	{
		MPI_Recv(&arraylen, 1, MPI_INT, parent(thread_index), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		array = malloc(sizeof(int32_t) * arraylen);
		fprintf(stderr, "[thread %d] Receiving array from thread %d with arraylen=%d\n", thread_index, parent(thread_index), arraylen);
		MPI_Recv((void*) array, arraylen, MPI_INT, parent(thread_index), 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	child = first_child(thread_index, thread_count);
	while(child != thread_index)
	{
		if(arraylen < 2) child_arraylen = 0;
		else
		{
			pivot = partition(array, 0, arraylen - 1, pivot_sel_method_part);
			child_arraylen = arraylen - pivot;
		}
		fprintf(stderr, "[thread %d] Sending array to thread %d with arraylen=%d\n", thread_index, child, child_arraylen);
		MPI_Send(&child_arraylen, 1, MPI_INT, child, 0, MPI_COMM_WORLD);
		MPI_Send((void*) &(array[pivot]), arraylen - pivot, MPI_INT, child, 1, MPI_COMM_WORLD);
		arraylen -= arraylen - pivot;
		child = next_child(child);
	}
	if(arraylen < 2) fprintf(stderr, "[thread %d] arraylen=%d, nothing to sort.\n", thread_index, arraylen);
	else
	{
		fprintf(stderr, "[thread %d] Sorting subarray of size %d\n", thread_index, arraylen);
		quicksort(array, 0, arraylen - 1, pivot_sel_method_sort);
	}
	if(thread_index > 0)
	{
		fprintf(stderr, "[thread %d] Done sorting, sending back arraylen=%d to thread 0\n", thread_index, arraylen);
		MPI_Send(&arraylen, 1, MPI_INT, 0, thread_index, MPI_COMM_WORLD);
		fprintf(stderr, "[thread %d] Sending back array to thread 0 with arraylen=%d\n", thread_index, arraylen);
		MPI_Send((void*) array, arraylen, MPI_INT, 0, thread_index + thread_count, MPI_COMM_WORLD);
		fprintf(stderr, "[thread %d] Done, finalizing.\n", thread_index);
		MPI_Finalize();
	}
	else
	{
		int32_t *arraylens = malloc(sizeof(int32_t) * (thread_count - 1));
		MPI_Request *requests = malloc(sizeof(MPI_Request) * (thread_count - 1));
		MPI_Status *stati = malloc(sizeof(MPI_Status) * (thread_count - 1));
		fprintf(stderr, "[thread %d] Done sorting, receiving array lengths from all threads...\n", thread_index);
		for(i = 1; i < thread_count; i++)
		{
			MPI_Irecv((void*) &(arraylens[i - 1]), 1, MPI_INT, i, i, MPI_COMM_WORLD, &requests[i - 1]);
		}
		MPI_Waitall(thread_count - 1, requests, stati);
		array_recv_offset = arraylen;
		fprintf(stderr, "[thread %d] Receiving arrays from threads...\n", thread_index);
		for(i = 1; i < thread_count; i++)
		{
			MPI_Irecv((void*) &(array[array_recv_offset]), arraylens[i - 1], MPI_INT, i, i + thread_count, MPI_COMM_WORLD, &requests[i - 1]);
			array_recv_offset += arraylens[i - 1];
		}
		MPI_Waitall(thread_count - 1, requests, stati);
		for(i = 0; i < array_recv_offset; i++) printf("%d\n", array[i]);
		FILE* statfile;
		if(statfilename)
		{
			statfile = fopen(statfilename, "w");
			fprintf(statfile, "Array lengths per thread:\n");
			fprintf(statfile, "%3d, %9d\n", 0, arraylen);
			for(i = 1; i < thread_count; i++)
			{
				fprintf(statfile, "%3d, %9d\n", i, arraylens[i - 1]);
			}
			fclose(statfile);
		}
		fprintf(stderr, "[main, thread 0] -------------------- Stats: -----------------------\n");
		fprintf(stderr, "Array lengths per thread:\n");
		fprintf(stderr, "> Thread   0: %9d\n", arraylen);
		for(i = 1; i < thread_count; i++)
		{
			fprintf(stderr, "> Thread %3d: %9d\n", i, arraylens[i - 1]);
		}
		MPI_Finalize();
	}
}

