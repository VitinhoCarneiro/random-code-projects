#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <omp.h>

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

int select_pivot(int32_t *array, int start, int end, int selection_method)
{
	switch(selection_method)
	{
		case  0: default:
			if((end - start) < ARRAYBIG) return median_of_3(array, start, end);
			return ninther(array, start, end);
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

int main(int argc, char **argv)
{
	int thread_count = 4, arraylen, pivot_sel_method_part = 0, pivot_sel_method_sort = 0, num_runs = 1, granulosity = 2, run, i;
	int part_thrds, slice;
	int32_t *array, *readarray;
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "--threads") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%u", &thread_count))
				{
					fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--threads\": `%s\', ignoring it.\n", argv[i]);
				}
			}
			else if(strcmp(argv[i], "--runs") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%u", &num_runs))
				{
					fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--threads\": `%s\', ignoring it.\n", argv[i]);
				}
			}
			else if(strcmp(argv[i], "--granulosity") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%u", &granulosity))
				{
					fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--granulosity\": `%s\', ignoring it.\n", argv[i]);
				}
			}
			

		}
	}

	char inputbuf[16];
	readarray = malloc(sizeof(int32_t) * 8);
	if(!readarray)
	{
		fprintf(stderr, "%s", err_oom);
		exit(1);
	}
	int arraysize = 8;
	arraylen = 0;
	if(!fgets(inputbuf, 16, stdin)) exit(0);
	while(sscanf(inputbuf, "%d", &readarray[arraylen]) > 0)
	{
		arraylen++;
		if(arraylen == arraysize)
		{
			//fprintf(stderr, "realoccing to %d\n", arraysize << 1);
			arraysize = arraysize << 1;
			readarray = realloc(readarray, sizeof(int32_t) * arraysize);
			if(!readarray)
			{
				fprintf(stderr, "%s", err_oom);
				exit(1);
			}
		}
		if(!fgets(inputbuf, 16, stdin)) break;
	}
	
	array = malloc(sizeof(int32_t) * arraylen);
	if(!array)
	{
		fprintf(stderr, "%s", err_oom);
		exit(1);
	}

	int part_offsets[thread_count << granulosity], part_ends[thread_count << granulosity], new_offsets[thread_count << granulosity], new_ends[thread_count << granulosity];
	
	double totaltime = 0., start, end;
	long running_elem_dist[thread_count];
	memset(running_elem_dist, 0, sizeof(long) * thread_count);
	for(run = 0; run < num_runs; run++)
	{
		fprintf(stderr, "Starting run %d of %d...\r", run, num_runs);
		memcpy(array, readarray, sizeof(int32_t) * arraylen);
		start = omp_get_wtime();

		part_offsets[0] = 0;
		part_ends[0] = arraylen - 1;
		for(part_thrds = 1; part_thrds <= (thread_count << granulosity) >> 1; part_thrds = part_thrds << 1)
		{
			#pragma omp parallel for schedule(dynamic, 1) num_threads(thread_count)
			for(slice = 0; slice < part_thrds; slice++)
			{
				//int thread_index = omp_get_thread_num();
				//fprintf(stderr, "thread #%d of %d desu!\n", thread_index, thread_count);
				int working_offset = part_offsets[slice];
				int working_end = part_ends[slice];
				int pivot;
				if(working_end - working_offset > 2)
				{
					pivot = partition(array, working_offset, working_end, pivot_sel_method_part);
				}
				else pivot = working_end + 1;
				//fprintf(stderr, "thread at slice #%d split %d-%d at p#%d\n", slice, working_offset, working_end, pivot);
				new_offsets[slice << 1] = working_offset;
				new_offsets[(slice << 1) + 1] = pivot + 1;
				new_ends[slice << 1] = pivot - 1;
				new_ends[(slice << 1) + 1] = working_end;
			}
			memcpy(part_offsets, new_offsets, sizeof(int) * (thread_count << granulosity));
			memcpy(part_ends, new_ends, sizeof(int) * (thread_count << granulosity));
		}
		//fprintf(stderr, "now sorting\n");
		#pragma omp parallel for schedule(dynamic, 1) num_threads(thread_count)
		for(slice = 0; slice < part_thrds; slice++)
		{
			int thread_index = omp_get_thread_num();
			if(part_ends[slice] - part_offsets[slice] > 0)
			{
				quicksort(array, part_offsets[slice], part_ends[slice], pivot_sel_method_sort);
			}
			running_elem_dist[thread_index] += part_ends[slice] - part_offsets[slice] + 1 > 0 ? part_ends[slice] - part_offsets[slice] + 1 : 0;
		}
		end = omp_get_wtime();
		totaltime += end - start;
	}
		fprintf(stderr, "%d runs of sorting %d elements completed by %d threads in %.12lfs (+- %.12lfs)\nAvg time per run: %.15lfs (+- %.15lfs)\nThread element distribution average:\n", num_runs, arraylen, thread_count, totaltime, omp_get_wtick(), totaltime / (double) num_runs, omp_get_wtick() / (double) num_runs);
	for(i = 0; i < thread_count; i++)
	{
		fprintf(stderr, "Thread #%3d: %9d\n", i + 1, (int)(running_elem_dist[i] / num_runs));
	}
	for(i = 0; i < arraylen; i++)
	{
		printf("%d\n", array[i]);
	}
}

