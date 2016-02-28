/*
 * filtroFIR.c
 *
 *  Created on: 26/08/2015
 *      Author: jdani
 */

#include "FFT_includes.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include "hwlib.h"
#include "socal.h"
#include "hps.h"
#include "alt_gpio.h"
#include "fftc.h"

float fc[] = {0,132.83,290.87,478.9,702.61,968.77,1285.4,1662.2,2110.5,2643.8,3278.3,4033.2,4931.4,6000}; //14 Valores

static float A[2*N];
static float Z[2*N];

// This is a integer SIN lookup table
// Values are (SIN(x) * 1024)
short int_sin_array[360]=
{0, 17, 35, 53, 71, 89, 107, 124, 142, 160,
177, 195, 212, 230, 247, 265, 282, 299, 316, 333,
350, 366, 383, 400, 416, 432, 448, 464, 480, 496,
512, 527, 542, 557, 572, 587, 601, 616, 630, 644,
658, 671, 685, 698, 711, 724, 736, 748, 760, 772,
784, 795, 806, 817, 828, 838, 848, 858, 868, 877,
886, 895, 904, 912, 920, 928, 935, 942, 949, 955,
962, 968, 973, 979, 984, 989, 993, 997, 1001, 1005,
1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023,
1024, 1023, 1023, 1022, 1021, 1020, 1018, 1016, 1014, 1011,
1008, 1005, 1001, 997, 993, 989, 984, 979, 973, 968,
962, 955, 949, 942, 935, 928, 920, 912, 904, 895,
886, 877, 868, 858, 848, 838, 828, 817, 806, 795,
784, 772, 760, 748, 736, 724, 711, 698, 685, 671,
658, 644, 630, 616, 601, 587, 572, 557, 542, 527,
512, 496, 480, 464, 448, 432, 416, 400, 383, 366,
350, 333, 316, 299, 282, 265, 247, 230, 212, 195,
177, 160, 142, 124, 107, 89, 71, 53, 35, 17,
0, -18, -36, -54, -72, -90, -108, -125, -143, -161,
-178, -196, -213, -231, -248, -266, -283, -300, -317, -334,
-351, -367, -384, -401, -417, -433, -449, -465, -481, -497,
-512, -528, -543, -558, -573, -588, -602, -617, -631, -645,
-659, -672, -686, -699, -712, -725, -737, -749, -761, -773,
-785, -796, -807, -818, -829, -839, -849, -859, -869, -878,
-887, -896, -905, -913, -921, -929, -936, -943, -950, -956,
-963, -969, -974, -980, -985, -990, -994, -998, -1002, -1006,
-1009, -1012, -1015, -1017, -1019, -1021, -1022, -1023, -1024, -1024,
-1024, -1024, -1024, -1023, -1022, -1021, -1019, -1017, -1015, -1012,
-1009, -1006, -1002, -998, -994, -990, -985, -980, -974, -969,
-963, -956, -950, -943, -936, -929, -921, -913, -905, -896,
-887, -878, -869, -859, -849, -839, -829, -818, -807, -796,
-785, -773, -761, -749, -737, -725, -712, -699, -686, -672,
-659, -645, -631, -617, -602, -588, -573, -558, -543, -528,
-512, -497, -481, -465, -449, -433, -417, -401, -384, -367,
-351, -334, -317, -300, -283, -266, -248, -231, -213, -196,
-178, -161, -143, -125, -108, -90, -72, -54, -36, -18};

