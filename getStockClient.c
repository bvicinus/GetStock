//
//  getStockClient.c
//  
//
//  Created by Brandon Vicinus, Gabriel Sotolongo on 11/25/15.
//
//================================== file = getStockClient.c
//=  A client program for getting stock quotes                                =
//=============================================================================
//= Code based off udpClient.c written by:
//=          Ken Christensen                                                  =
//=          University of South Florida                                      =
//=          WWW: http://www.csee.usf.edu/~christen                           =
//=          Email: christen@csee.usf.edu                                     =



#define  BSD                // WIN for Winsock and BSD for BSD sockets

//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#ifdef WIN
#include <windows.h>      // Needed for all Winsock stuff
#endif
#ifdef BSD
#include <sys/types.h>    // Needed for sockets stuff
#include <netinet/in.h>   // Needed for sockets stuff
#include <sys/socket.h>   // Needed for sockets stuff
#include <arpa/inet.h>    // Needed for sockets stuff
#include <fcntl.h>        // Needed for sockets stuff
#include <netdb.h>        // Needed for sockets stuff
#include <unistd.h>
#endif

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM           1050  // Port number used
#define  IP_ADDR      "127.0.0.1" // IP address of server1 (*** HARDWIRED ***)
#define MAX_LEN_REQUEST     2   //max length of a received string for requests
#define MAX_LEN_PACKET      4095 //max length of a packet, limited by buffer
#define MAX_LEN_USERNAME    32  //standard requirement
#define MAX_LEN_STOCKNAME   6   //5+1 GOOGL is largest stock available




//===================================HELPER FUNCTIONS=========================

//REQ
//asks user to enter a valid username they want to have
//adds username to packet
int registerUser(char *userNamePacket) {
   
   char userPacket[MAX_LEN_PACKET];
   char temp[MAX_LEN_PACKET];
   strcpy(userPacket, "REG,"); //first part of packet is 'REG,'
   
   printf("Enter what username you would like to register: \n");
   scanf("%s", temp);
   
   //check for size of username less than 32 chars
   if (strlen(temp) > 33) { //32+1 for string terminating char
      printf("Enter a username 32 characters or less: \n");
      return 0;
   }
   
   //add username on to the end of the packet
   strncat(userPacket, temp, MAX_LEN_USERNAME);
   
   strcpy(userNamePacket, userPacket);
   userNamePacket[strlen(userNamePacket)+1] = 0;
   //add semicolin
   userNamePacket[strlen(userNamePacket)] = ';';
   
   
   return 1; //successful run
   
}

//-----------------------------------------------------------------

//UNR
//requests username from user they wish to unregister
//add username to packet
int unRegisterUser(char *unRegisterPacket) {
   
   
   char unRegisterPck[MAX_LEN_PACKET];
   char temp[MAX_LEN_PACKET];
   strcpy(unRegisterPck, "UNR,"); //first part of packet is 'UNR,'
   
   printf("Enter what username you would like to unregister: \n");
   scanf("%s", temp);
   
   
   //check for size of username less than 32 chars
   if (strlen(temp) > 33) { //32+1 for string terminating char
      printf("Enter a username 32 characters or less: \n");
      return 0;
   }
   
   //add username to end of packet
   strncat(unRegisterPck, temp, MAX_LEN_USERNAME);
   
   strcpy(unRegisterPacket, unRegisterPck);
   unRegisterPacket[strlen(unRegisterPacket)+1] = 0;
   unRegisterPacket[strlen(unRegisterPacket)] = ';'; //add semicolon
   
   
   //proper execution, return 1
   return 1;
   
}

//-----------------------------------------------------------------

