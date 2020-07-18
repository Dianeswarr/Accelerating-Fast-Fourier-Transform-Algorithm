

#include <stdio.h>
#include <stdlib.h>
/* The three files containing input and expected output data */
#include "inReal.h"
#include "outReal.h"
#include "outImag.h"


#include <math.h>

/* For keeping time. */
#include "xtime_l.h"
#define NUM_SAMPLES 16 /* for float, this would mean 1 MiB of samples */
/* Tolerances for expected vs calculated results. You might need to adjust these
   values depending on the data types you use. */
#define SMALL_TOLERANCE  0.01f /* for values less than 1 */
#define LARGER_TOLERANCE 0.1f  /* for values larger than */
typedef float real;
typedef struct{real  re; real  im;} Complex ;

#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

 Complex X[NUM_SAMPLES];
Complex F[NUM_SAMPLES];
Complex scrach[NUM_SAMPLES];

/*******************************************************************************
 * Utility functions for keeping time. Feel free to adapt to your needs.
 *****************************************************************************/
XTime tStart[4], tEnd[4];

/* Starts timer n and prints info. */
void tick(int n) {
	printf("[%d] Timer started...\n",n);
	XTime_GetTime(&tStart[n]);
}

/* Stops timer n and prints its value in microseconds. */
void tock(int n) {
	XTime_GetTime(&tEnd[n]);
	printf("[%d] Timer stopped...\n",n);
	printf("[%d] Elapsed time : %.2f us.\n", n, 1.0 * (tEnd[n] - tStart[n]) / (COUNTS_PER_SECOND/1000000));
}




void fft11( float o[32], float r[32])
{
    int k,m,l,b,s,p,t;
    int i =0;

    float  sine, realPart, imgPart;
	Complex even;
	Complex odd;
    Complex f[16];

    float re[16];
    float im[16];
    for (l = 0; l < 16; l++)
       	{
    	re[l] = o[l];
       	}

    for (s = 0; s < 16; s++)
           	{
        	im[s] = o[s+16];

           	}



    float cosin[65]= {1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,1.000000,0.707107,-0.000000,-0.707107,-1.000000,-0.707107,0.000000,
	0.707107,1.000000,-0.000000,-1.000000,0.000000,1.000000,-0.000000,-1.000000,0.000001,1.000000,-0.707107,0.000000,0.707107,-1.000000,0.707107,
	-0.000000,-0.707106,1.000000,-1.000000,1.000000,-1.000000,1.000000,-1.000000,1.000000,-1.000000,1.000000,-0.707107,0.000000,0.707107,-1.000000,
	0.707107,0.000000,-0.707107,1.000000,0.000000,-1.000000,-0.000000,1.000000,0.000000,-1.000000,-0.000002,1.000000,0.707107,-0.000000,-0.707107,
	-1.000000,-0.707107,0.000002,0.707108};

    float sine1[65] = {-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,-0.000000,-0.707107,-1.000000,-0.707107,0.000000,0.707107,1.000000,0.707107,-0.000000,-1.000000,
	0.000000,1.000000,-0.000000,-1.000000,0.000000,1.000000,-0.000000,-0.707107,1.000000,-0.707107,0.000000,0.707107,-1.000000,0.707107,-0.000000,0.000000,-0.000000,0.000000
    ,-0.000000,0.000001,-0.000000,0.000001,-0.000000,0.707107,-1.000000,0.707107,-0.000000,-0.707107,1.000000,-0.707107,-0.000000,1.000000,0.000000,-1.000000,-0.000000,1.000000,
    0.000000,-1.000000,-0.000000,0.707107,1.000000,0.707106,-0.000001,-0.707107,-1.000000,-0.707106 };


    float cos2[8] = {1.000000,0.923880,0.707107,0.382683,0.000000,-0.382683,-0.707107,-0.923880 };
    float sine2[8] ={-0.000000,-0.382683,-0.707107,-0.923880,-1.000000,-0.923880,-0.707107,-0.382683 };

    for (k = 0; k < NUM_SAMPLES/2; k++)
	{

		even.re = 0;
		even.im = 0;
		odd.re = 0;
		odd.im = 0;

		for ( m = 0; m < NUM_SAMPLES / 2; m++)
		{


			//Even

			realPart = (re[2 * m] * cosin[i]) - ((im[2 * m] * sine1[i]));
			even.re = even.re + realPart;
			imgPart = (re[2 * m] *sine1[i]) + ((im[2 * m] * cosin[i]));
			even.im = even.im + imgPart;

			//Odd
			realPart = (re[(2 * m) + 1] * cosin[i]) - ((im[(2 * m) + 1] * sine1[i]));
			odd.re =odd.re + realPart;
			imgPart = (re[(2 * m) + 1] * sine1[i]) + ((im[(2 * m) + 1]* cosin[i]));
			odd.im =odd.im + imgPart;
			i++;
		}

		f[k].re = even.re + ((cos2[k] * odd.re) - (sine2[k] * odd.im));
		f[k].im = even.im + ((cos2[k] * odd.im) + (sine2[k] * odd.re));
		f[k+NUM_SAMPLES/2].re = even.re - ((cos2[k] * odd.re) - (sine2[k] * odd.im));
		f[k+NUM_SAMPLES/2].im = even.im - ((cos2[k] * odd.im) + (sine2[k] * odd.re));





	}

    for (b = 0; b < NUM_SAMPLES; b++)
           	{
        	   r[b] = f[b].re;  // real val


           	}
    for (t=0; t < NUM_SAMPLES;t++)
               	{
            	   r[t+16] = f[t].im; // imaginary val

               	}




return ;
}


/****6*************************************************************************/

int main()
{
   float X[32];
   float y[32];
  
  for(int i = 0; i < NUM_SAMPLES; ++i){
    X[i] = inReal[i];
  }

  for(int ii = 0; ii < NUM_SAMPLES; ++ii){
      X[ii+16] = 0;
    }


  printf("starting fft...\n");
  tick(0);
  fft11(X, y);
  tock(0);
  for(int u = 0; u < NUM_SAMPLES; u++){
        F[u].re=y[u];

      }
    for(int uu = 0; uu < NUM_SAMPLES; uu++){
          F[uu].im=y[uu+16];

        }

  /* Below part compares the calculated result with the expected ones, and
     reports any mismatches. */
  int numMismatches = 0;
  float tolReal, tolImag;
  for(int i = 0; i < NUM_SAMPLES; ++i){
    tolReal = outReal[i] < 1.0 ? SMALL_TOLERANCE : LARGER_TOLERANCE;
    tolImag = outImag[i] < 1.0 ? SMALL_TOLERANCE : LARGER_TOLERANCE;

    if(fabsf(outReal[i] - F[i].re) > tolReal || fabsf(outImag[i] - F[i].im) > tolImag){
      //printf("Results do not match at index %d!\n", i);
      //printf("Expected: (%f,%fi), calculated: (%f,%fi)\n", outReal[i], outImag[i], f[i].a, f[i].b);
      numMismatches++;
    }
     	printf("Expected: (%f,%fi), calculated: (%f,%fi)\n", outReal[i], outImag[i], F[i].re, F[i].im);
  }

  if (numMismatches > 0)
    printf("Number of differences: %d out of %d", numMismatches, NUM_SAMPLES);
  else printf("Success! Results match with expected output!\n");


  return 0;
}
