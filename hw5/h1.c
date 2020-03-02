#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"
 
int main(int argc,char **argv) {
	int size, rank;
	int send[1];
	int recv[1];
    MPI_Status status;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    *send = rank;
 
	if(rank != 0){
        // this is a Student
        // first send the request to teacher
        MPI_Send(send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        // then wait teacher to assign groups
        MPI_Recv(recv, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
        printf("Student %d pairs with %d\n", rank, *recv);
	}else{
        int i, last;
        printf("%d Students:\n", size - 1);
        for (i = 0; i < size - 1; i ++){
            // receive every request from student
            MPI_Recv(recv, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            
            if (i % 2){
                // if it's the second request, send group information to both of the students.
                *send = last;
                MPI_Send(send, 1, MPI_INT, *recv, 1, MPI_COMM_WORLD);
                *send = *recv;
                MPI_Send(send, 1, MPI_INT, last, 1, MPI_COMM_WORLD);
            }else{
                // if it's the first request, record the student.
                last = *recv;
            }
            if (size%2 == 0 && i == size - 2){
                // if there's odd number of students, let the last student group with himself/herself
                *send = last;
                MPI_Send(send, 1, MPI_INT, last, 1, MPI_COMM_WORLD);
            }
        }
    }
	MPI_Finalize();
	return 0;
}