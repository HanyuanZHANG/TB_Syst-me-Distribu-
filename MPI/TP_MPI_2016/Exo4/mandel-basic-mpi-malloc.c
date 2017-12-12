#include <stdio.h>
#include "mpi.h"

#if defined(__GNUC__) && (__GNUC__ >= 3)
#define ATTRIBUTE(x) __attribute__(x)
#else
#define ATTRIBUTE(x) /**/
#endif

#define MIN(_x, _y) ((_x) > (_y) ? (_y) : (_x))
#define ABS(_x) ((_x) >= 0 ? (_x) : -(_x))


/* N'hesitez pas a changer MAXX .*/
#define MAXX  502
#define MAXY (MAXX * 3 / 4)

#define NX (2 * MAXX + 1)
#define NY (2 * MAXY + 1)

#define NBITER 550
#define DATATAG 150
#define BLOCK 5

static int mandel(double, double);

int dump_ppm(const char *, int[NX][NY]);
int cases[NX][NY];


int main(int argc, char *argv[])
{
  MPI_Status status;
  int i,j, num, rank, size, nbslaves;
  char inputstr [100],outstr [100];

  /* Start up MPI */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  nbslaves = size -1;

  //int **res_block;
  int i_max, max_itr;
  int res_block[BLOCK][NY];

  if (rank == 0) {
    printf("enter rank 0");
    //int res[NY];
    if(NX%BLOCK){
        max_itr = NX/BLOCK +1;    
    }else{
        max_itr = NX/BLOCK;    
    }

    /* Begin User Program  - the master */
    for(int itr=0; itr<max_itr; itr++){

/*
       if((NX%BLOCK) && itr == (max_itr-1)){
           res_block = malloc( (NX%BLOCK) * sizeof( int * ));
           for(int i=0; i<NX%BLOCK; i++){
               res_block[i] = malloc(NY * sizeof(int));
           }
           i_max = NX;
           MPI_Recv(res_block, (NX%BLOCK)*NY, MPI_INT, 1, DATATAG, MPI_COMM_WORLD, &status); 
       }else{
           res_block = malloc( BLOCK * sizeof( int * ));
           for(int i=0; i<BLOCK; i++){
               res_block[i] = malloc(NY * sizeof(int));
           }
           i_max = (itr+1)*BLOCK;
           MPI_Recv(res_block, BLOCK*NY, MPI_INT, 1, DATATAG, MPI_COMM_WORLD, &status); 
       }
*/


        //printf("i_max now is *************-> %d\n", i_max);
        MPI_Recv(&res_block, BLOCK*NY, MPI_INT, 1, DATATAG, MPI_COMM_WORLD, &status); 
        //put value
        for(i=itr*BLOCK; i<(itr+1)*BLOCK; i++){
            for(j = -MAXY; j <= MAXY; j++) {
                cases[i][j + MAXY] = res_block[i%BLOCK][j + MAXY];
                /*
                if(itr==(max_itr-1)){
                    printf("m : %d", res_block[i%BLOCK][j + MAXY]);  
                }  */
            }             
        }
   }
   /*
   for(i = -MAXX; i <= MAXX; i++) {
        MPI_Recv(&res, 2*MAXY, MPI_INT, 1, DATATAG, MPI_COMM_WORLD, &status);      
        for(j = -MAXY; j <= MAXY; j++) {
          cases[i + MAXX][j + MAXY] = res[j + MAXY];
        }
    }*/
    dump_ppm("mandel.ppm", cases);
    printf("Fini.\n");
  }

  else {

    /* On est l'un des fils */
    double x, y;
    int i, j, res, rc;
   
    int max_itr;
    //int res_line[2*MAXY];
    if(NX%BLOCK){
        max_itr = NX/BLOCK +1;    
    }else{
        max_itr = NX/BLOCK;    
    }
    //int **res_block;
    int i_max, count;
    
    int res_block[BLOCK][NY];
    printf("rest for NX %d, BLOCK %d, max itr %d\n", NX, BLOCK, max_itr); 
   
    for(int itr=0; itr<max_itr; itr++) {
          /*if(NX%BLOCK && itr==(max_itr-1)){
              printf("test if 1\n");
              res_block = malloc( (NX%BLOCK) * sizeof( int * ));
              for(int row=0; row<NX%BLOCK; row ++){
                res_block[row] = malloc(NY * sizeof(int));
              }
              i_max = NX;
              count = (NX%BLOCK) * NY;        
          }else{
              res_block = malloc( BLOCK * sizeof( int * ));
              for(int row=0; row<BLOCK; row ++){
                res_block[row] = malloc(NY * sizeof(int));
              }
              i_max = (itr+1)*BLOCK;
              count = BLOCK * NY; 
              //printf("propagate to i_max :%d\n", i_max);
          }*/
          
          for(i=itr*BLOCK; i<(itr+1)*BLOCK; i++){

              //res_block[i%BLOCK] = malloc(NY * sizeof(int));

             // printf("propagate to i : %d, with row index %d\n", i, i%BLOCK);
              for(j = -MAXY; j <= MAXY; j++) {
                x = 2 * (i-MAXX) / (double)MAXX;
                y = 1.5 * j / (double)MAXY;
                res = mandel(x, y);

                //res_line[j+MAXY] = res;
                //printf("write in row %d, in column %d\n", i%BLOCK, j+MAXY);

                res_block[i%BLOCK][j+MAXY] = res;  
                //printf("count %d, block : %d\n", count, i%BLOCK); 
/*  
                if(itr==(max_itr-1)){
                    printf("s : %d", res);  
                }      */
              }
          }
          MPI_Send(&res_block, BLOCK*NY, MPI_INT, 0, DATATAG, MPI_COMM_WORLD); 
      }


    }

  MPI_Finalize();
  return 0;
}



/* function to compute a point - the number of iterations 
   plays a central role here */

int
mandel(double cx, double cy)
{
  int i;
  double zx, zy;
  zx = 0.0; zy = 0.0;
  for(i = 0; i < NBITER; i++) {
    double zxx = zx, zyy = zy;
    zx = zxx * zxx - zyy * zyy + cx;
    zy = 2 * zxx * zyy + cy;
    if(zx * zx + zy * zy > 4.0)
      return i;
  }
  return -1;
}

/* the image commputer can be transformed in a ppm file so
   to be seen with xv */

int
dump_ppm(const char *filename, int valeurs[NX][NY])
{
  FILE *f;
  int i, j, rc;

  f = fopen(filename, "w");
  if(f == NULL) { perror("fopen"); exit(1); }
  fprintf(f, "P6\n");
  fprintf(f, "%d %d\n", NX, NY);
  fprintf(f, "%d\n", 255);
  for(j = NY - 1; j >= 0; j--) {
    for(i = 0; i < NX; i++) {
      unsigned char pixel[3];
      if(valeurs[i][j] < 0) {
	pixel[0] = pixel[1] = pixel[2] = 0;
      } else {
	unsigned char val = MIN(valeurs[i][j] * 12, 255);
	pixel[0] = val;
	pixel[1] = 0;
	pixel[2] = 255 - val;
      }
      rc = fwrite(pixel, 1, 3, f);
      if(rc < 0) { perror("fwrite"); exit(1); }
    }
  }
  fclose(f);
  return 0;
}
 
