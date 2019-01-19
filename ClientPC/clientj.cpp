/* Simple control Raspberry Pi3 B+ through SDL and pigpio libs */ 
/* For compile use: g++ -o clientj clientj.cpp -lSDL -lSDL_net */

#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <cstdlib>
#include <cstring>

			 /*  CLIENT   */

const unsigned short PORT = 1234;  /* The port we are connecting to */
const unsigned short BUFFER_SIZE = 1024; /* Size of our buffer */ 



int main(int argc, char** argv)
{
    
  const char *host;   /* Where we store the host name */
  char buffer[BUFFER_SIZE]; /* Array of character's we'll use to transmit our message */
  int inputLength = 0;  /* The lenght of our string in characters */

  IPaddress serverIP; /* The IP we will connect to */
  TCPsocket clientSocket; /* The socket to use */
  
    
    int num_js, i, quit_flag;
   
    SDL_Event event;
    SDL_Joystick *js;

    /* Initialize SDL's joystick and video subsystems. */

    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) != 0) 
       {
	fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
	return 1;
       }
    atexit(SDL_Quit);


    /* Create a 256x256 window so we can collect input events. */

    if (SDL_SetVideoMode(256, 256, 16, 0) == NULL) 
       {
	printf("Error: %s\n", SDL_GetError());
	return 1;
       }


    /* Find out how many joysticks are available. */

    num_js = SDL_NumJoysticks();
    printf("SDL recognizes %i joystick(s) on this system.\n", num_js);
    if (num_js == 0)
       {
	printf("No joysticks were detected.\n");
	return 1;
       }

    /* Print out information about each joystick. */

    for (i = 0; i < num_js; i++)
       {

	/* Open the joystick. */

        SDL_JoystickEventState(SDL_ENABLE); 
   	js = SDL_JoystickOpen(i);

	if (js == NULL) 
           {
	    printf("Unable to open joystick %i.\n", i);
	    } 
             else
            {
	    printf("Joystick %i\n", i);
	    printf("\tName:       %s\n", SDL_JoystickName(i));
	    printf("\tAxes:       %i\n", SDL_JoystickNumAxes(js));
	    printf("\tTrackballs: %i\n", SDL_JoystickNumBalls(js));
	    printf("\tButtons:    %i\n", SDL_JoystickNumButtons(js));

	    /* Close the joystick. */ 

	    SDL_JoystickClose(js);
	}
    }
 

    /* We'll use the joystick for the demonstration. */
    
   js = SDL_JoystickOpen(0);
   
    if (js == NULL) 
        {
	printf("Unable to open joystick: %s\n", SDL_GetError());
        }

	/* Initialise SDL_Net */

  if(SDLNet_Init() < 0)
    {
     std::cout << "Failed to initialise SDL_Net: " << SDLNet_GetError() << "\n";
     exit(-1); // Quit
    }

     
   SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
   if(socketSet == NULL)
     {
      std::cout << "Failed to Allocate the socket set: " << SDLNet_GetError() << "\n";
     }else{
     std::cout << "Successfuly Allocated socket set." << std::endl;
     }
     
     int hostResolved = SDLNet_ResolveHost(&serverIP, "192.168.0.107", PORT); /* Server IP Address and Port */
     if(hostResolved == -1)
       {
        std::cout << "Failed to Resolve the server hostname: " << SDLNet_GetError() << "\nContinuing...\n";
       }else{
            Uint8 * dotQuad = (Uint8*)&serverIP.host;
        std::cout << "Succssefully resolved Server Host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[3];
        std::cout << " port " << SDLNet_Read16(&serverIP.port) << std::endl << std::endl;
            }
    
     if((host = SDLNet_ResolveIP(&serverIP)) == NULL)
       {
        std::cout << "Failed to resolve the server IP address: " << SDLNet_GetError() << std::endl;
       }else{
           std::cout << "Successfully resolved IP to host: " << host << std::endl;
            }

     bool shutdownClient = true;

      clientSocket = SDLNet_TCP_Open(&serverIP);
      if(!clientSocket)
        {
         std::cout << "Failed to open socket to server: " << SDLNet_GetError() << "\n";
         exit(-1);
        }else{
              std::cout << "Connection okey, about to read connection status from the server.." << std::endl;
             
      
        SDLNet_TCP_AddSocket(socketSet, clientSocket);
        
        int activeSockets = SDLNet_CheckSockets(socketSet, 5000);
        std::cout << "There are " << activeSockets << "socket(s) with date on them at the moment." << std::endl;
        
        int gotServerResponse = SDLNet_SocketReady(clientSocket);
        if(gotServerResponse !=0)
          {
           std::cout << "Got a response from the server... " << std::endl;
           int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
           std::cout << "Got the following from server: " << buffer << "(" << serverResponseByteCount << " bytes)" << std::endl;
          if(strcmp(buffer, "OK") == 0)
          {
           shutdownClient = false;
           std::cout << "Joing server now..." << std::endl << std::endl;
          } else{
          std::cout << "Server is full... Terminating connection." << std::endl;
         }
         }else{
               std::cout << "No response from server..." << std::endl;
        }

        }




    /* Loop until the user presses Q. */

    quit_flag = 0;
    while (SDL_WaitEvent(&event) != 0 && quit_flag == 0) 
      {


     inputLength = strlen(buffer)+1;

     sprintf(buffer, "%d %d %d %i %i %i", event.jhat.which ,event.jhat.hat, event.jhat.value, event.jaxis.which, event.jaxis.axis, event.jaxis.value);  /* Put SDL_JOYAXISMOTION, SDL_JOYHATMOTION, SDL_JOYBUTTONDOWN to buffer */

     if(SDLNet_TCP_Send(clientSocket, (void *)buffer, inputLength) < inputLength) /* Sent our buffer to Server vio TCP */
      {
       std::cout << "Failed to send message: " << SDLNet_GetError() << std::endl;
       exit(-1);
      }else{
           std::cout << "Message sent successfully." << std::endl;  
           }
	


       switch (event.type)
         {

	   case SDL_KEYDOWN:

	        if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_ESCAPE) 
                 {
		  printf("Q or ESCAPE pressed. Exiting.\n");
		  quit_flag = 1;
	         }
	        break;


  case SDL_JOYAXISMOTION:

       printf("Joystick %i, axis %i movement to %i\n", event.jaxis.which, event.jaxis.axis, event.jaxis.value);

       if(event.jaxis.axis == 3) /* Event to axis 0 (left-right) */
         {           
          if( event.jaxis.value >=0) /* Move Servo Two to right */
           {
          /*  decsertwo(); */  
           }
 
            else if (event.jaxis.value <=0) /* Move Servo Two to left */
                  {
                  /* incsertwo(); */
                  }                                    
         }
 
          else if (event.jaxis.axis == 4) /* Event to axis 1 (up-down) */
                {      
                 if( event.jaxis.value >=0) /* Move Servo One to down */
                  {
                /*   decserone(); */
                  } 
                   else if (event.jaxis.value <=0) /* Move Servo Two to up */
                         {
                       /*   incserone(); */
                         }
                }                        
	         break;

  case SDL_JOYHATMOTION:
    
       printf("which: %d hat: %d value: %d\n",event.jhat.which ,event.jhat.hat, event.jhat.value);
              
          /* MOVE FWD */

         if ( event.jhat.value == SDL_HAT_UP )
          { 
           if(event.jhat.value >= 0)
            {
         /*   fwd(); */
            }
             printf("MOVE FWD\n");
          }
         
          /* RVS */  
         
         if ( event.jhat.value == SDL_HAT_DOWN )
          {
           if( event.jhat.value >= 0)
            {
         /*    rvs(); */
            }
             printf("RVS\n");
          }

          /* MOVE LEFT */
          
         if ( event.jhat.value == SDL_HAT_LEFT )
          { 
           if(event.jhat.value >= 0)
            {
          /*   left(); */
            }  
             printf("MOVE LEFT\n");
          }
        
          /* MOVE RIGHT */  
            
         if ( event.jhat.value == SDL_HAT_RIGHT )
          {
           if(event.jhat.value >= 0)
            {
           /* right(); */
            }
             printf("MOVE RIGHT\n");
          }

         if ( event.jhat.value == SDL_HAT_CENTERED )
          {
           if(event.jhat.value >= 0)
            {
            /* stop(); */
            }
             printf("CENTER BUTTON STOP !\n");
          }
           break;


	    /* The SDL_JOYBUTTONUP and SDL_JOYBUTTONDOWN events
	       are generated when the state of a joystick button
	       changes. */

  case SDL_JOYBUTTONUP:

  /* fall through to SDL_JOYBUTTONDOWN */

  case SDL_JOYBUTTONDOWN:
 
      printf("Joystick %i button %i: %i\n", event.jbutton.which, event.jbutton.button, event.jbutton.state);
              
      if (event.jbutton.button == 5) // Set button 5 to Stop for all motors
       {
        if (event.jbutton.button >= 0)
         {
       //   stop();
         } 
          printf("STOP\n");
       }
      	break;
       
      }
    }

    /* Close the joystick. */

    SDL_JoystickClose(js);

    SDLNet_TCP_Close(clientSocket);
    SDLNet_Quit();
    SDL_Quit();

    return 0;
}