//QUO
//asks user to enter stocks they wish to view
//
int requestQuote(char *requestedQuotePacket) {
   
   
   char requestedQuotePck[MAX_LEN_PACKET];
   char temp[MAX_LEN_PACKET];
   int again = 0;
   int valid = 0;
   char comma[1] = ",";
   
   strcpy(requestedQuotePck, "QUO,"); //first part of the packet is QUO,
   
   //prompt for username
   char username[MAX_LEN_USERNAME];
   printf("Enter your username: \n");
   scanf("%s", username);
   //add username and comma
   strncat(requestedQuotePck, username, MAX_LEN_USERNAME);
   strncat(requestedQuotePck, comma, MAX_LEN_PACKET); //add a comma
   
   
   
   do {
      
      printf("Enter what stock quote you would like to view:\n");
      scanf("%s", temp);
      
      //check if requested stock is a valid length
      if (strlen(temp) <= MAX_LEN_STOCKNAME) {
         strncat(requestedQuotePck, temp, MAX_LEN_STOCKNAME);
         strncat(requestedQuotePck, comma, MAX_LEN_PACKET); //add a comma
         
      }
      else {
         printf("Enter a valid stock...\n");
         return 0;
      }
      
      //ask the user if they would like to enter another stock
      do{
         printf("Would you like to check another stock?\nEnter 1 for yes, 0 for no\n");
         scanf("%s", temp);
         
         //this if statement checks if the user entered only 1 stock to check
         if(again == 0 && atoi(temp) == 0) {
            //delete the extra comma
            requestedQuotePck[strlen(requestedQuotePck)-1] = ';';
            strcpy(requestedQuotePacket, requestedQuotePck);
            return 1;
         }
         
         
         again = atoi(temp);
         
         if (again == 0) { //valid answer: NO
            valid = 1;
            again = 0;
         }
         else if( again == 1) { //valid answer: YES
            valid = 1;
            again = 1;
         }
         else { //invalid answer
            printf("Invalid Entry\n");
            valid = 0;
         }
      }while(valid == 0);
      
   }while(again == 1);
   
   
   
   strcpy(requestedQuotePacket, requestedQuotePck);
   
   
   
   requestedQuotePacket[strlen(requestedQuotePacket) -2] = ';';
   
   //proper execution
   return 1;
   
}


//-----------------------------------------------------------------

//QQQ...
//a function made to make any/invalid requests possible (for testing purposes)
int invalidRequest(char *customPacket) {
   
   char randomPacket[MAX_LEN_PACKET];
   //since this is a test to send a custom packet, you can type anything
   //as long as its under the max packet length
   
   printf("Enter the command you would like to attempt: \n");
   scanf("%s", randomPacket);
   if (strlen(randomPacket) >= 2) {
      strcpy(customPacket, randomPacket);
   }
   else {
      printf("Invalid entry.\n");
      return 0;
   }
   
   //proper execution
   return 1;
}


//-----------------------------------------------------------------





