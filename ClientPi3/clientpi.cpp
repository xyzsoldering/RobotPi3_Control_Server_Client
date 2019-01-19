/* Simple control Raspberry Pi3 B+ through SDL and pigpio libs */ 
/* For compile use: g++ -o clientpi clientpi.cpp -lSDL -lSDL_net -lpigpio -lrt -lpthread */ 


#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>
#include <cstring>
#include <string>
#include "gpioset.h" /* GPIO Settings */


const unsigned short PORT = 1234;
const unsigned short BUFFER_SIZE = 1024; 


int main(int argc,char** argv)
{
  
/* Initialization GPIO */

  double start; 

  if (gpioInitialise() < 0)
    {
      fprintf(stderr, "pigpio initialisation failed\n");
      return 1;
    }

  
  const char *host;

  IPaddress serverIP;
  TCPsocket clientSocket;
 
  char buffer[BUFFER_SIZE];

  /* Initialise SDL_Net */

  if(SDLNet_Init() < 0)
    {
     std::cout << "Failed to initialise SDL_Net: " << SDLNet_GetError() << "\n";
     exit(-1); /* Quit */
    }

    
   SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
   if(socketSet == NULL)
     {
      std::cout << "Failed to Allocate the socket set: " << SDLNet_GetError() << "\n";
     }else{
     std::cout << "Successfuly Allocated socket set." << std::endl;
     }
     
     int hostResolved = SDLNet_ResolveHost(&serverIP, "localhost", PORT); /* 192.168.0.108 */
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
      
    while(shutdownClient == false)
       {

        int socketActive = SDLNet_CheckSockets(socketSet, 0);
 
        /* cout << "Sockets with data on them at the moment: " << activeSockets << endl; */
 
        if (socketActive != 0)
        {
            /* Check if we got a response from the server */
            int messageFromServer = SDLNet_SocketReady(clientSocket);
 
            if (messageFromServer != 0)
            {
                /* cout << "Got a response from the server... " << endl; */
                int serverResponseByteCount = SDLNet_TCP_Recv(clientSocket, buffer, BUFFER_SIZE);
 
                std::cout << "Received: " << buffer << std::endl;

 		/* Scan received buffer from server */

		

            int hwhich, hat, hvalue, jwhich, axis, jvalue;
  
             sscanf(buffer, "%d %d %d %i %i %i", &hwhich, &hat, &hvalue, &jwhich, &axis, &jvalue);
            if(hwhich == 0)	/* JOYHATMOTION */
                {
                  if(hat == 0)
                    {
                      if(hvalue == 0)
                        {
                         stop();
                        } else if(hvalue == 1) 
                                 {
                                  fwd();
                                 }else if(hvalue == 4)
                                      {
                                       rvs();
                                      }else if (hvalue == 8)
						{
						 left();
						}else if(hvalue == 2)
							{
							 right();
							}
                    }

               } if(jwhich == 0)   /* JOYAXISMOTION */
                    {
                       if(axis == 3)    
                            {
                             if(jvalue >= 0)
                               {
                                 decsertwo();
                               }else if(jvalue <=0)
                                      {
                                        incsertwo();
                                      }
                    }
                         else if(axis == 0)
                            {
                             if(jvalue >= 0)
                               {
                                decserone();
                               }else if(jvalue <=0)
                                      {
                                       incserone();
                                      } 
                                 } 
                         } 
         }

     }  
}


	SDLNet_TCP_Close(clientSocket);
	SDLNet_Quit();
	
}
