/******************************************************************************
* FILE: mpi_bug5.c
* DESCRIPTION: 
*   This is an "unsafe" program. It's behavior varies depending upon the
*   platform and MPI library
* AUTHOR: Blaise Barney 
* LAST REVISED: 01/24/09
******************************************************************************/
#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define MSGSIZE 2000

int main (int argc, char *argv[])
{
int           numtasks, rank, tag=111, dest=1, source=0, count=0, ack = 0;
long int i;
char          data[MSGSIZE];
double        start, end, result;
MPI_Status    status;
MPI_Request   req;

MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

if (rank == 0) {
  printf ("mpi_bug5 has started...\n");
  if (numtasks > 2) 
    printf("INFO: Number of tasks= %d. Only using 2 tasks.\n", numtasks);
  }

/******************************* Send task **********************************/
if (rank == 0) {

  /* Initialize send data */
  for(i=0; i<MSGSIZE; i++)
     data[i] =  'x';

  start = MPI_Wtime();
  while (1) {
    MPI_Isend(data, MSGSIZE, MPI_BYTE, dest, tag, MPI_COMM_WORLD, &req);
    MPI_Recv(&ack, 1, MPI_INT, dest, 120, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    count++;
    if (count % 10 == 0) {
      end = MPI_Wtime();
      printf("Count= %d  Time= %f sec.\n", count, end-start);
      start = MPI_Wtime();
      }
    }
    MPI_Wait(&req, &status);
  }

/****************************** Receive task ********************************/

if (rank == 1) {
  while (1) {
    MPI_Recv(data, MSGSIZE, MPI_BYTE, source, tag, MPI_COMM_WORLD, &status);
    MPI_Isend(&ack, 1, MPI_INT, source, 120, MPI_COMM_WORLD, &req);
    /* Do some work  - at least more than the send task */
    result = 0.0;
    for (i=0; i < 1000000; i++) 
      result = result + (double)random();
    //fprintf(stderr, "Loop done.\n");
    }
    MPI_Wait(&req, &status);
  }

MPI_Finalize();
}

