/* Example of simple joystick input with SDL and Raspberry Pi3 B+. */

#ifndef GPIOSET_H
#define GPIOSET_H

#include <pigpio.h>

 /* First Pulse to Servo */

int pulseserone = 1400;
int pulsesertwo = 1400;

 /* FWD. Set up GPIO */

 void fwd(void)
{ 
 gpioSetMode(27, PI_OUTPUT); // Set up GPIO 23
 gpioSetMode(22, PI_OUTPUT); // Set up GPIO 24
 gpioSetMode(17, PI_OUTPUT);
 gpioSetMode(18, PI_OUTPUT);

 gpioWrite(27, 1);
 gpioWrite(22, 1);
 gpioWrite(17, 0);
 gpioWrite(18, 0);
  
} 
   
 /* STOP. Set up GPIO */

 void stop(void)
{
   
  gpioSetMode(17, PI_OUTPUT);  
  gpioSetMode(18, PI_OUTPUT);  
  gpioSetMode(27, PI_OUTPUT);  
  gpioSetMode(22, PI_OUTPUT); 
 
  gpioWrite(17, 0);
  gpioWrite(18, 0);
  gpioWrite(27, 0);
  gpioWrite(22, 0);  
  
}

   /* RVS. Set up GPIO */

void rvs(void)
{
  gpioSetMode(17, PI_OUTPUT);
  gpioSetMode(18, PI_OUTPUT);
  gpioSetMode(22, PI_OUTPUT);
  gpioSetMode(27, PI_OUTPUT);

  gpioWrite(17, 1);
  gpioWrite(18, 1);
  gpioWrite(22, 0);
  gpioWrite(27, 0);

 }

void left(void)
{
  gpioSetMode(17, PI_OUTPUT);
  gpioSetMode(18, PI_OUTPUT);
  gpioSetMode(22, PI_OUTPUT);
  gpioSetMode(27, PI_OUTPUT);

  gpioWrite(27, 1);
  gpioWrite(17, 0);
  gpioWrite(18, 1);
  gpioWrite(22, 0);

}

void right(void)
{ 
  gpioSetMode(17, PI_OUTPUT);
  gpioSetMode(18, PI_OUTPUT);
  gpioSetMode(22, PI_OUTPUT);
  gpioSetMode(27, PI_OUTPUT);

  gpioWrite(22, 1);
  gpioWrite(18, 0);
  gpioWrite(27, 0);
  gpioWrite(17, 1);

}

 /* Increase Servo One */

 void incserone(void)
{
   pulseserone = pulseserone+15; 
    
   if (pulseserone>=2400) /* Stop Pulse */
     { 
       pulseserone=2400;  
       fprintf(stderr, "STOP USE SERVO ONE: CRITICAL PULSE! %i\n", pulseserone);
     }
  
   gpioSetMode(20, PI_OUTPUT);  /* Set Up GPIO */
   gpioServo(20, pulseserone);  /* Set Up PULSE */
      
   printf("S_1 INC_PULSE: %i\n", pulseserone);  /* Print in Terminal */ 
   
}

   /* Decrease Servo One */

void decserone(void)
{
  pulseserone = pulseserone-15;
  
  if (pulseserone<=600) /* Stop Pulse */
    {
     pulseserone=600;
     fprintf(stderr, "STOP USE SERVO ONE: CRITICAL PULSE! %i\n", pulseserone);  
    }

  gpioSetMode(20, PI_OUTPUT); /* Set Up GPIO */
  gpioServo(20, pulseserone); /* Set Up PULSE */
 
  printf("S_1 DEC_PULSE: %i\n", pulseserone); /* Print in Terminal */
 }

 /* Increase Servo Two */

void incsertwo(void)
{
   pulsesertwo = pulsesertwo+15; 
    
   if (pulsesertwo>=2400) /* Stop Pulse */
     { 
       pulsesertwo=2400;
       fprintf(stderr, "STOP USE SERVO TWO: CRITICAL PULSE! %i\n", pulsesertwo);
     }
  
   gpioSetMode(21, PI_OUTPUT);  /* Set Up GPIO */
   gpioServo(21, pulsesertwo);  /* Set Up PULSE */
      
   printf("S_2 INC_PULSE: %i\n", pulsesertwo); /* Print in Terminal */ 

}

  /* Decrease Servo Two */

void decsertwo(void)
{
  pulsesertwo = pulsesertwo-15;
  
  if (pulsesertwo<=600) /* Stop Pulse */
    {
     pulsesertwo=600;
     fprintf(stderr, "STOP USE SERVO TWO: CRITICAL PULSE! %i\n", pulsesertwo);  
    }

  gpioSetMode(21, PI_OUTPUT); /* Set Up GPIO */
  gpioServo(21, pulsesertwo); /* Set Up PULSE */
   
  printf("S_2 DEC_PULSE: %i\n", pulsesertwo); /* Print in Terminal */
    
} 

#endif
