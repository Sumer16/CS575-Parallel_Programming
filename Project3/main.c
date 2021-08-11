#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

//g++ main.c -o main -lm -fopenmp
unsigned int seed = 0;

int	NowYear;		// 2021 - 2026
int	NowMonth;		// 0 - 11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int	NowNumDeer;		// number of deer in the current population
int NowNumBeasts;

const float GRAIN_GROWS_PER_MONTH =		9.0;
const float ONE_DEER_EATS_PER_MONTH =		1.0;

const float AVG_PRECIP_PER_MONTH =		7.0;	// average
const float AMP_PRECIP_PER_MONTH =		6.0;	// plus or minus
const float RANDOM_PRECIP =			2.0;	// plus or minus noise

const float AVG_TEMP =				60.0;	// average
const float AMP_TEMP =				20.0;	// plus or minus
const float RANDOM_TEMP =			10.0;	// plus or minus noise

const float MIDTEMP =				40.0;
const float MIDPRECIP =				10.0;

const float Beast_Need_Water_parameter = 0.01;   

float
FtoC(float temperature)
{
    temperature = (5./9.)*(temperature-32);
    return temperature;
}

float
Ranf( unsigned int *seedp,  float low, float high )
{
        float r = (float) rand_r( seedp );              // 0 - RAND_MAX

        return(   low  +  r * ( high - low ) / (float)RAND_MAX   );
}

float
SQR( float x )
{
        return x*x;
}

float tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
float precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );

void
Deer()
{
    while(NowYear < 2027){
        int nextNumDeer = NowNumDeer;

        int carryingCapacity = (int)( NowHeight );

        if( nextNumDeer <= carryingCapacity){
            if( nextNumDeer >= NowNumBeasts){
                nextNumDeer++;
            }
            nextNumDeer++;
        }
        
        else{ // Grain cannot afford enough food.
            if( nextNumDeer < NowNumBeasts){
                nextNumDeer--;
            }
            nextNumDeer--;
        }
            
        if( nextNumDeer < 0 )
            nextNumDeer = 0;
        #pragma omp barrier
        
        NowNumDeer = nextNumDeer;
        #pragma omp barrier   
        // do nothing
        #pragma omp barrier
    }
}

void
Grain()
{
    while(NowYear < 2027){
        float nextHeight = NowHeight;
        nextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        nextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
        
        if( nextHeight < 0.)
            nextHeight = 0.;
        
        #pragma omp barrier
        
        NowHeight = nextHeight;
        #pragma omp barrier    
        // do nothing
        #pragma omp barrier
    }
}

void
Beasts()
{
    while(NowYear < 2027){
        int nextNumBeasts = NowNumBeasts;
        
        if( nextNumBeasts *2 <= NowNumDeer){
            nextNumBeasts++;
        }
        if( nextNumBeasts * 0.01 > NowPrecip || nextNumBeasts >= NowNumDeer) // Check if water is enough for Beasts.
            nextNumBeasts = nextNumBeasts - 2;
        
        
        if( nextNumBeasts < 0 )
            nextNumBeasts = 0;    
        #pragma omp barrier
        
        NowNumBeasts = nextNumBeasts;
        #pragma omp barrier   
        // do nothing
        #pragma omp barrier   
    }
}

void
Watcher()
{
    while(NowYear < 2027){
        // do nothing
        #pragma omp barrier
        // do nothing
        #pragma omp barrier
        // increment the date
        //printf("%d year  %d months\n", NowYear, NowMonth + 1);
        if(NowMonth == 11){
              NowMonth = 0;
              NowYear++;
        }
        else{
              NowMonth++;
        }
        // This part show the deers, grain, temperature, and rainfall
        //printf("  Current Deers = %d\n  Current Grain Height = %f\n  Current temperature = %f\n  Current Precipitation = %f\n", NowNumDeer, NowHeight, NowTemp, NowPrecip);
        printf("%d,%4.2f,%4.2f,%4.2f,%d\n", NowNumDeer, NowHeight, FtoC(NowTemp), NowPrecip, NowNumBeasts);
        
        //printf("=======================================\n");
      
        // This part calculate temperature and precipitation
        float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );

        float temp = AVG_TEMP - AMP_TEMP * cos( ang );
        NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
        
        float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
        NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP ) - (NowNumBeasts * Beast_Need_Water_parameter);
        if( NowPrecip < 0.)
        	  NowPrecip = 0.;
        // Update the constant value
        tempFactor = exp(   -SQR(  ( NowTemp - MIDTEMP ) / 10.  )   );
        precipFactor = exp(   -SQR(  ( NowPrecip - MIDPRECIP ) / 10.  )   );
        
        #pragma omp barrier
    }
}


int main(int argc, char* argv[])
{
      // starting date and time:
      NowMonth = 0;
      NowYear  = 2021;
      
      //Initialize Temperature and Precipitation
      float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
      float temp = AVG_TEMP - AMP_TEMP * cos( ang );
      NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );
      
      float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
      NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
      
      // starting state (feel free to change this if you want):
      NowNumDeer = 1;
      NowHeight =  1.; // Grain height
      NowNumBeasts = 0;
      omp_set_num_threads( 4 );	// same as # of sections
  
      #pragma omp parallel sections
      {
      	  #pragma omp section
      	  {
      	  	Deer();
      	  }
      
      	  #pragma omp section
        	{
      	  	Grain();
      	  }
      
      	  #pragma omp section
      	  {
      	  	Watcher();
      	  }
      
      	  #pragma omp section
      	  {
      		  Beasts();	// your own
      	  }
      }       // implied barrier -- all functions must return in order
    	  // to allow any of them to get past here
     return 0;  
       
}
 
 
 
 
 