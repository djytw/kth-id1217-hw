#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"
#include "string.h"

char *f[]={
    "aa", "b", "c", "d", "e", "g", "f", "k", "m", "n", "ppp", "ss", "sss", "cxxx"
};
char *g[]={
    "a", "b", "cc", "dd", "e", "f"
};
char *h[]={
    "a", "b", "c", "dd", "e", "g", "f"
};
char** Names[3] = {f,g,h};

int check(char** n, char* s){
    int i = 0;
    while(n[i]){
        if (!strcmp(n[i],s)) return 1;
        else i++;
    }
    return 0;
}


int main(int argc,char **argv) {
	int size, rank;
	unsigned char *avail_students;
    MPI_Status status;
	
	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (size != 3){
        printf("Size should equals to 3!\n");
        return 0;
    }
    char **names = Names[rank];
    char recv[100];
    int count = 0;
    int ignore0 = 0, ignore1 = 0, ignore2 = 0;
    char names1[100][100], names2[100][100];
    int names1_top = 0, names2_top = 0;
    check(Names[0],"s");
    while(1){
        // first - bcast a name to all
        if (ignore0 == 0 && names[count]){
            MPI_Send(names[count], sizeof(names[count]), MPI_CHAR, (rank+1)%3, 0, MPI_COMM_WORLD);
            MPI_Send(names[count], sizeof(names[count]), MPI_CHAR, (rank+2)%3, 0, MPI_COMM_WORLD);
        } else if (ignore0 == 0){
        // after send all names, send a stop signal, and skip this step
            char t = 0;
            MPI_Send(&t, 1, MPI_CHAR, (rank+1)%3, 0, MPI_COMM_WORLD);
            MPI_Send(&t, 1, MPI_CHAR, (rank+2)%3, 0, MPI_COMM_WORLD);
            ignore0 = 1;
        }
        if (!ignore1){
        // receive name from its right-side database
            MPI_Recv(recv, 100, MPI_CHAR, (rank+1)%3, 0, MPI_COMM_WORLD, &status);
            // receive stop signal, skip this step 
            if (recv[0] == 0) ignore1 = 1;
            else{
                if (check(names, recv)){
                    strcpy(names1[names1_top++], recv); 
                }
            }
        }
        if (!ignore2){
        // receive name from its left-side database
            MPI_Recv(recv, 100, MPI_CHAR, (rank+2)%3, 0, MPI_COMM_WORLD, &status);
            // receive stop signal, skip this step
            if (recv[0] == 0) ignore2 = 1;
            else {
                if (check(names, recv)){
                    strcpy(names2[names2_top++], recv); 
                }
            }
        }
        if (ignore0 !=0 && ignore1 != 0 && ignore2 != 0) break;
        count++;
    }
    // get same names names1 and names2, combine 2 tables:
    int i = 0, j = 0;
    for (i = 0; i < names1_top; i ++){
        for (j = 0; j < names2_top; j++){
            if (!strcmp(names1[i], names2[j]))
            printf("#%d discovers: %s\n", rank, names1[i]);
        }
    }
	MPI_Finalize();
	return 0;
}