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
sig_atomic_t sigFlagAlrm = 0;

void signal_handler(int iSignal);
void signal_handler(int iSignal) {

  switch (iSignal) {

  case SIGINT:
    sigFlagStop = -1;
    break;
  case SIGALRM:
    sigFlagAlrm = -1;
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

int response;
int returnVAlue;
int nbrOctSent;
char* resPoten[10];
float resPotenOld;


int main(int iArgCount, char * apstrArgValue[]) {

  int iResult = 0;
  int iVerif = 0;

  //time now
  time_t timeNow = time(NULL);
  unsigned long seconds = difftime(timeNow, 0);
  unsigned long oldSeconds;

  // fork variables
  pid_t pid = 0;
  int a2iFdPipe[2];
  char strMessage[] = "message";
  char strMessageRecu[7];

  char * tmp;

  int buttonsState = 5;
  int buttonsStateOld = 6;
  int buttonsState1Old = 6;
  int buttonsState2Old = 6;
  int buttonsState1 = 5;
  int buttonsState2 = 5;
  int buttonsStateReceive = 5;

  int x = 75;
  union buffer dataToReceive;
  union buffer data;
  memset( & dataToReceive, 0, 2);

  // Oled screen
  struct TDevice oledScreen = {
    .m_strPathDev = "/dev/ttyAL0",
    .m_iDescIo = -1,
  };

  // Buttons on Both Edges (x o o x)							
  struct TDeviceCharGpIO gpioButton1 = {
    .m_DevChar = {
      .m_iDescIo = -1,
      .m_strPathDev = "/dev/gpiochip3",
    },
  };
  	
  	struct TDevice cdevAdcIA0 = {
			.m_strPathDev 	= "/dev/ad7928c0IA0",
			.m_iDescIo 		= -1,
	};
	
  // definitions of structure of the screen
  struct termios termiosOled;
  struct termios termiosOledInit;

  // opening of devices
  oledScreen.m_iDescIo = open(oledScreen.m_strPathDev, (O_RDWR | O_NONBLOCK));
  gpioButton1.m_DevChar.m_iDescIo = open(gpioButton1.m_DevChar.m_strPathDev, O_RDWR);
  cdevAdcIA0.m_iDescIo = open(cdevAdcIA0.m_strPathDev,O_RDONLY); // Ouverture du potentiomètre

  // signals used on this project,
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

  // init 
  // get back current config
  iVerif = tcgetattr(oledScreen.m_iDescIo, & termiosOled);

  // copy the init to reset at the end of the program
  memcpy( & termiosOledInit, & termiosOled, sizeof(struct termios));

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
    iVerif = tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOled);
    usleep(110000);
    nbrOctSent = write(oledScreen.m_iDescIo, & setBaudRate, sizeof(setBaudRate));
    usleep(110000);
    //returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
    //fprintf(stdout, "Return value : %i\n", returnVAlue);
    cfsetospeed( & termiosOled, B115200);
    usleep(110000);
    // set profile	a second time
    iVerif = tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOled);
    usleep(110000);
  }
  // create pipe to transfer data between forks
  iResult = pipe(a2iFdPipe);
  if (0 == iResult) {

    // forking
    pid = fork();
    printf("FORK %i :\n", pid); // Duplication of the process

    switch (pid) { // Switch pour pere/fils

    case -1: // Si erreur alors on ferme toute communication
      fprintf(stdout, "ECHEC\n");
      close(a2iFdPipe[0]);
      close(a2iFdPipe[1]);
      iResult = -1;
      break;

      // button handler (exterior devices)
    case 0: // Fils
      close(a2iFdPipe[0]);

      int temp = 0;
      int temp2 = 0;
      int temp3 = 1;
      float pourcent = 0;
      char* tempString[10];
      while (sigFlagStop != -1) {
        
        // max value 4052
        ReadAdcIA(cdevAdcIA0.m_iDescIo,resPoten,sizeof(resPoten));
        //printf("%s\n",resPoten);
        
        pourcent = (float)(atoi(resPoten)-2526)/1526;
        sprintf(tempString, "%.*f", 2, pourcent);
        pourcent = atof(tempString);
        
        //printf("pourcent : %f\n",pourcent);
        
        
        /*float original = 4.48;

int tmp = original * 10; // 44.8 truncated to 44

float truncated = tmp / 10.0; // 4.4*/

	      if (pourcent < -1){
	          pourcent = -1;
      	}

        
        if (pourcent != resPotenOld){
		      
		      
		      //printf("%'.2f\n",pourcent);

        	write(a2iFdPipe[1],&pourcent,4);
        		
        	resPotenOld = pourcent;
        	//printf("pourcent : %f\n",pourcent);
        }
        //printf("%s\n",resPoten);
        //if (resPoten == resPotenOld)
        
        //int test = atoi(resPoten);
        //printf("atoi : %i\n",test);
        //rusleep(100000);
        
        
        
        
        usleep(100000);

      }
      // when close program
      close(gpioButton1.m_DevChar.m_iDescIo);
      close(cdevAdcIA0.m_iDescIo);
      close(a2iFdPipe[1]);
      break;

      ////////////////////////////////////////////////////////////////////////////////////////////

      // OLED screen handler
    default: // Pere
      close(a2iFdPipe[1]);
      int tempMove = 1;
      
      	
			
			printTriangle(oledScreen.m_iDescIo, 40, 40, 4);

      /*write(oledScreen.m_iDescIo, & moveCursor, sizeof(moveCursor));
      usleep(7000);
      returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
			
      
      write(oledScreen.m_iDescIo, & textSpaceInvider, sizeof(textSpaceInvider));
      usleep(7000);
      returnVAlue = read(oledScreen.m_iDescIo, & response, 1);*/
      writeText(oledScreen.m_iDescIo, 20, 63, "Space Invider");

      writeText(oledScreen.m_iDescIo, 30, 90, "SAE");

      usleep(50000);

      //write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
      //usleep(50000);
      //read(oledScreen.m_iDescIo, & response, 1);
      fd_set readSet;
      
      float potenReceive = 0;
      
      int vitesse = 0;
      

      while (sigFlagStop != -1) {

        // reading of buttons states
        /*alarm(1);
        iResult = read(a2iFdPipe[0], &buttonsStateReceive, 1);*/

        FD_ZERO( & readSet);
        FD_SET(a2iFdPipe[0], & readSet);

        // Set timeout for select to zero for non-blocking check
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50;
        
				tempMove = 1;
				
        // Use select to check for readability with zero timeout
        int ready = select(a2iFdPipe[0] + 1, & readSet, NULL, NULL, & timeout);

        if (ready > 0) {
          // Data is ready to be read
          iResult = read(a2iFdPipe[0], & potenReceive, 4);
          if (iResult == -1) {
            // Handle other read errors if needed
            //perror("read");
            break;
          }

          // Process the read data if needed
        } else if (ready == 0) {
          // No data available, do something else or continue the loop
          //tempMove = 0;
        } else {
          // Handle select error
          //perror("select");
          break;
        }
        
        
				//printf("%'.2f\n",potenReceive);
        
        
        
        /*if((x>8) && (potenReceive<0)){
        	//printf("%'.2f\n",(2*potenReceive));
					x = x + (2*potenReceive);
        }
        else if((x< 120) && (potenReceive>0)){
        	//printf("%'.2f\n",(2*potenReceive));
					x = x + (2*potenReceive);
        }*/
        
				if(((x>8) && (potenReceive<0))||((x< 120) && (potenReceive>0))){
					vitesse = 2*potenReceive;
					x = x + vitesse;
				}
		    
		    
		    
		    if (vitesse !=0){
		    
		    	
          for (int iBcl4 = 1;iBcl4<=abs(vitesse);iBcl4++){
          
          	
          	// vers la gauche
						if ((x>8) && (potenReceive<0)){
							CircleBlack[3] = BufferTableCircle[3]+(-iBcl4+1);
							//printf("%i\n",iBcl4);
						}
						// vers la droite
						if ((x< 120) && (potenReceive>0)){
							CircleBlack[3] = BufferTableCircle[3]-(iBcl4-1);
						}
            SendCommand(oledScreen.m_iDescIo, & CircleBlack, sizeof(CircleBlack));
          }
				  // modifications on the table of char
			  	
			  	BufferTableCircle[3] = x;
				  //tmp = blackRectangle;
				  //SendCommand(oledScreen.m_iDescIo,tmp);
				  /*printf("Etat read : %i\n",returnVAlue);
				  printf("Reponse : %i\n",response);*/

				  // sending command to display main  character
				  SendCommand(oledScreen.m_iDescIo, & BufferTableCircle, sizeof(BufferTableCircle));
				  //tmp = BufferTableCircle;
				  //SendCommand(oledScreen.m_iDescIo,tmp);
				  /*printf("Nbr octets envoyés : %i\n",nbrOctSent);
				  printf("Nbr octets recu : %i\n",returnVAlue);
				  printf("Reponse : %i\n",response);*/
				  //tempMove = 0;
         }
        
        
        

        
        



        //while((difftime( time( NULL ), 0 ) - oldSeconds) < 1){}
      }

      // ending program
      // clear screen
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
      printf("fin");
      tcsetattr(oledScreen.m_iDescIo, TCSANOW, & termiosOledInit);
      close(oledScreen.m_iDescIo);
      close(a2iFdPipe[0]);
    }
  }

  return iResult;
}
