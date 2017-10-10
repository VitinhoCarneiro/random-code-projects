#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char** argv){
	int rank, threads, i, j, bArraySize;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &threads);
	bArraySize = (int)(16000000 / threads) * threads;
	printf("Oin %d\n", rank);
	float array[(int)(bArraySize / threads)], *bigArray, sums[threads], sum = 0.f;
	printf("Oin2 %d\n", rank);
	if(rank == 0){
		printf("Captain Roberto Nascimento is up. You better move. Mission given, mission accomplished.\n");
		bigArray = malloc(sizeof(float) * bArraySize);
		for(i = 0; i < bArraySize; i++){
			bigArray[i] = (float)(i);
		}
	}
	printf("Oin3 %d\n", rank);
	MPI_Scatter(bigArray, bArraySize / threads, MPI_FLOAT, array, bArraySize / threads, MPI_FLOAT, 0, MPI_COMM_WORLD);
	printf("Oin4 %d\n", rank);
	printf("Process %d got array.\n", rank);
	for(i = 0; i < bArraySize / threads; i++){
		array[i] = array[i] + (float)(i);
		sum += array[i];
	}
	MPI_Gather(array, bArraySize / threads, MPI_FLOAT, bigArray, bArraySize / threads, MPI_FLOAT, 0, MPI_COMM_WORLD);
	MPI_Gather(&sum, 1, MPI_FLOAT, sums, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
	if(rank == 0){
		printf("Captain Roberto presents you the results:\n\n");
		for(i = 0; i < threads; i++){
			printf("Thread #%d:\nFirst 5 elements: [", i);
			for(j = 0; j < 5; j++){
				printf("%9.1f ", bigArray[i * bArraySize / threads + j]);
			}
			printf("]\nSum = %11.1f\n\n", sums[i]);
		}
		printf("END OF REPORT. MISSION ACCOMPLISHED.\n");
	}
}
