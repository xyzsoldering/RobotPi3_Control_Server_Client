/* Simple control Raspberry Pi3 B+ through SDL and pigpio libs */ 
/* For compile use: g++ -o clientpi clientpi.cpp -lSDL -lSDL_net */ 

#include <iostream>
#include <cstdlib>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_net.h>

const unsigned short PORT = 1234; // The port our server will listen for incoming connections
const unsigned short BUFFER_SIZE = 1024; // Size of our data buffer 
const unsigned short MAX_SOCKETS = 3; // Max number of sockets
const unsigned short MAX_CLIENTS = MAX_SOCKETS - 1; // Max number of clients in our socket set
const std::string SERVER_NOT_FULL = "OK";
const std::string SERVER_FULL     = "FULL";


int main(int argc, char *argv[])
{
    

//------- Create a listening TCP socket on port 1234

    IPaddress serverIP;
 
    SDLNet_ResolveHost(&serverIP, NULL, PORT); 
    
    TCPsocket serverSocket;
    TCPsocket clientSocket[MAX_CLIENTS];
    
    bool shutdownServer = false;
    bool socketIsFree[MAX_CLIENTS];
    int clientCount = 0;
    char buffer[BUFFER_SIZE];
    int receivedByteCount = 0;
    

   if(SDLNet_Init() == -1)   // Initialise SDL NET
     {
      std::cout << "Failed to initialise SDL_NET: " << SDLNet_GetError() << std::endl;
      exit(-1); // Quit
     }
   
     // Create the socket set
    SDLNet_SocketSet  socketSet = SDLNet_AllocSocketSet(MAX_SOCKETS);
    if(socketSet == NULL)
    {
     std::cout << "Faild to allocate the socket set: " << SDLNet_GetError() << "\n";
     exit(-1); // Quit
    }else{
          std::cout << "Allocated socket set with size: " << MAX_SOCKETS << ", of which " << MAX_CLIENTS << " are availbe for use by clients. " << std::endl;
         } 
   
   // Initialize all the client sockets
   for(int loop = 0; loop<MAX_CLIENTS; loop++)
      {
       clientSocket[loop] = NULL;
       socketIsFree[loop] = true;
       }


   int hostResolved = SDLNet_ResolveHost(&serverIP, NULL, PORT);
       if(hostResolved == -1)
         {
          std::cout << "Failed to resolve the Server Host: " << SDLNet_GetError() << std::endl;
         }else{
              Uint8 * dotQuad = (Uint8*)&serverIP.host;
        std::cout << "Succssefully resolved Server Host to IP: " << (unsigned short)dotQuad[0] << "." << (unsigned short)dotQuad[1] << "." << (unsigned short)dotQuad[3];
        std::cout << " port " << SDLNet_Read16(&serverIP.port) << std::endl << std::endl;
        }
     
       // Try to open the server socket
    serverSocket = SDLNet_TCP_Open(&serverIP);
 
    if (!serverSocket)
    {
        std::cout << "Failed to open the server socket: " << SDLNet_GetError() << "\n";
        exit(-1);
    }
    else
    {
       std::cout << "Sucessfully created server socket." << std::endl;
    }
  SDLNet_TCP_AddSocket(socketSet, serverSocket);
   std::cout << "Awaiting client..." << std::endl;
  
   // ----Main loop----
      
     do
  {
   int numActiveSockets = SDLNet_CheckSockets(socketSet, 1);
   if(numActiveSockets !=0)
     {
      std::cout << "There are currently " << numActiveSockets << "socket(s) with date to be processed." << std::endl;
     }
   

      int serverSocketActivity = SDLNet_SocketReady(serverSocket);
 
      if (serverSocketActivity != 0)
        {
            // If we have room for more clients...
            if (clientCount < MAX_CLIENTS)
            {
 
                // Find the first free socket in our array of client sockets
                int freeSpot = -99;
                for (int loop = 0; loop < MAX_CLIENTS; loop++)
                {
                    if (socketIsFree[loop] == true)
                    {
                        
                        socketIsFree[loop] = false; // Set the socket to be taken
                        freeSpot = loop;  
                        break;                      // Break out of the loop straight away
                    }
                }
 
                // ...accept the client connection and then...
                clientSocket[freeSpot] = SDLNet_TCP_Accept(serverSocket);
 
                
                SDLNet_TCP_AddSocket(socketSet, clientSocket[freeSpot]);
 
                // Increase our client count
                clientCount++;
 
                // Send a message to the client saying "OK" 
        

                strcpy( buffer, SERVER_NOT_FULL.c_str() );
                int msgLength = strlen(buffer) + 1;
                
                SDLNet_TCP_Send(clientSocket[freeSpot], (void *)buffer, msgLength);
 
                std::cout << "Client connected. There are now " << clientCount << " client(s) connected." << std::endl << std::endl;
            }
            else // If we don't have room for new clients...
            {
                std::cout << "*** Maximum client count reached - rejecting client connection ***" << std::endl;
 
                // Accept the client connection to clear it from the incoming connections list
                TCPsocket tempSock = SDLNet_TCP_Accept(serverSocket);
 
                                
                strcpy( buffer, SERVER_FULL.c_str() );
                int msgLength = strlen(buffer) + 1;
                
                SDLNet_TCP_Send(tempSock, (void *)buffer, msgLength);
 
                // Shutdown, disconnect, and close the socket to the client
                SDLNet_TCP_Close(tempSock);
            }
 
        } // End of if server socket is has activity check
   

    // Loop to check all possible client sockets for activity
        for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)
        {
            // If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
            int clientSocketActivity = SDLNet_SocketReady(clientSocket[clientNumber]);
 
            //cout << "Just checked client number " << clientNumber << " and received activity status is: " << clientSocketActivity << endl;
 
            // If there is any activity on the client socket...
            if (clientSocketActivity != 0)
            {
                // Check if the client socket has transmitted any data by reading from the socket and placing it in the buffer character array
                receivedByteCount = SDLNet_TCP_Recv(clientSocket[clientNumber], buffer, BUFFER_SIZE);
 
                // If there's activity, but we didn't read anything from the client socket, then the client has disconnected...
                if (receivedByteCount <= 0)
                {
                    //...so output a suitable message and then...
                    std::cout << "Client " << clientNumber << " disconnected." << std::endl << std::endl;
 
                    //... remove the socket from the socket set, then close and reset the socket ready for re-use and finally...
                    SDLNet_TCP_DelSocket(socketSet, clientSocket[clientNumber]);
                    SDLNet_TCP_Close(clientSocket[clientNumber]);
                    clientSocket[clientNumber] = NULL;
 
                    // ...free up their slot so it can be reused...
                    socketIsFree[clientNumber] = true;
 
                    // ...and decrement the count of connected clients.
                    clientCount--;
 
                    std::cout << "Server is now connected to: " << clientCount << " client(s)." << std::endl << std::endl;
                }
                else // If we read some data from the client socket...
                {
                    // Output the message the server received to the screen
                    std::cout << "Received: >>>> " << buffer << " from client number: " << clientNumber << std::endl;
 
                    // Send message to all other connected clients
                    int originatingClient = clientNumber;
 
                    for (int loop = 0; loop < MAX_CLIENTS; loop++)
                    {
                        // Send a message to the client saying "OK" to indicate the incoming connection has been accepted
                        //strcpy( buffer, SERVER_NOT_FULL.c_str() );
                        int msgLength = strlen(buffer) + 1;
 
                        // If the message length is more than 1 (i.e. client pressed enter without entering any other text), then
                        // send the message to all connected clients except the client who originated the message in the first place
                        if (msgLength > 1 && loop != originatingClient && socketIsFree[loop] == false)
                        {
                            std::cout << "Retransmitting message: " << buffer << " (" << msgLength << " bytes) to client number: " << loop << std::endl;
                            SDLNet_TCP_Send(clientSocket[loop], (void *)buffer, msgLength);
                        }
 
                    }
 
                    // If the client told us to shut down the server, then set the flag to get us out of the main loop and shut down
                    if ( strcmp(buffer, "shutdown") == 0 )
                    {
                        shutdownServer = true;
 
                        std::cout << "Disconnecting all clients and shutting down the server..." << std::endl << std::endl;
                    }
 
                }
 
            } // End of if client socket is active check
 
        } // End of server socket check sockets loop

}

    
   while(shutdownServer == false);

    SDLNet_FreeSocketSet(socketSet);
    SDLNet_TCP_Close(serverSocket); 
    SDLNet_Quit();
    SDL_Quit();

   return 0;

}








