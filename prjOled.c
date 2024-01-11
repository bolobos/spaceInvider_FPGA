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

  int response;
  int returnVAlue;
  int nbrOctSent;

  // definitions of different commands

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
    0xFF,
    0xCE,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x7F,
    0x00,
    0x7F,
    0x00,
    0x00
  };
  
  char moveCursor[] = {
      0xFF, 0xE4, 0x00, 0x3F, 0x00, 0x3F
  };
  
  char textSpaceInvider[] = {
      0x00, 0x06, 0x53, 0x50, 0x41, 0x43, 0x45, 0x20, 0x49, 0x4E, 0x56, 0x49, 0x44, 0x45, 0x52, 0x00
  };
  


  char clearScreen[2] = {
    0xFF,
    0xD7
  };
  char setBaudRate[] = {
    0x00,
    0x0B,
    0x00,
    0x19
  };

  char frameDelaySet[] = {
    0xFF, 
    0x69, 
    0x00, 
    0x01
  };
  
  char *tmp ;

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

  // definitions of structure of the screen
  struct termios termiosOled;
  struct termios termiosOledInit;

  // opening of devices
  oledScreen.m_iDescIo = open(oledScreen.m_strPathDev, (O_RDWR | O_NONBLOCK));
  gpioButton1.m_DevChar.m_iDescIo = open(gpioButton1.m_DevChar.m_strPathDev, O_RDWR);

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
  memcpy(&termiosOledInit,&termiosOled,sizeof(struct termios));
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
      while (sigFlagStop != -1) {

        // polling on two buttons
        poll( & gpioButton1.m_pollfd[1], 1, 100);
        poll( & gpioButton1.m_pollfd[0], 1, 100);

        // press the button 1
        if (gpioButton1.m_pollfd[1].revents == gpioButton1.m_pollfd[1].events) {
          read(gpioButton1.m_event_request[1].fd, & (gpioButton1.m_event_data[1]), sizeof(gpioButton1.m_event_data[1]));
          //fprintf(stdout, "Bouton 1\n");
          if (temp == 0) {
            temp = 1;
            buttonsState1 = 1;
          } else {
            temp = 0;

            buttonsState1 = 2;
          }
            if(temp3 == 0){
               write(a2iFdPipe[1], & buttonsState1, 1);
               //fprintf(stdout,"Send : %i\n",buttonsState1);
            }
            
        }

        // press the button 2
        if (gpioButton1.m_pollfd[0].revents == gpioButton1.m_pollfd[0].events) {
          read(gpioButton1.m_event_request[0].fd, & (gpioButton1.m_event_data[0]), sizeof(gpioButton1.m_event_data[0]));
          //fprintf(stdout, "Bouton 2\n");
          if (temp2 == 0) {
            temp2 = 1;
            buttonsState2 = 3;
          } else {
            temp2 = 0;

            buttonsState2 = 4;
            
          }
            if(temp3 == 0){
               write(a2iFdPipe[1], & buttonsState2, 1);
               //fprintf(stdout,"Send : %i\n",buttonsState2);
            }
            else{
               temp3 = 0;
            }
        }
        /*
        if((buttonsState2 == 3) && ((temp2 || temp) == 1)){
            write(a2iFdPipe[1], & buttonsState2, 1);
            fprintf(stdout,"Send : %i\n",buttonsState2);
        }
        
        else if((buttonsState1 == 1) && ((temp2 || temp) == 1)){
            write(a2iFdPipe[1], & buttonsState1, 1);
            fprintf(stdout,"Send : %i\n",buttonsState1);
        }
        */
        /*if(buttonsState1Old != buttonsState1){
            buttonsState1Old = buttonsState1;
            buttonsState = buttonsState1;
            write(a2iFdPipe[1], & buttonsState, 1);
            fprintf(stdout,"Send : %i\n",buttonsState);
        }
        
        else if(buttonsState2Old != buttonsState2){
            buttonsState2Old = buttonsState2;
            buttonsState = buttonsState2;
            write(a2iFdPipe[1], & buttonsState, 1);
            fprintf(stdout,"Send : %i\n",buttonsState);
            
        }*/
        /*if (buttonsStateOld != buttonsState){
            buttonsStateOld = buttonsState;
            write(a2iFdPipe[1], & buttonsState, 1);
            fprintf(stdout,"Send : %i\n",buttonsState);
            sigFlagAlrm = 0;
        }*/
        
        
         usleep(500);
        
        

      }
      // when close program
      close(gpioButton1.m_DevChar.m_iDescIo);
      close(a2iFdPipe[1]);
      break;

      ////////////////////////////////////////////////////////////////////////////////////////////

    // OLED screen handler
    default: // Pere

      close(a2iFdPipe[1]);
      int tempMove = 1;
      
      write(oledScreen.m_iDescIo, & moveCursor, sizeof(moveCursor));
      usleep(7000);
      returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
      
      write(oledScreen.m_iDescIo, & textSpaceInvider, sizeof(textSpaceInvider));
      usleep(7000);
      returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
      
      usleep(5000000);
      
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
      

      while (sigFlagStop != -1) {

        // reading of buttons states
        /*alarm(1);
        iResult = read(a2iFdPipe[0], &buttonsStateReceive, 1);*/
        
        
        fd_set readSet;
        FD_ZERO(&readSet);
        FD_SET(a2iFdPipe[0], &readSet);

        // Set timeout for select to zero for non-blocking check
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 50;

        // Use select to check for readability with zero timeout
        int ready = select(a2iFdPipe[0] + 1, &readSet, NULL, NULL, &timeout);

        if (ready > 0) {
            // Data is ready to be read
            iResult = read(a2iFdPipe[0], &buttonsStateReceive, 1);
            if (iResult == -1) {
                // Handle other read errors if needed
                perror("read");
                break;
            }

            // Process the read data if needed
        } else if (ready == 0) {
            // No data available, do something else or continue the loop
        } else {
            // Handle select error
            perror("select");
            break;
        }
        
       /*while (sigFlagAlrm != -1) {
           char buttonsStateReceive;
           ssize_t iResult;

           // Handling EINTR for read
           do {
               
           } while (iResult == -1 && errno == EINTR);

           if (iResult == -1) {
               // Handle other read errors if needed
               perror("read");
               break;
           }

           // Process the read data if needed
       }
       // Reset the signal handler for SIGALRM
         signal(SIGALRM, SIG_DFL);*/
        //fprintf(stdout,"Receive : %i\n",buttonsStateReceive);
        //fprintf(stdout,"Nombre recu :  %i\n",buttonsStateReceive);
        
        // define x of the protagonist
        BufferTableCircleBlack[3] = BufferTableCircle[3];
        
        if ((buttonsStateReceive == 1) && (x > 13)) {
          x = x - 2;
          tempMove = 1;
        } else if ((buttonsStateReceive == 3) && (x < 115)) {
          x = x + 2;
          tempMove = 1;
        } else {
          tempMove = 0;
        }
        
        BufferTableCircle[3] = x;


        // execute the reset only if event occur
        if (tempMove == 1) {
         // modifications on the table of char
          write(oledScreen.m_iDescIo, & BufferTableCircleBlack, sizeof(BufferTableCircleBlack));
          usleep(5000);
          returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
          //tmp = blackRectangle;
          //SendCommand(oledScreen.m_iDescIo,tmp);
          /*printf("Etat read : %i\n",returnVAlue);
          printf("Reponse : %i\n",response);*/
        

        // sending command to display main  character
         write(oledScreen.m_iDescIo, & BufferTableCircle, sizeof(BufferTableCircle));
          usleep(5000);
          returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
          //tmp = BufferTableCircle;
        //SendCommand(oledScreen.m_iDescIo,tmp);
        /*printf("Nbr octets envoyés : %i\n",nbrOctSent);
        printf("Nbr octets recu : %i\n",returnVAlue);
        printf("Reponse : %i\n",response);*/
        
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
