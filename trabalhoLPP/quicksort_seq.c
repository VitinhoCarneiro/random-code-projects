#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
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

int select_pivot(int32_t *array, int start, int end)
{
	if((end - start) < ARRAYBIG) return median_of_3(array, start, end);
	return ninther(array, start, end);
}

int partition(int32_t *array, int start, int end)
{
	int pivot = select_pivot(array, start, end);
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

void quicksort(int32_t *array, int start, int end)
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
	int pivot = partition(array, start, end);
	//fprintf(stderr, "recursing from %d %d\n", start, end);
	quicksort(array, start, pivot - 1);
	quicksort(array, pivot + 1, end);
}

const char *err_oom = "(main) Error: not enough memory to initialize array. Exiting.\n";

int main(int argc, char** argv)
{
	int arraylen = 0, arraysize = 8, i, num_runs, run;
	if(argc > 1)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "--runs") == 0)
			{
				i++;
				if(i >= argc) break;
				if(!sscanf(argv[i], "%u", &num_runs))
				{
					fprintf(stderr, "(main, thread 0) Error: Invalid number for argument \"--threads\": `%s\', ignoring it.\n", argv[i]);
				}
			}
		}
	}
	char inputbuf[16];
	int32_t *array = malloc(sizeof(int32_t) * 8);
	if(!array)
	{
		fprintf(stderr, "%s", err_oom);
		exit(1);
	}
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
				exit(1);
			}
		}
		if(!fgets(inputbuf, 16, stdin)) break;
	}
	//fprintf(stderr, "got it\n");
	double time, start, end;
	for(run = 0; run < num_runs; run++)
	{
		fprintf(stderr, "Starting run %d of %d\r", run, num_runs);
		start = omp_get_wtime();
		quicksort(array, 0, arraylen - 1);
		end = omp_get_wtime();
		time += end - start;
	}
	fprintf(stderr, "Done sorting %d runs in %.12lf seconds;\nAvg %.15lf seconds per iteration\n", num_runs, time, time / (double)num_runs);
	for(i = 0; i < arraylen; i++) printf("%d\n", array[i]);
}

