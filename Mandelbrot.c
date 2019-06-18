/*
 * Author: Christopher O'Rourke
 * Date: 4/13/2018
 */
 
//Headers
#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"
#include <altera_up_avalon_video_pixel_buffer_dma.h> //to swap front and back buffer
#include <math.h>
#include <stdlib.h> //for file I/O
#include <altera_avalon_mailbox_regs.h>
#include <altera_avalon_mailbox.h>
#include <altera_avalon_performance_counter.h>

alt_u64 time, time1;
alt_mailbox_dev** mailbox;
alt_mailbox_dev *mailboxes;
alt_up_pixel_buffer_dma_dev *my_pixel_buffer;
int cpuid = NIOS2_CPU_ID_VALUE;

//Variable Declaration
int x;
int y;
int iteration;
int value;
double imag;
double real;
int color;
double xtemp;
int MAX = 1000; //max number of iterations
float startReal;
float startImag;

//Size of the Window Frame
int width = 240;
int height = 320;

//Zoom and Translations of Window Frame
double zoom = 0.005;
float min_x;
float max_x;
float min_y;
float max_y;

//Keep track of Zoom Origin
float target_x;
float target_y;
int found = 1; //Acts as a boolean

//Structures
typedef struct complex{
    float real;
    float imag;
}complexNumber;

typedef struct cartesian{
    float x_point;
    float y_point;
}cartesianNumber;

//Declare structures
complexNumber compNum;
cartesianNumber P1, P2, Point;

//Declare Functions
int getColor(int interation);
int mandelbrot(float startReal, float startImag, int MAX);
complexNumber map(cartesianNumber, cartesianNumber, cartesianNumber);
float min_X(cartesianNumber, cartesianNumber);
float max_X(cartesianNumber, cartesianNumber);
float min_Y(cartesianNumber, cartesianNumber);
float max_Y(cartesianNumber, cartesianNumber);


int main()
{

    //Open the Pixel Buffer
    my_pixel_buffer = alt_up_pixel_buffer_dma_open_dev("/dev/video_pixel_buffer_dma_0");

    printf ("Program running (UART)...\n");
    fprintf (stderr, "Program running (LCD)...\n");

    /////////////For Lab05
    //Mailboxes
    alt_u32 message = 0;
    mailbox = (alt_mailbox_dev**)malloc(4);
    //alt_mailbox_dev *send_dev, *recv_dev;
    // Open the four mailboxes
    mailbox[0] = altera_avalon_mailbox_open("/dev/mailbox_0");
    mailbox[1] = altera_avalon_mailbox_open("/dev/mailbox_1");
    mailbox[2] = altera_avalon_mailbox_open("/dev/mailbox_2");
    mailbox[3] = altera_avalon_mailbox_open("/dev/mailbox_3");
    ////////////////////////////////////////////////////////////

    //Set the initial window frame to encompass:
    //-2.5 <= x <= 1
    //-1 <= y <= 1
    P1.x_point = -2.5;
    P1.y_point = -1;
    P2.x_point = 1;
    P2.y_point = 1;

    while (1)
    {
   	 alt_up_pixel_buffer_dma_clear_screen(my_pixel_buffer,0);
   	 for (x = 0; x < width; x++)
   	 {
   		 for (y = 0; y < height; y++)
   		 {
   			 //Point to convert
   			 Point.x_point = x;
   			 Point.y_point = y;

   			  // convert a Point to the appropriate complex number
   			 compNum = map(Point, P1, P2);

   			 //Check if the complex point is a part of the mandelbrot set
   			 //And return the amount of iterations
   			 value = mandelbrot(compNum.real, compNum.imag, MAX);
   			 printf("Value: %d" , value);

   			 //Find the first pixel greater than 450 to set as zoom origin
   			 if((value > 450) && found == 1){
   				 target_x = compNum.real;
   				 target_y = compNum.imag;
   				 found = 0;
   			 }

   			 //send the amount of iterations to return a specific color
   			 color = getColor(value);

   			 //Draw the pixel to the screen
   			 alt_up_pixel_buffer_dma_draw(my_pixel_buffer,color,imag,real);
   		 }
   	  }
   	 alt_up_pixel_buffer_dma_clear_screen(my_pixel_buffer,0);
    }//end while loop
  return 0;
}

