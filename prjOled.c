/* ---------------------------------------------------------------------
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ioctlgpio.c
 *
 *   This is a small example of ioctl() accesses to the  GPIO user-space
 *   API under Linux (list the GPIO controllers and theirs lines,
 *   to read/write data, to wait on a input GPIO line for some events.
 *
 * Copyright (C) 2023 GEii IUT-Acy, USMB.
 *
 * Author:
 * 	Pascal Aygalinc <pascal.aygalinc@univ-smb.fr>
 * ---------------------------------------------------------------------
 */

#include "prjOled.h"

sig_atomic_t sigFlagStop = 0;

void signal_handler(int iSignal);
void signal_handler(int iSignal) {

switch (iSignal) {

  case SIGINT:
    sigFlagStop = -1;
    break;
  case SIGALRM:
    sigFlagStop = -1;
  default:
    break;
  }
}

struct dataMix16Separatebits {

  short int dataAlone8bitsLOW;
  short int dataAlone8bitsHIGH;
};

union buffer {

  struct dataMix16Separatebits commandStruct;
  unsigned short command;
};


void send16bits(int iFd, union buffer data) {
  write(iFd, & data.commandStruct.dataAlone8bitsHIGH, sizeof(data.commandStruct.dataAlone8bitsHIGH));
  write(iFd, & data.commandStruct.dataAlone8bitsLOW, sizeof(data.commandStruct.dataAlone8bitsHIGH));
}











