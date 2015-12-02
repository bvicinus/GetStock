//
//  getStockServer.c
//  
//
//  Created by Brandon Vicinus, Gabriel Sotolongo on 11/25/15.
//
//
//==================================================== file = udpServer.c =====
//=  A message "server" program to demonstrate sockets programming            =
//=============================================================================
//= Code based off udpClient.c written by:
//=  Author: Ken Christensen                                                  =
//=          University of South Florida                                      =
//=          WWW: http://www.csee.usf.edu/~christen                           =
//=          Email: christen@csee.usf.edu                                     =
//=---------------------------------------------------------------------------=
//=  History:  KJC (08/02/08) - Genesis (from server1.c)                      =
//=            KJC (09/07/09) - Minor clean-up                                =
//=            KJC (09/22/13) - Minor clean-up to fix warnings                =
//=============================================================================
#define  BSD                // WIN for Winsock and BSD for BSD sockets

//----- Include files --------------------------------------------------------
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

//----- Defines --------------------------------------------------------------
#define  PORT_NUM   1050    // Arbitrary port number for the server
#define MAX_LEN_PACKET      4095 //max length of a packet, limited by buffer
#define MAX_LEN_USERNAME    32  //standard requirement
#define MAX_LEN_STOCKNAME   6   //5+1 GOOGL is largest stock available
#define MAX_NUM_USERS       20 //arbitrary