// This is a integer COS lookup table
// Values are (COS(x) * 1024)
short int_cos_array[360]=
{1024, 1023, 1023, 1022, 1021, 1020, 1018, 1016, 1014, 1011,
1008, 1005, 1001, 997, 993, 989, 984, 979, 973, 968,
962, 955, 949, 942, 935, 928, 920, 912, 904, 895,
886, 877, 868, 858, 848, 838, 828, 817, 806, 795,
784, 772, 760, 748, 736, 724, 711, 698, 685, 671,
658, 644, 630, 616, 601, 587, 572, 557, 542, 527,
512, 496, 480, 464, 448, 432, 416, 400, 383, 366,
350, 333, 316, 299, 282, 265, 247, 230, 212, 195,
177, 160, 142, 124, 107, 89, 71, 53, 35, 17,
0, -18, -36, -54, -72, -90, -108, -125, -143, -161,
-178, -196, -213, -231, -248, -266, -283, -300, -317, -334,
-351, -367, -384, -401, -417, -433, -449, -465, -481, -497,
-512, -528, -543, -558, -573, -588, -602, -617, -631, -645,
-659, -672, -686, -699, -712, -725, -737, -749, -761, -773,
-785, -796, -807, -818, -829, -839, -849, -859, -869, -878,
-887, -896, -905, -913, -921, -929, -936, -943, -950, -956,
-963, -969, -974, -980, -985, -990, -994, -998, -1002, -1006,
-1009, -1012, -1015, -1017, -1019, -1021, -1022, -1023, -1024, -1024,
-1024, -1024, -1024, -1023, -1022, -1021, -1019, -1017, -1015, -1012,
-1009, -1006, -1002, -998, -994, -990, -985, -980, -974, -969,
-963, -956, -950, -943, -936, -929, -921, -913, -905, -896,
-887, -878, -869, -859, -849, -839, -829, -818, -807, -796,
-785, -773, -761, -749, -737, -725, -712, -699, -686, -672,
-659, -645, -631, -617, -602, -588, -573, -558, -543, -528,
-512, -497, -481, -465, -449, -433, -417, -401, -384, -367,
-351, -334, -317, -300, -283, -266, -248, -231, -213, -196,
-178, -161, -143, -125, -108, -90, -72, -54, -36, -18,
-1, 17, 35, 53, 71, 89, 107, 124, 142, 160,
177, 195, 212, 230, 247, 265, 282, 299, 316, 333,
350, 366, 383, 400, 416, 432, 448, 464, 480, 496,
512, 527, 542, 557, 572, 587, 601, 616, 630, 644,
658, 671, 685, 698, 711, 724, 736, 748, 760, 772,
784, 795, 806, 817, 828, 838, 848, 858, 868, 877,
886, 895, 904, 912, 920, 928, 935, 942, 949, 955,
962, 968, 973, 979, 984, 989, 993, 997, 1001, 1005,
1008, 1011, 1014, 1016, 1018, 1020, 1021, 1022, 1023, 1023};

// Integer sin lookup
double int_sin( int deg )
{
	deg %= 360;
	if(deg < 0)
	{
	  deg += 360;
	}
	return((double)int_sin_array[deg]/1024);
}

// Integer cos lookup
double int_cos( int deg )
{
	deg %= 360;
	if(deg < 0)
	{
		deg += 360;
	}
	return((double)int_cos_array[deg]/1024);
}

void normalize_signal( SIGNAL * FFT_signal, int center)
{
	int prom=0;
	int i;
	for(i=0;i<FFT_signal->lenght; i++)
	{
		prom=prom+FFT_signal->signal[i];
	}
	prom=prom/FFT_signal->lenght;
	prom=center-prom;

	for(i=0;i<FFT_signal->lenght; i++)
	{
		FFT_signal->signal[i]=FFT_signal->signal[i]+prom;
	}
}

void save_type(char * inpfile, char * outpfile, int * enable)
{
	size_t length;
	length = strlen(inpfile);
	int n=3;
	int num_code=length-n;
	int i;
	for(i=0;i<=n;i++)
	{
		outpfile[i]=inpfile[i+num_code];
	}
	if(outpfile[0]=='w' && outpfile[1]=='a' && outpfile[2]=='v')
	{
		enable=1;
	}
	else
	{
		enable=2;
	}
}

void save_signal( SIGNAL * FFT_signal, char * filename)
{
	FILE * pFilt;
	int i;
	pFilt = fopen(filename,"wb");
	//save
	unsigned char dato;
	for(i=0;i<FFT_signal->lenght;i++)
	{
		dato=(unsigned char)FFT_signal->signal[i];
		fwrite ( &dato, sizeof(char), 1, pFilt);
	}
	fclose(pFilt);
}

void melcepstrum_conversion(int in[], int n, float mel[], int m, float fs)
{
    int i, j;

    float deltaf = fs / n;
    for (i = 0; i < m; i++)
    {
        mel[i] = 0.0;
        for (j = 0; j < n; j++)
        {
            mel[i] += in[j]*melH(j*deltaf, i+1);
        }
        mel[i] = log10(mel[i]);
    }
}

// compute the value of the mel triangle filter bank i at frequency f
double melH(int f, int i)
{
    if (f < fc[i-1] || f >= fc[i+1]) return 0;
    if (f < fc[i]) return (f-fc[i-1])/(fc[i]-fc[i-1]);
    else           return (f-fc[i+1])/(fc[i]-fc[i+1]);
}

// O(n2) dct - simple but slow (use for small vectors only)
void dct(float in[], float out[], int n)
{
    int i,j;

    double pn = PI / n;
    for (i = 0; i < n; i++)
    {
        out[i] = 0.0;
        for (j = 0; j < n; j++)
        {
            out[i] += in[j]*sin(i*pn*(j+0.5));
        }
    }
}

