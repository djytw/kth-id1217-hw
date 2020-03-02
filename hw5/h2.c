#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"
#include "string.h"

int random_student(unsigned char* avail_students, int size){
    unsigned char *a_stu = (unsigned char*) malloc(size);
    int i, count = 0;
    for (i = 1; i < size; i++){
        if (avail_students[i] == 0xff) a_stu[count++] = i;
    }
    if (count == 0)return -1;// no available students
    return a_stu[rand()%count];
}

int main(int argc,char **argv) {
	int size, rank;
	unsigned char *avail_students;
    MPI_Status status;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    avail_students = (unsigned char*) malloc (size);
 
	if(rank != 0){
        // this is a Student
        MPI_Recv(avail_students, size, MPI_UNSIGNED_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        // check self
        if (avail_students[rank] == 0 || avail_students[rank] == 0xff){
            // not assigned. choose a partner
            int partner = random_student(avail_students, size);
            if (partner == -1)
                printf("Student %d pairs with %d\n", rank, rank);
            else
                printf("Student %d pairs with %d\n", rank, partner);
            avail_students[partner] = rank;
            avail_students[rank] = partner;
            //inform the partner
            MPI_Send(avail_students, size, MPI_UNSIGNED_CHAR, partner, 0, MPI_COMM_WORLD);
        }else{
            // already assigned, print partner
            printf("Student %d pairs with %d\n", rank, avail_students[rank]);
            // continue to anbother student
            int next = random_student(avail_students, size);
            // mark the student is unavailable
            avail_students[next] = 0;
            MPI_Send(avail_students, size, MPI_UNSIGNED_CHAR, next, 0, MPI_COMM_WORLD);
        }
        
	}else{
        // this is teacher
        int i, last;
        printf("%d Students:\n", size - 1);
        memset(avail_students, -1, size);
        // randomly choose a student to start
        int next = random_student(avail_students, size);
        // mark the student is unavailable
        avail_students[next] = 0;
        //start
        MPI_Send(avail_students, size, MPI_UNSIGNED_CHAR, next, 0, MPI_COMM_WORLD);
    }
	MPI_Finalize();
	return 0;
}