//===== Main program =========================================================
int main()
{
#ifdef WIN
   WORD wVersionRequested = MAKEWORD(1,1);       // Stuff for WSA functions
   WSADATA wsaData;                              // Stuff for WSA functions
#endif
   int                  server_s;        // Server socket descriptor
   struct sockaddr_in   server_addr;     // Server Internet address
   struct sockaddr_in   client_addr;     // Client Internet address
   struct in_addr       client_ip_addr;  // Client IP address
   int                  addr_len;        // Internet address length
   char                 out_buf[4096];   // Output buffer for data
   char                 in_buf[4096];    // Input buffer for data
   int                  retcode;         // Return code
   int                  numUsers =0;      //keeps track of number of users
   
   struct userStruct { //structure to easily make an array of username strings
      char username[MAX_LEN_USERNAME];
   };
   
   struct userStruct listUsers[MAX_NUM_USERS]; //array of usernames
   
   
#ifdef WIN
   // This stuff initializes winsock
   WSAStartup(wVersionRequested, &wsaData);
#endif
   
   //---------------------------------------------------------------------

   //Christensen code
   // Create a socket
   //   - AF_INET is Address Family Internet and SOCK_DGRAM is datagram
   server_s = socket(AF_INET, SOCK_DGRAM, 0);
   if (server_s < 0)
   {
      printf("*** ERROR - socket() failed \n");
      exit(-1);
   }
   
   //---------------------------------------------------------------------

   //Christensen code
   // Fill-in my socket's address information
   server_addr.sin_family = AF_INET;                 // Address family to use
   server_addr.sin_port = htons(PORT_NUM);           // Port number to use
   server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
   retcode = bind(server_s, (struct sockaddr *)&server_addr,
                  sizeof(server_addr));
   if (retcode < 0)
   {
      printf("*** ERROR - bind() failed \n");
      exit(-1);
   }
   
   //---------------------------------------------------------------------

   int listenAgain =0;
   
   do{
      //Christensen code
      // Wait to receive a message from client
      printf("Waiting for recvfrom() to complete... \n");
      addr_len = sizeof(client_addr);
      retcode = recvfrom(server_s, in_buf, sizeof(in_buf), 0,
                         (struct sockaddr *)&client_addr, &addr_len);
      if (retcode < 0)
      {
         printf("*** ERROR - recvfrom() failed \n");
         exit(-1);
      }
      
      // Copy the four-byte client IP address into an IP address structure
      memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);
      
      //---------------------------------------------------------------------
      
      //INTERPRET THE RECIEVED MESSAGE FROM THE CLIENT
      
      char responsePacket[MAX_LEN_PACKET];
      
      char receivedPacket[MAX_LEN_PACKET];
      strcpy(receivedPacket, in_buf);

      
      //incoming request, first 3 chars of the packet
      char incomingRequest[4];
      strncpy(incomingRequest, receivedPacket, 3);
      
      //check for valid incoming comand
      if (strcmp(incomingRequest, "REG") != 0
          && strcmp(incomingRequest, "UNR") != 0
          && strcmp(incomingRequest, "QUO") != 0) {
         
         printf("Detected improper request: %s\n", incomingRequest);
         incomingRequest[0] = 'z'; //made to go to default automatically.
      }
      
      switch (incomingRequest[0]) {
         
         case 'R': {//REG
            int i = 4;
            int duplicate = 0;
            char requestedUsername[MAX_LEN_USERNAME];
            
            
            //used to clear the requestedUsername variable!!!!!!!!!!!
            for (int t = strlen(requestedUsername); t > 0; t--) {
               requestedUsername[t-1] = '\0';
            }
            
            while(receivedPacket[i] != ';' && i < MAX_LEN_USERNAME){
               
               //used to check for invalid paramaters
               if (receivedPacket[i] == ',') {
                  duplicate =1; //not actually a duplicate
                  //makes duplicate=1 to bypass username registration
                  strcpy(responsePacket, "INP;");
               }
               
               //take in the username
               requestedUsername[i-4] = tolower(receivedPacket[i]);
               i++;
            }
            
            printf("username: %s \n", requestedUsername);
            
            int count =0;
            
            //checking for duplicates
            for (int n = 0; n <= numUsers; n++) {
               if( strcmp( listUsers[n].username, requestedUsername) == 0){
                  strcpy(responsePacket, "UAE;" );
                  printf("UAE ISSUED\n");
                  duplicate = 1;
               }
               count = n;
                  
            }
               
            if (duplicate != 1) { //if no duplicate found, add user
                  
               struct userStruct additionalUser;
               strcpy(additionalUser.username, requestedUsername);
               listUsers[count] = additionalUser;
               numUsers++;
               printf("added user %s successfully\n", listUsers[count].username);
               duplicate = 0;
               strcpy(responsePacket, "ROK;");

            }
            
            
            break;
         }
            
         case 'U': { //UNR
            
            printf("attempting to unregister username...\n");
            int i = 4;
            int found = -1;
            char requestedUsername[MAX_LEN_PACKET];
            
            //used to clear the requestedUsername variable!!!!!!!!!!!
            for (int t = strlen(requestedUsername); t > 0; t--) {
               requestedUsername[t-1] = '\0';
            }
            
            //skips the first 4 chars of the packet header: URN,
            while(receivedPacket[i] != ';' && i < MAX_LEN_USERNAME){
               
               requestedUsername[i-4] = tolower(receivedPacket[i]);
               i++;
            }
            
            printf("looking for username: %s\n", requestedUsername);
            
            //looking for username...
            for (int n = 0; n <= numUsers; n++) {
               
               if( strcmp( listUsers[n].username, requestedUsername) == 0){
                  found = n;
               }
            }//end for
            
            if (found >= 0) { //if found
               strcpy(responsePacket, "ROK;");
               printf("User %s found and deleted.\n", requestedUsername);
               //have to delete the user now tho
               strcpy(listUsers[found].username, "" );
               
            }
            else { //unfound
               strcpy(responsePacket, "UNR;"); //user not registered
               printf("User %s not found.\n", requestedUsername);
            }

 
            break;
         }
         case 'Q': { //QUO
            
            
            
            int currentCharSpot = 4; //skip the first 4 chars: QUO,
            char tempStockName[MAX_LEN_STOCKNAME] ;
            char incomingUsername[MAX_LEN_USERNAME];
            
            //used to clear the incomingusername variable!!!!!!!!!!!
            for (int t = strlen(incomingUsername); t > 0; t--) {
               incomingUsername[t-1] = '\0';
            }
            
            
            //take in the username
            int temp1 =0;
            while (receivedPacket[currentCharSpot] != ','
                   && receivedPacket[currentCharSpot] != ';'){
               incomingUsername[temp1] = tolower(receivedPacket[currentCharSpot]);
               temp1++;
               currentCharSpot++;
            }
            
            
            //if the user only provides a username and no paramaters,
            //issue invalid parameters error packet
            if (receivedPacket[currentCharSpot] == ';') {
               strcpy(responsePacket, "INP;"); //invalid parameters
               break;
            }
            
            //if username is over 32 chars 
            if (strlen(incomingUsername) >= 33) {
               strcpy(responsePacket, "INU;"); //invalid user name
               break;
            }
            
            
            printf("Username attempting to access with: %s\n", incomingUsername);
            //MAKE SURE THE USERNAME IS VALID HERE!!!!!!!!!!!!!!!!!!!!!
            
            int found = -1;
            
            for (int n = 0; n <= numUsers; n++) {
               
               if( strcmp( listUsers[n].username, incomingUsername) == 0){
                  found = n;
               }
            }//end for
            
            if (found >= 0) { //valid username found in list of usernnames
               printf("Valid username: %s\n" , incomingUsername);
               strcpy(responsePacket, "ROK,");

            }
            else { //username unfound, dont check stock prices!
               strcpy(responsePacket, "UNR;");
               //this will auto not enter the loop to access the stock values
               receivedPacket[currentCharSpot] = ';';
            }

            

            
            while(receivedPacket[currentCharSpot] != ';'){
               
               currentCharSpot++;//skip the comma
               
               //used to clear the tempstockname variable!!!!!!!!!!!
               for (int t = strlen(tempStockName); t > 0; t--) {
                  tempStockName[t-1] = '\0';
               }
               
               int counter = 0;
               //loop peels off a stock name
               while (receivedPacket[currentCharSpot] != ',' && receivedPacket[currentCharSpot] != ';') {
                  tempStockName[counter] = receivedPacket[currentCharSpot];
                  currentCharSpot++;
                  counter++;
               }
               
               //should have made this case insensative, but overlooked that
               //in the standard.  could have easily implemented this
               if (strcmp(tempStockName, "FB") == 0) {
                  printf("Looking up FB stock info...\n");
                  strncat(responsePacket, "94.01," , MAX_LEN_PACKET);
                  
               }
               else if (strcmp(tempStockName, "APPL") == 0) {
                  printf("Looking up APPL stock info...\n");
                  strncat(responsePacket, "121.30," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "NFLX") == 0) {
                  printf("Looking up NFLX stock info...\n");
                  strncat(responsePacket, "114.31," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "GOOGL") == 0) {
                  printf("Looking up GOOGL stock info...\n");
                  strncat(responsePacket, "625.61," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "IBM") == 0) {
                  printf("Looking up IBM stock info...\n");
                  strncat(responsePacket, "161.99," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "CSCO") == 0) {
                  printf("Looking up CSCO stock info...\n");
                  strncat(responsePacket, "28.42," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "JNPR") == 0) {
                  printf("Looking up JNPR stock info...\n");
                  strncat(responsePacket, "28.42," , MAX_LEN_PACKET);

               }
               else if (strcmp(tempStockName, "MOBL") == 0) {
                  printf("Looking up MOBL stock info...\n");
                  strncat(responsePacket, "5.72," , MAX_LEN_PACKET);

               }
               else {
                  printf("other... %s \n" , tempStockName);
                  strncat(responsePacket, "-1," , MAX_LEN_PACKET);

               }
               
            }//end while loop
            
            //replace the last comma with a semicolin
            responsePacket[strlen(responsePacket) -1] = ';';

            
            break;
         }
         default: {
            //invalid command responce code yo
            strcpy(responsePacket, "INC;");
            break;
         }
      }//end while
      
      
      // Output the received message
      printf("Received from client: %s \n", receivedPacket);
      printf("Packet being sent: %s\n", responsePacket);
      
      
      //---------------------------------------------------------------------
      
      //SEND RESPONSE TO THE CLIENT BASED UPON RECEIVED PACKET
      
      // Send to the client using the server socket
      strcpy(out_buf, responsePacket);
       //Christensen code
      retcode = sendto(server_s, out_buf, (strlen(out_buf) + 1), 0,
                       (struct sockaddr *)&client_addr, sizeof(client_addr));
      if (retcode < 0)
      {
         printf("*** ERROR - sendto() failed \n");
         exit(-1);
      }
      
      //prompt user to ask if they want to listen for another message
      printf("\nListen for another message?\n Enter 1 for yes, 0 for no.\n");
      scanf("%d", &listenAgain);
      
      
   }while(listenAgain == 1);
   
   
   
   //=======================================================================
   //Christensen code
   // Close all open sockets
#ifdef WIN
   retcode = closesocket(server_s);
   if (retcode < 0)
   {
      printf("*** ERROR - closesocket() failed \n");
      exit(-1);
   }
#endif
#ifdef BSD
   retcode = close(server_s);
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
   
   // Return zero and terminate
   return(0);
}//end main