float min_X(cartesianNumber P1, cartesianNumber P2){
    if(P1.x_point < P2.x_point){
   	 return P1.x_point;
    }else{
   	 return P2.x_point;
    }
}
float max_X(cartesianNumber P1, cartesianNumber P2){
    if(P1.x_point > P2.x_point){
   	 return P1.x_point;
    }else{
   	 return P2.x_point;
    }
}
float min_Y(cartesianNumber P1, cartesianNumber P2){
    if(P1.y_point < P2.y_point){
   	 return P1.y_point;
    }else{
   	 return P2.y_point;
    }
}
float max_Y(cartesianNumber P1, cartesianNumber P2){
    if(P1.y_point < P2.y_point){
   	 return P1.y_point;
    }else{
   	 return P2.y_point;
    }
}
complexNumber map(cartesianNumber P1, cartesianNumber P2, cartesianNumber Point){

    //col = Point.x_point  row = Point.y_point
    max_x = max_X(P1, P2);
    max_y = max_Y(P1, P2);
    max_x = min_X(P1, P2);
    max_y = min_Y(P1, P2);

    //Zoom
    min_x = target_x - 1 / (pow(1.5,zoom));
    max_x = target_x + 1 / (pow(1.5,zoom));
    min_y = target_x - .75 / (pow(1.5,zoom));
    max_y = target_x + .75 / (pow(1.5,zoom));

    compNum.real = Point.x_point/320 * (max_x - min_x) + min_x;
    compNum.imag = (239-Point.y_point) / 240 * (max_y - min_y) + min_y;

    return (compNum);
}

int mandelbrot(float startReal, float startImag, int MAX)
{
    iteration = 0;
    real = startReal;
    imag = startImag;
    xtemp = 0;

    //Pc(z) = z^2 + c
    while(((real*real + imag*imag) < 4) && iteration < MAX)
    {
   	 xtemp = real*real - imag*imag + startReal;
   	 imag = 2*real*imag + startImag;
   	 real = xtemp;
   	 iteration++;

   	 if(startReal == real && startImag == imag)
   	 {
   		 //indicates the point is in the mandelbrot set
   		 return -1;
   	 }
    }
    if(iteration >= MAX)
    {
   	 //Point lies within the mandelbrot set
   	 return -1;
    }
    else
    {
   	 //Return the iteration count
   	 return iteration;
    }
}

int getColor(int interation){
    //Color in mandelbrot set
    if(iteration == -1)
    {
   	 //Black
   	 color = 0x000000;
    }
    if(iteration == 0)
    {
   	 //Black
   	 color = 0x000000;
    }
    else
    {
   	 if(iteration <= 200)
   	 {
   		 //Red
   		 color = 0xFF0000;
   	 }
   	 if(iteration > 200 && iteration <=400)
   	 {
   		 //Orange
   		 color = 0xFFA500;
   	 }
   	 if(iteration >400 && iteration <= 600)
   	 {
   		 //Yellow
   		 color = 0xFFFF00;
   	 }
   	 if(iteration > 600 && iteration <=800)
   	 {
   		 //Green
   		 color = 0x008000;
   	 }
   	 if(iteration > 800){
   		 //Blue
   		 color = 0x0000FF;
   	 }
    }
    return color;
}

/*
    PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
    PERF_START_MEASURING(PERFORMANCE_COUNTER_0_BASE);
    for(x=NIOS2_CPU_ID_VALUE; x < 240; x+=2)

    time = perf_get_total_time ((void*)PERFORMANCE_COUNTER_0_BASE);
    time += time;
    altera_avalon_mailbox_post(mailbox[1-cpuid], 1);
    altera_avalon_mailbox_pend(mailbox[cpuid]);
    printf ("%d\n", time);
    alt_up_pixel_buffer_dma_clear_screen(my_pixel_buffer,0);
    count = count + 1;
    //PERF_RESET(PERFORMANCE_COUNTER_0_BASE);
 //End If-Else statement for Rotation
  */



