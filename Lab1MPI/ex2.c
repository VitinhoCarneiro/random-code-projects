#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mpi.h"

#define MASTER 0

int main(int argc, char** argv){
	int rank, threads, source, destination, recvbuf;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &threads);
	if(threads & 1) exit(1);
	destination = rank >= (threads >> 1) ? rank - (threads >> 1) : (threads >> 1) + rank;
	source = rank >= (threads >> 1) ? (rank - (threads >> 1)) : (threads >> 1) + rank;
	if(rank == 0){
		printf("Master is up.\n");
	}
	MPI_Request r;
	if(rank >= (threads >> 1)){
		MPI_Isend(&rank, 1, MPI_INT, destination, 0, MPI_COMM_WORLD, &r);
		printf("Task #%d sent message to task #%d.\n", rank, destination);
		MPI_Wait(&r, MPI_STATUS_IGNORE);
		printf("Task #%d's message to task %d got received.\n", rank, destination);
		MPI_Recv(&recvbuf, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Task #%d got message from task #%d.\n", rank, source);
	}else{
		MPI_Recv(&recvbuf, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("Task #%d got message from task #%d.\n", rank, source);
		MPI_Isend(&rank, 1, MPI_INT, destination, 0, MPI_COMM_WORLD, &r);
		printf("Task #%d sent message to task #%d.\n", rank, destination);
		MPI_Wait(&r, MPI_STATUS_IGNORE);
		printf("Task #%d's message to task %d got received.\n", rank, destination);
	}
	MPI_Finalize();
	return 0;
}
