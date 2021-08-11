#include <xmmintrin.h>
#include <stdio.h>
#include <omp.h>

#define SSE_WIDTH		4

#define NUMTRIALS   50

#ifndef NUMT
#define NUMT      4
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE    2000
#endif

//g++ main.c -o main -lm -fopenmp

void
Mult(float *a, float *b, float *c, int len)
{

    for(int i = 0; i < len; i++){
        c[i] = a[i] * b[i];
    }
}

/*void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;
	register float *pc = c;
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		_mm_storeu_ps( pc,  _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
		pc += SSE_WIDTH;
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}

// Didn't use this function.
float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	register float *pa = a;
	register float *pb = b;

	__m128 ss = _mm_loadu_ps( &sum[0] );
	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		ss = _mm_add_ps( ss, _mm_mul_ps( _mm_loadu_ps( pa ), _mm_loadu_ps( pb ) ) );
		pa += SSE_WIDTH;
		pb += SSE_WIDTH;
	}
	_mm_storeu_ps( &sum[0], ss );

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}*/

// Using the below assembly code can witness better speedup.
void
SimdMul( float *a, float *b,   float *c,   int len )
{
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;
	__asm
	(
		".att_syntax\n\t"
		"movq    -24(%rbp), %r8\n\t"		// a
		"movq    -32(%rbp), %rcx\n\t"		// b
		"movq    -40(%rbp), %rdx\n\t"		// c
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"movups	%xmm0, (%rdx)\n\t"	// store the result
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
			"addq $16, %rdx\n\t"
		);
	}

	for( int i = limit; i < len; i++ )
	{
		c[i] = a[i] * b[i];
	}
}



float
SimdMulSum( float *a, float *b, int len )
{
	float sum[4] = { 0., 0., 0., 0. };
	int limit = ( len/SSE_WIDTH ) * SSE_WIDTH;

	__asm
	(
		".att_syntax\n\t"
		"movq    -40(%rbp), %r8\n\t"		// a
		"movq    -48(%rbp), %rcx\n\t"		// b
		"leaq    -32(%rbp), %rdx\n\t"		// &sum[0]
		"movups	 (%rdx), %xmm2\n\t"		// 4 copies of 0. in xmm2
	);

	for( int i = 0; i < limit; i += SSE_WIDTH )
	{
		__asm
		(
			".att_syntax\n\t"
			"movups	(%r8), %xmm0\n\t"	// load the first sse register
			"movups	(%rcx), %xmm1\n\t"	// load the second sse register
			"mulps	%xmm1, %xmm0\n\t"	// do the multiply
			"addps	%xmm0, %xmm2\n\t"	// do the add
			"addq $16, %r8\n\t"
			"addq $16, %rcx\n\t"
		);
	}

	__asm
	(
		".att_syntax\n\t"
		"movups	 %xmm2, (%rdx)\n\t"	// copy the sums back to sum[ ]
	);

	for( int i = limit; i < len; i++ )
	{
		sum[0] += a[i] * b[i];
	}

	return sum[0] + sum[1] + sum[2] + sum[3];
}


int main()
{
    float *A = (float*)malloc(sizeof(float) * ARRAYSIZE);
    float *B = (float*)malloc(sizeof(float) * ARRAYSIZE);
    float *C = (float*)malloc(sizeof(float) * ARRAYSIZE);
    omp_set_num_threads( NUMT );
    
    //float nonMult_Perf = 0;
    //float SIMDMult_Perf = 0;
    float nonMult_Thd_Perf = 0;
    float SIMDMult_Thd_Perf = 0;
// If don't want to compare with different threads, use this one.
/*    for (int i = 0; i < NUMTRIALS; i++){
        float temp_nonMult_Perf = 0;
        
        time0 = omp_get_wtime( );
        Mult(A, B, C, (int) ARRAYSIZE);
        time1 = omp_get_wtime( );
        
        temp_nonMult_Perf = ARRAYSIZE/ (time1 - time0) / 1000000;
          if(temp_nonMult_Perf > nonMult_Perf){
              nonMult_Perf = temp_nonMult_Perf;
          }
       }  
       for (int i = 0; i < NUMTRIALS; i++){   
        float temp_SIMDMult_Perf = 0;
        
        time0 = omp_get_wtime( );
        SimdMul(A, B, C, (int) ARRAYSIZE);
        time1 = omp_get_wtime( );
        
        temp_SIMDMult_Perf = ARRAYSIZE/ (time1 - time0 ) / 1000000;
          if(temp_SIMDMult_Perf > SIMDMult_Perf){
              SIMDMult_Perf = temp_SIMDMult_Perf;
          }  
       }   
*/
 

//#pragma omp parallel for default(none) shared(A, B, C, time0, time1, nonMult_Thd_Perf)
     for(int i = 0; i < NUMTRIALS; i++){
         float temp_nonMult_Thd_Perf = 0;
         double timeS, timeE; // time Start & time End
         timeS = omp_get_wtime( );
         #pragma omp parallel
         {
           int first = omp_get_thread_num() * ARRAYSIZE/NUMT;
           Mult(&A[first], &B[first], &C[first], (int) ARRAYSIZE/NUMT);
         }
         timeE = omp_get_wtime( );
         temp_nonMult_Thd_Perf = ARRAYSIZE/ (timeE - timeS) / 1000000;
         if(temp_nonMult_Thd_Perf > nonMult_Thd_Perf){
            nonMult_Thd_Perf = temp_nonMult_Thd_Perf;
        }
     }
     
     
//#pragma omp parallel for default(none) shared(A, B, C, time0, time1, SIMDMult_Thd_Perf)
     for(int i = 0; i < NUMTRIALS; i++){
         float temp_SIMDMult_Thd_Perf = 0;
         double timeS, timeE; // time Start & time End
         timeS = omp_get_wtime( );
         #pragma omp parallel
         {
           int first = omp_get_thread_num() * ARRAYSIZE/NUMT;
           SimdMul(&A[first], &B[first], &C[first], (int) ARRAYSIZE/NUMT);
         }
         timeE = omp_get_wtime( );
         temp_SIMDMult_Thd_Perf = ARRAYSIZE/ (timeE - timeS) / 1000000;
         if(temp_SIMDMult_Thd_Perf > SIMDMult_Thd_Perf){
            SIMDMult_Thd_Perf = temp_SIMDMult_Thd_Perf;
        }
        
     }
     
    free(A);
    free(B);
    free(C);
    // If don't want to compare with different threads, use the below one.
    //printf("ArraySize = %5dK  MultNonSIMD = %7.2f  MultSIMD = %7.2f  NonSIMD_Thd = %7.2f  SIMD_Thd = %7.2f\n", ARRAYSIZE/1024, nonMult_Perf, SIMDMult_Perf, nonMult_Thd_Perf, SIMDMult_Thd_Perf);
    //printf("ArraySize = %5dK  NumThds = %d, NonSIMD_Thd = %7.2f  SIMD_Thd = %7.2f\n", ARRAYSIZE/1024, (int) NUMT, nonMult_Thd_Perf, SIMDMult_Thd_Perf);
    printf("%5d, %d, %7.2f, %7.2f\n", ARRAYSIZE/1024, (int) NUMT, nonMult_Thd_Perf, SIMDMult_Thd_Perf);
    return 0;

}