//=======================================================================================
//=======================================================================================
// PROGRAMA 		: Filtros Digitales (FIR)
// FUNCI�N 			: Pr�ctica Proyecto de Grado
// REALIZADO POR	: Holguer A. Becerra D. - Jefferson M. Garc�a P. - Juan D. Prieto A.
// ORGANIZACI�N		: Universidad pontificia Bolivariana
// FECHA 			: Agosto 2015
//=======================================================================================
//=======================================================================================

//=======================================================================================
// LIBRERIAS
//=======================================================================================
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include "./include/hwlib.h"
#include "./include/socal/socal.h"
#include "./include/socal/hps.h"
#include "./include/socal/alt_gpio.h"

#include "hps_0.h"
#include "filtroFIR.h"



//=======================================================================================
// PAR�METROS
//=======================================================================================
#define REG_BASE 0xC0000000
#define REG_SPAN 0x04000000

//=======================================================================================
// VARIABLES GLOBALES
//=======================================================================================
int i=0,j=0;
int fd;
void *virtual_base;
volatile unsigned long *h2p_start_timer_addr=NULL;
volatile unsigned long *p2h_div_time_addr=NULL;
volatile unsigned long *p2h_get_tick_addr=NULL;


volatile unsigned long *val1_addr=NULL;
volatile unsigned long *val2_addr=NULL;
volatile unsigned long *select_addr=NULL;
volatile unsigned long *wr_addr=NULL;
volatile unsigned long *result_addr=NULL;


//=======================================================================================
// RUTINA: INICIO DEL SISTEMA
//=======================================================================================
void init_system()
{
	if(( fd = open( "/dev/mem", ( O_RDWR | O_SYNC ))) == -1 )
	{
		printf( "ERROR: could not open \"/dev/mem\"...\n" );
		return;
	}

	virtual_base = mmap( NULL, REG_SPAN, ( PROT_READ | PROT_WRITE ), MAP_SHARED, fd, REG_BASE );
	if( virtual_base == MAP_FAILED )
	{
		printf( "ERROR: mmap() failed...\n" );
		close( fd );
		return;
	}

	h2p_start_timer_addr 	= virtual_base + (( unsigned long )( REG_BASE + TIMER_START_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	p2h_div_time_addr 		= virtual_base + (( unsigned long )( REG_BASE + TIMER_DIV_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	p2h_get_tick_addr 		= virtual_base + (( unsigned long )( REG_BASE + GETTICK_BASE ) & ( unsigned long)( REG_SPAN-1 ));


	///acc para el filtro HW
	val1_addr 		= virtual_base + (( unsigned long )( REG_BASE + VAL1_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	val2_addr 		= virtual_base + (( unsigned long )( REG_BASE + VAL2_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	select_addr 	= virtual_base + (( unsigned long )( REG_BASE + SELECTOR_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	wr_addr 		= virtual_base + (( unsigned long )( REG_BASE + WR_BASE ) & ( unsigned long)( REG_SPAN-1 ));
	result_addr 	= virtual_base + (( unsigned long )( REG_BASE + RESULT_SPAN ) & ( unsigned long)( REG_SPAN-1 ));

	alt_write_word( p2h_div_time_addr, 240 );
	alt_write_word( h2p_start_timer_addr, 0 );

}
//=======================================================================================
// EJECUCI�N FILTRO FIR
//=======================================================================================
int main()
{
 struct timeval t1, t2;
    double elapsedTime;
	
	

	int time_cont=0;

//	init_system();



	FIR_filter filtro;
	SIGNAL signal1;
	SIGNAL signal1_output;
	SIGNAL signal2_output;


	filtro.F1=6500;
	filtro.F2=9500;
	filtro.order=100;
	filtro.FS=44100;
	filtro.window_type=2;
	filtro.filter_type=2;
	filtro.sw_hw=0;

	calc_FIR_coefs(&filtro);
	load_signal(&signal1,"song.bin");

	//<><><><><><><><><><><><><><><><><>//
	// 		 SIGNAL REPLICATION 		//
	//<><><><><><><><><><><><><><><><><>//
	signal1_output.lenght=signal1.lenght;
	signal1_output.signal=(int*) malloc (sizeof(int)*signal1.lenght);

	signal2_output.lenght=signal1.lenght;
	signal2_output.signal=(int*) malloc (sizeof(int)*signal2_output.lenght);


	//<><><><><><><><><><><><><><><><><>//
	// APPLY FILTER & NORMALIZE SIGNAL	//
	//<><><><><><><><><><><><><><><><><>//
	normalize_signal( &signal1, 128);

	
	  // start timer
    gettimeofday(&t1, NULL);

	
	apply_FIR_whole_signal(&filtro, &signal1, &signal1_output);
	
    // do something
    // ...

    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf ("%f ms.\n",elapsedTime);

	
	
	  // start timer
    gettimeofday(&t1, NULL);

	
	apply_FIR_whole_signal_pthreads(&filtro, &signal1, &signal2_output);

	// do something
    // ...

    // stop timer
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf ("%f ms.\n",elapsedTime);


	normalize_signal( &signal1_output, 128);
	normalize_signal( &signal2_output, 128);




	save_signal( &signal1_output, "nonpthreads.bin");
	save_signal( &signal2_output, "pthreads.bin");

	printf("Finalizando Ejecucion.\n");
	return 0;
}
