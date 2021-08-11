#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#ifndef NUMNODES
#define NUMNODES  1000
#endif

#ifndef NUMT
#define NUMT      4
#endif

#define XMIN     -1.
#define XMAX      1.
#define YMIN     -1.
#define YMAX      1.

#define N	 0.7

//g++ main.c -o main -lm -fopenmp

float Height( int, int );	// function prototype

int main( int argc, char *argv[ ] )
{
#ifndef _OPENMP
	fprintf( stderr, "No OpenMP support!\n" );
	return 1;
#endif

	omp_set_num_threads( NUMT );

	// the area of a single full-sized tile:
	// (not all tiles are full-sized, though)

  float fullTileArea = (  ( ( XMAX - XMIN )/(float)(NUMNODES-1) )  *
				( ( YMAX - YMIN )/(float)(NUMNODES-1) )  );   
  float edgeTileArea = fullTileArea/2;
  float cornerTileArea = fullTileArea/4;
  
  float Vol_sum = 0;

	// sum up the weighted heights into the variable "volume"
	// using an OpenMP for loop and a reduction:
  
  double time0 = omp_get_wtime( );
#pragma omp parallel for default(none) shared(fullTileArea, edgeTileArea, cornerTileArea) reduction(+:Vol_sum)
  for( int i = 0; i < NUMNODES*NUMNODES; i++ )
  {
	  int iu = i % NUMNODES;
	  int iv = i / NUMNODES;
	  float z = Height( iu, iv );
	  
    if(iu % NUMNODES == 0 && iv % NUMNODES == 0){
        Vol_sum += z * cornerTileArea;
        //printf("acc_volume = %f \n", Vol_sum);
    }
    else if(iu % NUMNODES == 0 || iv % NUMNODES == 0){
        Vol_sum += z * edgeTileArea;
        //printf("acc_volume = %f \n", Vol_sum);
    }
    else{
        Vol_sum += z * fullTileArea;
        //printf("acc_volume = %f \n", Vol_sum);
    }
    
  }
  Vol_sum = 2 * Vol_sum; // volume should also include the volume below the xy platform.
  
  double time1 = omp_get_wtime( );
  double megaNumnodesPerSecond = (double)(NUMNODES * NUMNODES) / ( time1 - time0 ) / 1000000.;

  
	fprintf(stderr, "    The volume of the superquadric with N = %3.2f is %6.3f\n", (float)N, Vol_sum);
  fprintf(stderr, "    %2d threads  %d NUMNODES  megaNumnodesPerSecond = %6.3lf \n",
		NUMT, NUMNODES, megaNumnodesPerSecond);
  
}


float
Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	float x = -1.  +  2.*(float)iu /(float)(NUMNODES-1);	// -1. to +1.
	float y = -1.  +  2.*(float)iv /(float)(NUMNODES-1);	// -1. to +1.

	float xn = pow( fabs(x), (double)N );
	float yn = pow( fabs(y), (double)N );
	float r = 1. - xn - yn;
	if( r <= 0. )
	        return 0.;
	float height = pow( r, 1./(float)N );
	return height;
}