//===== Main program ==========================================================
int main()
{
#ifdef WIN
   WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
   WSADATA wsaData;                              // Stuff for WSA functions
#endif
   int                  client_s;        // Client socket descriptor
   struct sockaddr_in   server_addr;     // Server Internet address
   int                  addr_len;        // Internet address length
   char                 out_buf[4096];   // Output buffer for data
   char                 in_buf[4096];    // Input buffer for data
   int                  retcode;         // Return code
   
#ifdef WIN
   // This stuff initializes winsock
   WSAStartup(wVersionRequested, &wsaData);
#endif
   
   //---------------------------------------------------------------------
   
    //Christensen code
   // Create a socket
   //   - AF_INET is Address Family Internet and SOCK_DGRAM is datagram
   client_s = socket(AF_INET, SOCK_DGRAM, 0);
   if (client_s < 0)
   {
      printf("*** ERROR - socket() failed \n");
      exit(-1);
   }
   
    //Christensen code
   // Fill-in server1 socket's address information
   server_addr.sin_family = AF_INET;                 // Address family to use
   server_addr.sin_port = htons(PORT_NUM);           // Port num to use
   server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
   
   //---------------------------------------------------------------------
   
   // Assign a message to buffer out_buf
   
   char received_request[MAX_LEN_REQUEST]; //max length is 2; 1 character, 1
                                        //more for end of string character
   char packet[MAX_LEN_PACKET]; //4095 max length limited by buffers
   
   int incorrectEntry       = 0;
   int checkRequestCall     = 0;
   
   do {
      printf("Enter what you would like to do:\n1. Register Username\n2. Unregister Username\n3. Request Stock Quote(s)\n");
      scanf ("%s", received_request);
      
      switch (atoi(received_request)) {
         
         case 1:
            //REG
            do{
               checkRequestCall = registerUser(&packet);
               //returns 0 if unsuccessful registration
            }while(checkRequestCall == 0);
            
            
            incorrectEntry = 0;
            break;
         
         case 2:
            //UNR
            do{
               checkRequestCall = unRegisterUser(&packet);
               //returns 0 if unsuccessful UNregistration
            }while(checkRequestCall == 0);

            
            incorrectEntry = 0;
            break;
        
         case 3:
            //QUO
            do{
               checkRequestCall = requestQuote(&packet);
               //returns 0 if unsuccessful quote retuest
            }while(checkRequestCall == 0);

            
            incorrectEntry = 0;
            break;
            
         
         case 4: //testing functionality; invalid command
            //QQQ
            do{
               checkRequestCall = invalidRequest(&packet);
               //returns 0 if unsuccessful invalid request
            }while(checkRequestCall == 0);
            
            incorrectEntry = 0;
            break;
            
            
         default:
            printf("Enter a valid request...\n");
            incorrectEntry = 1;
            break;
      }

   }while (incorrectEntry==1) ;
   
   //THIS IS WHERE THE PACKET IS EDITED FOR SENDING
   strcpy(out_buf, packet);
   
   printf("\nPacket being sent to server: %s \n\n", out_buf);
   
   
   //---------------------------------------------------------------------
    //Christensen code
   // Now send the message to server.  The "+ 1" is for the end-of-string
   // delimiter
   retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
                    (struct sockaddr *)&server_addr, sizeof(server_addr));
   if (retcode < 0)
   {
      printf("*** ERROR - sendto() failed \n");
      exit(-1);
   }
   
   //---------------------------------------------------------------------
   
    //Christensen code
   // Wait to receive a message (need to spin loop on the receive)
   //  - Output a "." for each spin that sleeps for 5 second
   //  WE NEED TO ONLY RESEND TWICE (OR 3) AND WAIT 5 SECONDS
   addr_len = sizeof(server_addr);
#ifdef WIN
   Sleep(1000);  // Windows Sleep for 1000 ms (1 second)
   printf(".");
#endif
#ifdef BSD
   sleep(1);
#endif

   
   retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0,
                      (struct sockaddr *)&server_addr, &addr_len);
   while (retcode <= 0){
#ifdef WIN
      Sleep(5000);  // Windows Sleep for 5000 ms (5 second)
      printf(".");
      retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr));
#endif
#ifdef BSD
      sleep(5);     // Unix sleep for 5 second
      printf("we slept for 5 seconds to wait...\nwe sent a duplicate packet....\n");
      fflush(stdout);
      retcode = sendto(client_s, out_buf, (strlen(out_buf) + 1), 0,
                       (struct sockaddr *)&server_addr, sizeof(server_addr));
      if (retcode < 0)
      {
         printf("*** ERROR - sendto() failed \n");
         exit(-1);
      }
#endif
      retcode = recvfrom(client_s, in_buf, sizeof(in_buf), 0,
                         (struct sockaddr *)&server_addr, &addr_len);
   }
   
   //------------------------------------------------------------------------
   
   //INTERPRET THE DATA
   
   // Output the received message
   printf("Received from server: %s \n", in_buf);
   
   //---------------------------------------------------------------------
   
    //Christensen code
   // Close all open sockets
#ifdef WIN
   retcode = closesocket(client_s);
   if (retcode < 0)
   {
      printf("*** ERROR - closesocket() failed \n");
      exit(-1);
   }
#endif
#ifdef BSD
   retcode = close(client_s);
   if (retcode < 0)
   {
      printf("*** ERROR - close() failed \n");
      exit(-1);
   }
#endif
   
#ifdef WIN
   // This stuff cleans-up winsock
   WSACleanup();
#endif
   
   //---------------------------------------------------------------------
   
   // Return zero and terminate
   return(0);
}


