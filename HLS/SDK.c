#include"xaxidma.h"
#include"xfft.h"
#include<stdio.h>
#include<xparameters.h>
#include "inReal.h"
#include "outReal.h"
#include "outImag.h"
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "xtime_l.h"
#define NUM_SAMPLES 16
 /* for float, this would mean 1 MiB of samples */
/* Tolerances for expected vs calculated results. You might need to adjust these
   values depending on the data types you use. */
#define SMALL_TOLERANCE  0.01f /* for values less than 1 */
#define LARGER_TOLERANCE 0.1f  /* for values larger than */
typedef float real;
typedef struct{real  re; real  im;} Complex ;
typedef struct{real  values; bool  tlast;} Complex11 ;
#ifndef PI
# define PI	3.14159265358979323846264338327950288
#endif

              // Timer Function ( to calculation the execution time)
//-----------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------
 Complex X[NUM_SAMPLES];
Complex F[NUM_SAMPLES];
Complex scrach[NUM_SAMPLES];
int inStreamData[NUM_SAMPLES];

XFft my_fft;
XFft_Config  *my_fft_cfg;

XAxiDma AxiDma;
XAxiDma_Config *axidma_cfg;

    //Sending data from PS to PL side and receive data from PL to PS
//-----------------------------------------------------------------------------------------------------

int Run_HW_Accelerator(float A[32], float
res_hw[32])
{
//transfer A to the Vivado HLS block
	//tick(0);
	int status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) A, 32 * sizeof(float),
	XAXIDMA_DMA_TO_DEVICE);
	if (status != XST_SUCCESS) {
	printf("Error: DMA transfer to Vivado HLS block failed\n");
	return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;


	//get results from the Vivado HLS block
	status = XAxiDma_SimpleTransfer(&AxiDma, (unsigned int) res_hw,
			64 * sizeof(float),
	XAXIDMA_DEVICE_TO_DMA);
	if (status != XST_SUCCESS) {
	printf("Error: DMA transfer from Vivado HLS block failed\n");
	return XST_FAILURE;
	}
	/* Wait for transfer to be done */
	while (XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE)) ;
	//tock(0);
	//poll the DMA engine to verify transfers are complete
	/* Waiting for data processing */
	/* While this wait operation, the following action would be done
	 * First: Second matrix will be sent.
	 * After: Multiplication will be compute.
	 * Then: Output matrix will be sent from the accelerator to DDR and
	 * it will be stored at the base address that you set in the first
	SimpleTransfer
	 */
	//while ((XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)) ||
	 //(XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE))) ;

	return 0;
	}
//--------------------------------------------------------------------------------------------------------
              //Intitilizing IP core (FFT) and DMA Module
//--------------------------------------------------------------------------------------------------------

void initPeripherals() {
	printf("initializing FFT MODULE\n");
	my_fft_cfg = XFft_LookupConfig(XPAR_FFT_1_DEVICE_ID);
	if (my_fft_cfg) {
		int status = XFft_CfgInitialize(&my_fft, my_fft_cfg);
		if (status != XST_SUCCESS)
			printf("Error initializing    FFT MODULE\n");
	}

	printf("initializing AxiDMA\n");
	axidma_cfg = XAxiDma_LookupConfig(XPAR_AXIDMA_0_DEVICE_ID);
	if (axidma_cfg) {
		int status = XAxiDma_CfgInitialize(&AxiDma, axidma_cfg);
		if (status != XST_SUCCESS)
			printf("Error initializing AxiDMA core\n");
	}

	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DEVICE_TO_DMA);
	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK, XAXIDMA_DMA_TO_DEVICE);
}
//---------------------------------------------------------------------------------------------------
int main() {


float total[32];
float total1[32];
float total111[32];
float total2[32];


			initPeripherals();   //Intitilizing IP core (FFT) and DMA Module
			 for(int j = 0; j < NUM_SAMPLES; ++j){
				 total[j] = inReal[j];

			  }

			  for(int g = 0; g < NUM_SAMPLES; ++g){
				  total[g+16] = 0;

			   }

			  for(int gt = 0; gt < 32; ++gt){
				  total1[gt]     =  total[gt];
				  //printf(" calculated: %d\n", total1[gt] );
			  			   }

while (true)
{
int junk;
printf("Choose junk: ");
scanf("%d",&junk);
XFft_Set_junk(&my_fft,junk);
XFft_Start(&my_fft);


			  			Xil_DCacheFlushRange( (u32) total1, 32 * sizeof(float));   // clearing all cache memory
			  			Xil_DCacheFlushRange((u32) total111, 32 * sizeof(float));
			  			tick(0);
			  			Run_HW_Accelerator(total1, total111); //Sending data from PS to PL side and receive data from PL to PS
			  			tock(0);
			  			Xil_DCacheInvalidateRange( (u32) total111, 32 * sizeof(float));

                        while (!XFft_IsDone(&my_fft)); // wait till the last value is received
                        printf("Computation Completed\n");
			  			for(int gtt = 0; gtt < 32; ++gtt){
			  				total2[gtt]   = total111[gtt];
			  						  				 // printf(" calculated: %f\n", total2[gtt] );
			  						  			  			   }
			  						  //for(int gttt = 0; gttt < 32; ++gttt){
			  							 // total2[gttt]= valtemp[gttt].f;
			  						 //	printf(" calculated: %f\n", total2[gttt] );
			  						 			  			  			   //}



			  			for(int u = 0; u < NUM_SAMPLES; u++){
			  			      F[u].re=total2[u];

			  			    }
			  			  for(int uu = 0; uu < NUM_SAMPLES; uu++){
			  			        F[uu].im=total2[uu+16];

			  			      }
			  			int numMismatches = 0;
			  			  float tolReal, tolImag;
			  			  for( int i = 0; i < NUM_SAMPLES; ++i){
			  			    tolReal = outReal[i] < 1.0 ? SMALL_TOLERANCE : LARGER_TOLERANCE;
			  			    tolImag = outImag[i] < 1.0 ? SMALL_TOLERANCE : LARGER_TOLERANCE;

			  			    if(fabsf(outReal[i] - F[i].re) > tolReal || fabsf(outImag[i] - F[i].im) > tolImag){
			  			      //printf("Results do not match at index %d!\n", i);
			  			      //printf("Expected: (%f,%fi), calculated: (%f,%fi)\n", outReal[i], outImag[i], f[i].a, f[i].b);
			  			      numMismatches++;
			  			    }
			 printf ("Expected: (%f,%fi), calculated: (%f,%fi)\n", outReal[i], outImag[i], F[i].re, F[i].im);
			  			  }

			  			  if (numMismatches > 0)
			  			    printf("Number of differences: %d out of %d", numMismatches, NUM_SAMPLES);
			  			  else printf("Success! Results match with expected output!\n");

}
			  			  return 0;


	}
