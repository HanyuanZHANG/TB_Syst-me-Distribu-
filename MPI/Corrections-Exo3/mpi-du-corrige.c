#include <stdio.h>
#include "mpi.h"
#include <dirent.h>

#include "const.h"


int main(int argc, char *argv[])
{
  MPI_Status status;
  int i,j, num, rank, size,  tag, next, from, nbslaves;
  int willingslave;
  int notjobfound ;
  FILE *fp;
  DIR *dirp;
  int notover;
  int  receivedsize;

  int partialsize, totalsize;
  char instr [100], base[256], cmd[256], buf[256];
  struct dirent *direntp;


  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
 
  /* Initialization */

  notover = 1;
  totalsize = 0;
  next = (rank + 1) % size;
  from = (rank + size - 1) % size;
  nbslaves = size -1;

  if (rank == 0) {

    /* Begin User Program - the master */

    dirp = opendir(argv[1]);
    if (dirp == NULL) {
      perror("Erreur sur ouverture repertoire");
      exit(1);
    }
    strcpy(base, "du -s ");
    strcat(base, argv[1]);
    strcat(base, "/");
    while (notover) {

	MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG , MPI_COMM_WORLD, &status);
    willingslave = status.MPI_SOURCE;
	printf("message from slave  number %d \n", willingslave);
    if ( status.MPI_TAG ==ASKFORWORK) {
	printf("message ask for work  \n");
      MPI_Recv(instr, 0, MPI_CHAR,willingslave,  ASKFORWORK, MPI_COMM_WORLD, &status);

    notjobfound = 1;
     while (notjobfound) {
	if ( (direntp = readdir( dirp )) != NULL ) {
	  if(!strcmp(direntp->d_name, "."))
	    {  continue;}
	  if(!strcmp(direntp->d_name, ".."))
	    { continue;}
	  strcpy(cmd, base);
	  strcat(cmd, direntp->d_name);
    notjobfound = 0;
	}
	else {
	  notover = 0;
          notjobfound = 0;
	  strcpy (cmd, "du -s /tmp");
	}
	}


	printf("I 've sent  %s to  slave number %d \n",cmd, willingslave);
	printf("I 've sent  length %d \n",strlen(cmd));
	printf("Process sending %s to %d\n", cmd, willingslave );
	MPI_Send(cmd, strlen(cmd), MPI_CHAR, willingslave, WORK, MPI_COMM_WORLD); 

      }
    else if (status.MPI_TAG == RESULT) {
      /* Wait for results from slaves */
        printf("attente  maitre donnees depuis noeud  %d\n", willingslave);
	MPI_Recv(&partialsize, 1, MPI_INT, willingslave, RESULT, MPI_COMM_WORLD, &status);
	totalsize = totalsize + partialsize;
	printf("I got %d from node %d \n",partialsize, i);
	printf("Total %d  \n",totalsize);

    }
    else {
    printf("erreur\n");
    }
    }
  
    printf("calcul termine pour le maitre\n");
    (void)closedir( dirp );
    printf("close dir  pour le maitre\n");

    for( i=1 ; i < nbslaves+1 ; i++ ) {
      MPI_Send(cmd, 0, MPI_CHAR, i, WORK, MPI_COMM_WORLD); }

  }
  

  else {


    while (notover) {

      size = 0;
      /* Receive data from master */

      /* do not forget that the data received in an array and that its contents
	 will not be reset or reinitialized */
      MPI_Send(cmd, 0, MPI_CHAR, 0, ASKFORWORK, MPI_COMM_WORLD); 

      strcpy(instr, "                                                                                   ");
      MPI_Recv(instr, 100, MPI_CHAR, 0, WORK, MPI_COMM_WORLD, &status);

      /* we call MPI_Get8count to determine the size of the message received */

      MPI_Get_count (&status, MPI_CHAR, &receivedsize);
      if ( receivedsize== 0) {notover = 0;}
      else {



	fp = popen(instr, "r");
	while (fgets(buf, BUFSIZ, fp) != NULL) {
	  size = size + atoi(buf);
	}
	fclose(fp);

	/* Send result to master */
	MPI_Send(&size, 1, MPI_INT, 0, RESULT, MPI_COMM_WORLD); 
      }
    }

  }
  printf("calcul finalise\n");

  MPI_Finalize();
  return 0;
}
 