int main(int iArgCount, char * apstrArgValue[]) {

  int iResult = 0;
  int iVerif = 0;
  
  //time now
  time_t timeNow = time( NULL );
  unsigned long seconds = difftime( timeNow, 0 );
  unsigned long oldSeconds;

  // fork variables
  pid_t pid = 0;
  int a2iFdPipe[2];
  char strMessage[] = "message";
  char strMessageRecu[7];
  
  int response;
  int returnVAlue;
  int nbrOctSent;

  char BufferTable[12] = {
    0xff,
    0xCF,
    0x00,
    0x0A,
    0x00,
    0x14,
    0x00,
    0x5A,
    0x00,
    0x3C,
    0xF8,
    0x11
  };
  
  // rayon : 6
  char BufferTableCircle[12] = {
    0xFF,
    0xCC,
    0x00,
    0x07,
    0x00,
    0x78,
    0x00,
    0x06,
    0x84,
    0x10
  };
   char BufferTableCircleBlack[12] = {
    0xFF,
    0xCC,
    0x00,
    0x07,
    0x00,
    0x78,
    0x00,
    0x06,
    0x00,
    0x00
  };
  
  char blackRectangle[12] = {
   0xFF, 0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x7F, 0x00, 0x00
  };
  

  char clearScreen[2] = {0xFF, 0xD7};
  char setBaudRate[] = {0x00, 0x0B, 0x00, 0x19};
  
  int buttonsState = 5;
  int buttonsStateReceive = 5;

  int x = 75;
  union buffer dataToReceive;
  union buffer data;
  memset( & dataToReceive, 0, 2);
  struct TDevice oledScreen = {
    .m_strPathDev = "/dev/ttyAL0",
    .m_iDescIo = -1,
  };

	
	// Buttons on Both Edges (x o o x)							
	struct TDeviceCharGpIO gpioButton1 	= 	{	.m_DevChar	= {	.m_iDescIo 		= -1,
																					.m_strPathDev 	= "/dev/gpiochip3", },
														};	
  
  
  struct termios termiosOled;
  
  struct termios termiosOledInit;
  
  

  oledScreen.m_iDescIo = open(oledScreen.m_strPathDev, (O_RDWR | O_NONBLOCK));
  gpioButton1.m_DevChar.m_iDescIo = open(gpioButton1.m_DevChar.m_strPathDev, O_RDWR);

  //signal(SIGINT,signal_handler);
  signal(SIGALRM, signal_handler);
  signal(SIGINT, signal_handler);


  // init buttons
  memset( & gpioButton1.m_chipinfo, 0, sizeof(struct gpiochip_info)); // ChipInfoButtons1
  memset( & gpioButton1.m_request, 0, sizeof(struct gpiohandle_request)); // RequestButtons1

  ioctl(gpioButton1.m_DevChar.m_iDescIo, GPIO_GET_CHIPINFO_IOCTL, & gpioButton1.m_chipinfo); // ChipInfoButtons1

  for (int iBcl2 = 0; iBcl2 <= 1; iBcl2++) {
    gpioButton1.m_event_request[iBcl2].lineoffset = iBcl2;
    gpioButton1.m_event_request[iBcl2].handleflags = GPIOHANDLE_REQUEST_INPUT;
    gpioButton1.m_event_request[iBcl2].eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
    ioctl(gpioButton1.m_DevChar.m_iDescIo, GPIO_GET_LINEEVENT_IOCTL, & gpioButton1.m_event_request[iBcl2]);
    gpioButton1.m_pollfd[iBcl2].fd = gpioButton1.m_event_request[iBcl2].fd;
    gpioButton1.m_pollfd[iBcl2].events = POLLIN;
  }
  //alarm(5);

  // get back current config
  iVerif = tcgetattr(oledScreen.m_iDescIo, & termiosOled);
  termiosOledInit = termiosOled;
  if (iVerif == 0) {


    // ignore signals of terminals
    termiosOled.c_iflag &= ~CLOCAL;
    // set profile	
    iVerif = tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOled);

    // block mode
    iVerif = fcntl(oledScreen.m_iDescIo, F_SETFL, ~O_NONBLOCK);
    
    // manual config
    // put default setup (merguez)
    cfmakeraw( & termiosOled);

    // set parameter
    termiosOled.c_cflag &= ~(CSTOPB);
    
    /*cfsetispeed( & termiosOled, B9600);
    cfsetospeed( & termiosOled, B9600);*/
    cfsetispeed( & termiosOled, B115200);
    cfsetospeed( & termiosOled, B9600);
   nbrOctSent = write(oledScreen.m_iDescIo, & setBaudRate, sizeof(setBaudRate));
   usleep(50000);
   returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
   fprintf(stdout, "Return value : %i\n",returnVAlue);
   cfsetospeed( & termiosOled, B115200);

    // set profile	a second time
    iVerif = tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOled);
     

  }

  printf("1\n");
  iResult = pipe(a2iFdPipe);
  if (0 == iResult) {
    printf("2\n");
    pid = fork();
    printf("FORK %i :\n", pid); // Duplication du processus

    switch (pid) { // Switch pour pere/fils
    case -1: // Si erreur alors on ferme toute communication
      fprintf(stdout, "ECHEC\n");
      close(a2iFdPipe[0]);
      close(a2iFdPipe[1]);
      iResult = -1;
      break;

      // buttons
    case 0: // Fils
      close(a2iFdPipe[0]);

      int temp = 0;
      while (sigFlagStop != -1) {
        poll( & gpioButton1.m_pollfd[1], 1, 100);
        poll( & gpioButton1.m_pollfd[0], 1, 100);
        
        if (gpioButton1.m_pollfd[1].revents == gpioButton1.m_pollfd[1].events) {
          read(gpioButton1.m_event_request[1].fd, & (gpioButton1.m_event_data[1]), sizeof(gpioButton1.m_event_data[1]));
          fprintf(stdout,"Bouton 1\n");
          if (temp == 0) {
            temp = 1;
            buttonsState = 0;
          } else {
            temp = 0;

            buttonsState = 1;
          }
        }
        
        if (gpioButton1.m_pollfd[0].revents == gpioButton1.m_pollfd[0].events) {
          read(gpioButton1.m_event_request[0].fd, & (gpioButton1.m_event_data[0]), sizeof(gpioButton1.m_event_data[0]));
          fprintf(stdout,"Bouton 2\n");
          if (temp == 0) {
            temp = 1;
            buttonsState = 2;
          } else {
            temp = 0;

            buttonsState = 3;
          }
        }
        
        //fprintf(stdout,"Send : %i\n",buttonsState);
        
        write(a2iFdPipe[1],&buttonsState,1); 
        
      }
      close(gpioButton1.m_DevChar.m_iDescIo);
      close(a2iFdPipe[1]);
      break;

////////////////////////////////////////////////////////////////////////////////////////////

      // écran OLED
    default: // Pere
    
    // clear screen
      close(a2iFdPipe[1]);
      int tempMove = 1;
      

      while (sigFlagStop != -1) {
      
        iResult = read(a2iFdPipe[0],&buttonsStateReceive, 1); 
        //fprintf(stdout,"Receive : %i\n",buttonsStateReceive);
        
        if((buttonsStateReceive == 0) && (x > 12)){
            x = x - 6;
            tempMove = 1 ;
        }
        else if((buttonsStateReceive == 2) && (x < 116)){
            x = x + 6;
            tempMove = 1 ;
        }
        else{
            tempMove = 0 ;
        }          
        
        BufferTableCircle[3] = x;
        

        
         if(tempMove == 1 ){
            nbrOctSent = write(oledScreen.m_iDescIo, & blackRectangle, sizeof(blackRectangle));
            usleep(7000);
            returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
           /*printf("Etat read : %i\n",returnVAlue);
           printf("Reponse : %i\n",response);*/
         }
         
         //BufferTableCircleBlack[3] = x;
         
        nbrOctSent = write(oledScreen.m_iDescIo, & BufferTableCircle, sizeof(BufferTableCircle));
        usleep(7000);
        returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
        /*printf("Nbr octets envoyés : %i\n",nbrOctSent);
        printf("Nbr octets recu : %i\n",returnVAlue);
        printf("Reponse : %i\n",response);*/

        
        
        
        
        //while((difftime( time( NULL ), 0 ) - oldSeconds) < 1){}
    }
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
    printf("fin");
    tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOledInit);
    close(oledScreen.m_iDescIo);
    close(a2iFdPipe[0]);
  }
  }
  //BufferTableCircle[12]	

  return iResult;
}
