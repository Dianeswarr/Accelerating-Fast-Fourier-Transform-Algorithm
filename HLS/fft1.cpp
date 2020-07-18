#include <assert.h>
#include <ap_axi_sdata.h>
#include <hls_stream.h>
typedef struct{float  re; float  im; } Complex ;
#define NUM_SAMPLES 16
//------------------------------------------------------------------------------------
// functions to insert and extract elements from an axi stream
// including conversion to correct data type
template <typename T, int U, int TI, int TD>
inline T pop_stream(ap_axiu <sizeof(T)*8,U,TI,TD> const &e)
{
assert(sizeof(T) == sizeof(int));
union
{
int ival;
T oval;
} converter;
converter.ival = e.data;
T ret = converter.oval;
volatile ap_uint<sizeof(T)> strb = e.strb;
volatile ap_uint<sizeof(T)> keep = e.keep;
volatile ap_uint<U> user = e.user;
volatile ap_uint<1> last = e.last;
volatile ap_uint<TI> id = e.id;
volatile ap_uint<TD> dest = e.dest;
return ret;
}
template <typename T, int U, int TI, int TD>
inline ap_axiu <sizeof(T)*8,U,TI,TD> push_stream(T const &v, bool last =
false)
{
ap_axiu<sizeof(T)*8,U,TI,TD> e;
assert(sizeof(T) == sizeof(int));
union
{
int oval;
T ival;
} converter;
converter.ival = v;
e.data = converter.oval;
// set it to sizeof(T) ones
e.strb = -1;
e.keep = 15; //e.strb;
e.user = 0;
e.last = last ? 1 : 0;
e.id = 0;
e.dest = 0;
return e;
}

//------------------------------------------------------------------------------------

template <typename T>
void fft11( T o[32], T r[32])
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
    #pragma HLS pipeline II=1
    	re[l] = o[l];
       	}

    for (s = 0; s < 16; s++)
           	{
  #pragma HLS pipeline II=1
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
  #pragma HLS pipeline II=1
		even.re = 0;
		even.im = 0;
		odd.re = 0;
		odd.im = 0;

		for ( m = 0; m < NUM_SAMPLES / 2; m++)
		{
                    #pragma HLS pipeline II=1

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
                     #pragma HLS pipeline II=1
        	   r[b] = f[b].re;  // real val


           	}
    for (t=0; t < NUM_SAMPLES;t++)
               	{
                     #pragma HLS pipeline II=1
            	   r[t+16] = f[t].im; // imaginary val

               	}




return ;
}




//------------------------------------------------------------------------------------
typedef ap_axiu<32,4,5,5> AXI_VAL;
template <typename T, int U, int TI, int TD>
void dut_mmult_accel_core ( AXI_VAL in_stream[32],  AXI_VAL
out_stream[32])
{
T A[32], C[32];
assert(sizeof(T)*8 == 32);
// stream in the 2 input matrices
for(int i=0; i<32; i++)
{
  
 A[i] = pop_stream<T,U,TI,TD>(in_stream[i]);
 }

 // do multiplication
fft11<T>(A,C);
 for(int j=0; j<32; j++) {

 out_stream[j] = push_stream<T,U,TI,TD>(C[j], j==31);
}
return;
}
// THIS IS THE TOP LEVEL DESIGN THAT WILL BE SYNTHESIZED
void fft (AXI_VAL in_stream[32], AXI_VAL out_stream[32], int junk)
{
#pragma HLS INTERFACE axis port =in_stream
#pragma HLS INTERFACE axis port =out_stream
#pragma HLS INTERFACE s_axilite port = junk bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port =return bundle=CRTL_BUS
	if (junk ==5)
	{
dut_mmult_accel_core <float, 4, 5, 5>(in_stream, out_stream);
	}
return;
}
