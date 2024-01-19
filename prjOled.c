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











      struct mob {
        int x;
        int y;
        int hp;
        char model[12];
      };

      struct missile {
        int x;
        int y;
        int damage;
        char model[12];
      };

      struct ship {
        int x;
        int rate;
        int damage;
        int specialdamage;
        char model[];
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

  int xbis = 75;
  union buffer dataToReceive;
  union buffer data;
  memset( & dataToReceive, 0, 2);
  
  
  
      
      
      
      
      
      
      
      
      
      
      
      
      
      


      struct mob TabMob[50];
      struct missile TabMissile[20];
      struct ship MasterShip;
      
                struct timeval Time;
          struct timezone Balec;
      
      
      
      
      
     int leave = 0;
      int play = 0;
      int compteur = 0;
      int iBcl2 = 0;
      int iBcl3 = 0;
      int iBcl4 = 0;
      int iBcl5 = 0;
      srand(time(NULL));
      int xTable[6] = {
        11,
        32,
        53,
        74,
        95,
        116
      };
      int x = 0;
      int A1 = 0;
      int A2 = 0;
      int A3 = 0;
      int A4 = 0;
      int A5 = 0;
      int A6 = 0;
      int enablemob = 0;
      int enablemiss = 0;
      unsigned long long t = 0;
      unsigned long long tpre = 0;
      unsigned long long ut = 0;
      unsigned long long utpre = 0;
      
      
            for (int iBcl = 0; iBcl < 50; iBcl++) {
        TabMob[iBcl].x = -1;
        TabMob[iBcl].y = 10;
        TabMob[iBcl].hp = 50;
       
        memcpy( & (TabMob[iBcl].model), & RondHaut, sizeof(RondHaut));

        if (iBcl < 20) {
          TabMissile[iBcl].x = -1;
          TabMissile[iBcl].y = 117;
          TabMissile[iBcl].damage = 50;
          memcpy( & (TabMissile[iBcl].model), & RondBas, sizeof(RondBas));

        }
      }

      MasterShip.x = 64;
      MasterShip.rate = 500;
      MasterShip.damage = 50;
      MasterShip.specialdamage = 100;
      memcpy( & (MasterShip.model), & RondBasCentre, sizeof(RondBasCentre));      
      
      TabMissile[iBcl4].x = - 1; 
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      
      

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
      char* stringButton[2];
      int concatenateData = 0;
      char * concatenateString[20];
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
            //fprintf(stdout, "1\n");
            buttonsState1 = 1;
          } else {
            temp = 0;

            buttonsState1 = 2;
            //fprintf(stdout, "2\n");
          }
          /*if (temp3 == 0) {
            write(a2iFdPipe[1], & buttonsState1, 1);
            //fprintf(stdout,"Send : %i\n",buttonsState1);
          }*/

        }

        // press the button 2
        if (gpioButton1.m_pollfd[0].revents == gpioButton1.m_pollfd[0].events) {
          read(gpioButton1.m_event_request[0].fd, & (gpioButton1.m_event_data[0]), sizeof(gpioButton1.m_event_data[0]));
          
          if (temp2 == 0) {
            temp2 = 1;
            buttonsState2 = 3;
            //fprintf(stdout, "3\n");
          } else {
            temp2 = 0;
						//fprintf(stdout, "4\n");
            buttonsState2 = 4;

          }
          
          }
        
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
      	
      	
				//sprintf(stringButton, "%i", buttonsState1);
				
				// concatenate datas
				/*strcpy(&concatenateString,&tempString);
				strcat(&concatenateString,".");
				strcat(&concatenateString,&stringButton);*/
				
				
        concatenateData = (100+pourcent*100) + buttonsState1*100000;

        //printf("%s\n",resPoten);
        //if (resPoten == resPotenOld)
        
        //int test = atoi(resPoten);
        //printf("atoi : %i\n",test);
        //rusleep(100000);
        
        
        if ((pourcent != resPotenOld) || (buttonsState1 != buttonsState1Old)){
        
      		//printf("String : %s\n",concatenateString);
        	write(a2iFdPipe[1],&concatenateData,4);
        	//printf("DATA : %i\n",concatenateData);
        
        }
        
        
        if (pourcent != resPotenOld){
        	resPotenOld = pourcent;
        }
        if (buttonsState1 != buttonsState1Old){
        	buttonsState1Old = buttonsState1;
        }
        
        usleep(10000);

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
      
      //SendCommand(oledScreen.m_iDescIo,&clearScreen,sizeof(clearScreen),100000);
      
      
      char logo[8][11] = {{0,0,1,0,0,0,0,0,1,0,0},{0,0,0,1,0,0,0,1,0,0,0},{0,0,1,1,1,1,1,1,1,0,0},{0,1,1,0,1,1,1,0,1,1,0},{1,1,1,1,1,1,1,1,1,1,1},{1,0,1,1,1,1,1,1,1,0,1},{1,0,1,0,0,0,0,0,1,0,1},{0,0,0,1,1,0,1,1,0,0,0}};
      backgroundColor[2] = 0x50;
      backgroundColor[3] = 0xDE;
      SendCommand(oledScreen.m_iDescIo,&backgroundColor,sizeof(backgroundColor),100000);
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
      
  	//logoPrint(oledScreen.m_iDescIo,logo, 20, 20);

			//printTriangle(oledScreen.m_iDescIo, 40, 40, 4);
			
			
			textColor[2] = 248;
			textColor[3] = 64;
      textBackground[2] = 0x50;
      textBackground[3] = 0xDE;
      SendCommand(oledScreen.m_iDescIo,&textBackground,sizeof(textBackground),5000);
			SendCommand(oledScreen.m_iDescIo,&textColor,sizeof(textColor),5000);
			SendCommand(oledScreen.m_iDescIo,&characterHeight,sizeof(characterHeight),5000);
			SendCommand(oledScreen.m_iDescIo,&characterWidth,sizeof(characterWidth),5000);
			
			printf("Aye\n");
      sleep(1);
      enum trajLogo{
        DROIT,
        BAS,
        GAUCHE,
        HAUT
      };

      int etat = DROIT;
      int xLogo = 10;
      int yLogo = 20;
      blackRectangle[10] = 0x50;
      blackRectangle[11] = 0xDE;

      for(int iBcl8=0;iBcl8 < 100;iBcl8++){
        textColor[2] = 0+iBcl8;
        textColor[3] = 10+iBcl8;
        blackRectangle[3] = xLogo;
        blackRectangle[5] = yLogo;
        blackRectangle[7] = xLogo+11;
        blackRectangle[9] = yLogo+8;
        SendCommand(oledScreen.m_iDescIo, & blackRectangle, sizeof(blackRectangle),7000);
        SendCommand(oledScreen.m_iDescIo,&textColor,sizeof(textColor),6000);
        writeText(oledScreen.m_iDescIo, 20, 30, "Space Invider",sizeof("Space Invider"));
        
        writeText(oledScreen.m_iDescIo, 50, 40, "SAE",sizeof("SAE"));
        
        switch (etat) {
        case DROIT:
          // code block
          xLogo = xLogo +2;
          if (iBcl8 > 40){
            etat = BAS;
          }
          break;
        case BAS:
          // code block
          yLogo = yLogo +3;
          if (iBcl8 > 50){
            etat = GAUCHE;
          }
          break;
        case GAUCHE:
          // code block
          xLogo = xLogo -2;
          if (iBcl8 > 90){
            etat = HAUT;
          }
          break;
        case HAUT:
          yLogo = yLogo - 3;
          // code block

          break;
        default:
          // code block
        }
        logoPrint(oledScreen.m_iDescIo,logo, xLogo, yLogo);


        
      }
      writeText(oledScreen.m_iDescIo, 20, 30, "Space Invider",sizeof("Space Invider"));
      sleep(1);


      writeText(oledScreen.m_iDescIo, 20, 40, "SAE",sizeof("SAE"));

      /*writeTextEvolve(oledScreen.m_iDescIo, 20, 30, "Space Invider",sizeof("Space invider"));
      sleep(1);


      writeTextEvolve(oledScreen.m_iDescIo, 50, 50, "SAE",sizeof("SAE"));*/

      

      //writeText(oledScreen.m_iDescIo, 20, 63, "Space Invider");
      
      /*SendCommand(oledScreen.m_iDescIo,&screenTimeout,sizeof(screenTimeout),100000);
      SendCommand(oledScreen.m_iDescIo,&screenSpeed,sizeof(screenTimeout),50000);*/

      //writeText(oledScreen.m_iDescIo, 30, 90, "SAE");
			

      usleep(5000000);

      //write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      write(oledScreen.m_iDescIo, & clearScreen, sizeof(clearScreen));
      usleep(50000);
      read(oledScreen.m_iDescIo, & response, 1);
      //usleep(50000);
      //read(oledScreen.m_iDescIo, & response, 1);
      fd_set readSet;
      
      int tempStart = 1; 
      
      
      char * receiveString[20];
      
      
      int receiveData;
      
      float valPoten = 0;
      char* valPotenTemp[6];
      int valButton = 0;
      
      int vitesse = 0;
      while (sigFlagStop != -1) {
      
      
      
      
      
      
      
      
      
      
      
      
      
      gettimeofday( & Time, & Balec);
          tpre = t;
          t = Time.tv_sec;
          ut = Time.tv_usec + (Time.tv_sec * 1000000);
          if (ut >= (utpre + 100000)) {

            utpre = ut;
            A4++;
            A1++;
            A5++;
            A6++;

          }

          if (tpre != t) {
            A3++;
            A2++;

          }

          if (A3 == 5) {
            A3 = 0;
            //fprintf(stdout,"ajout mob : \n");

            while ((TabMob[iBcl2].x) != -1) { // on cherche une place de libre
              iBcl2++;
            }
            int couleur = (rand() % 3) + 1;
            //fprintf(stdout, "couleur : %i\n", couleur);
            x = rand() % 6; // On souhaite une valeur entre 0 et 5 car on divise notre ecran en 6 collones de 21 pixels et la deux extremites 22 et 23 pixel. Les centres de ces valeurs sont 11,32,53,74,95,116
            x = xTable[x];
            TabMob[iBcl2].x = x; 
            TabMob[iBcl2].y = 10;
            TabMob[iBcl2].hp = 50 * couleur;
            //fprintf(stdout, "HP : %i\n", TabMob[iBcl2].hp);
            //fprintf(stdout,"PASSAGE ADD : %i  value %i  \n", iBcl2, x);
            //char mob[12] = {0xFF, 0xCC, 0x00, x, 0x00, 10, 0x00, 0x06, 0x84, 0x10};
            //memcpy(&(TabMob->model),&mob,sizeof(mob)); // a modifier pour avoir le x/y   
            TabMob[iBcl2].model[5] = TabMob[iBcl2].y;
            TabMob[iBcl2].model[3] = x;

            switch (couleur) {
            case 1:
              TabMob[iBcl2].model[8] = 0x07;
              TabMob[iBcl2].model[9] = 0xE0;
              break;
            case 2:
              TabMob[iBcl2].model[8] = 0xFF;
              TabMob[iBcl2].model[9] = 0xE0;
              break;
            case 3:
              TabMob[iBcl2].model[8] = 0xF8;
              TabMob[iBcl2].model[9] = 0x00;
              break;
            }

            iBcl2 = 0;
            enablemob = 1;
          }

          if (A2 == 3) {
            //fprintf(stdout,"qjout missile : \n");
            A2 = 0;
            while ((TabMissile[iBcl3].x) != -1) {
              iBcl3++;
            }
            //MasterShip.x =(rand()%100)+10;
            //fprintf(stdout,"PASSAGE ADD : %i  value %i  \n", iBcl3, MasterShip.x);
            TabMissile[iBcl3].x = MasterShip.x;
            TabMissile[iBcl3].y = 100;
            TabMissile[iBcl3].damage = 50;
            //char mis[12] = {0xFF, 0xCC, 0x00, MasterShip.x, 0x00, 0x75, 0x00, 0x06, 0x84, 0x10};
            //memcpy(&(TabMissile->model),&RondBas,sizeof(mis)); 
            TabMissile[iBcl3].model[3] = MasterShip.x;
            TabMissile[iBcl3].model[5] = TabMissile[iBcl3].y;
            iBcl3 = 0;
            enablemiss = 1;
          }
      
          // fprintf(stdout,"A5 : %i \n", A5);
          // fprintf(stdout,"A1 : %i \n", A1);

          if ((A5 == 1) || (A1 == 1)) {
            for (iBcl4 = 0; iBcl4 < 50; iBcl4++) {
              if (A1 == 1) {
                enablemiss = 1;
                //fprintf(stdout, "enablemiss: \n");
                if (iBcl4 < 20) {
                  if ((TabMissile[iBcl4].x) != -1) {
                    if (TabMissile[iBcl4].y <= 7) {
                      TabMissile[iBcl4].x = -1;
                      RondClear[3] = TabMissile[iBcl4].model[3];
                      RondClear[5] = TabMissile[iBcl4].model[5];
                      write(oledScreen.m_iDescIo, & RondClear, sizeof(RondClear));
                      //fprintf(stdout, "Missile Perdu : \n");

                    } else {
                      TabMissile[iBcl4].y = TabMissile[iBcl4].y - 1;
                      TabMissile[iBcl4].model[5] = TabMissile[iBcl4].y;
                      //fprintf(stdout,"Valeur X  : %i \n",TabMissile[iBcl4].model[3]);
                      //fprintf(stdout,"Avance val tableau %i : \n",iBcl4);
                    }
                  }
                }
              }

              if ((TabMob[iBcl4].x) != -1) {
                if (TabMob[iBcl4].y >= 121) {
                  TabMob[iBcl4].x = -1;
                  RondClear[3] = TabMob[iBcl4].model[3];
                  RondClear[5] = TabMob[iBcl4].model[5];
                  write(oledScreen.m_iDescIo, & RondClear, sizeof(RondClear));
                  //fprintf(stdout, "MOB PERDU : \n");
                } else {
                  if (A5 == 1) {
                    enablemob = 1;
                    TabMob[iBcl4].y = TabMob[iBcl4].y + 1;
                    TabMob[iBcl4].model[5] = TabMob[iBcl4].y;
                    //fprintf(stdout,"Avance val tableau %i : \n",TabMob[iBcl4].model[3]);
                    //fprintf(stdout,"Avance val tableau %i : \n",TabMob[iBcl4].model[5]);
                    
                  }
                  for (iBcl5 = 0; iBcl5 < 20; iBcl5++) {
                    if (((TabMissile[iBcl5].x) <= ((TabMob[iBcl4].x) + 11)) && (((TabMissile[iBcl5].x) >= ((TabMob[iBcl4].x) - 11)))) {
                      if ((TabMissile[iBcl5].y) < ((TabMob[iBcl4].y) + 10)) {

                        //fprintf(stdout, "Collision : %i \n", TabMob[iBcl4].hp - TabMissile[iBcl5].damage);
                        //fprintf(stdout, "Valeur Y missile   : %i \n", TabMissile[iBcl5].y);
                        //fprintf(stdout,"Valeur Y mob   : %i \n",TabMob[iBcl4].y );
                        if ((TabMob[iBcl4].hp - TabMissile[iBcl5].damage) <= 0) {
                          if (A5 == 1) {
                            TabMob[iBcl4].model[5] = TabMob[iBcl4].y - 1;
                          }
                          if (A1 == 1) {
                            TabMissile[iBcl5].model[5] = TabMissile[iBcl5].y + 1;
                          }

                          RondClear[3] = TabMissile[iBcl5].model[3];SendCommand(oledScreen.m_iDescIo, & BufferTableCircle, sizeof(BufferTableCircle),5000);
                          RondClear[5] = TabMissile[iBcl5].model[5];
                          //fprintf(stdout, "clear X : %i \n", RondClear[3]);
                          //fprintf(stdout, "clear Y : %i \n", RondClear[5]);
                          write(oledScreen.m_iDescIo, & RondClear, sizeof(RondClear));
                          usleep(6000);
                          RondClear[3] = TabMob[iBcl4].model[3]; // fonctionne pas la suppression pour le moment, il faut aussi regarder les +1 etc 
                          RondClear[5] = TabMob[iBcl4].model[5];
                          write(oledScreen.m_iDescIo, & RondClear, sizeof(RondClear)); // LE READ EST ENLEVER       
                          TabMob[iBcl4].x = -1;
                          TabMissile[iBcl5].x = -1;
                          usleep(6000);
                        } else {
                          if (A1 == 1) {
                            TabMissile[iBcl5].model[5] = TabMissile[iBcl5].y + 1;
                          }

                          RondClear[3] = TabMissile[iBcl5].model[3];
                          RondClear[5] = TabMissile[iBcl5].model[5];
                          write(oledScreen.m_iDescIo, & RondClear, sizeof(RondClear));
                          usleep(6000);
                          TabMob[iBcl4].hp = TabMob[iBcl4].hp - TabMissile[iBcl5].damage;
                           
                          if (TabMob[iBcl4].hp == 100) {
                            TabMob[iBcl4].model[8] = 0xFF;
                            TabMob[iBcl4].model[9] = 0xE0;                            
                          } else {
                            TabMob[iBcl4].model[8] = 0x07;
                            TabMob[iBcl4].model[9] = 0xE0;                           
                          }
                          TabMissile[iBcl5].x = -1;
                        }
                      }

                    } // mettre les ibcl a 0 again
                  }
                  iBcl5 = 0;
                }

              }
            }
            iBcl4 = 0;
            if (A5 == 1) {
              A5 = 0;
            } 
             if (A1 == 1) {
              A1 = 0;
            }

          }

          /*
                char blackRectangle[12] = {    0xFF,    0xCE,    0x00,    0x00,    0x00,    0x00,    0x00,    0x7F,    0x00,    0x7F,    0x00,    0x00  };
                 int c = 0;     
              write(oledScreen.m_iDescIo,&blackRectangle,sizeof(blackRectangle));         
               int iReceive = 0;   
                  int f = read(oledScreen.m_iDescIo,&iReceive,1);
                 usleep(50000);*/
          int iReceive = 0;
          int c = 0;

          if (enablemob == 1) { // A résoudre 
            enablemob = 0;
            for (int f = 0; f < 50; f++) {
              if ((TabMob[f].x) != -1) {
                Rond[3] = TabMob[f].model[3];
                Rond[5] = TabMob[f].model[5] - 1;

                write(oledScreen.m_iDescIo, & Rond, sizeof(Rond));

                read(oledScreen.m_iDescIo, & iReceive, 1);

                usleep(10000);

                //fprintf(stdout,"PASSAGE PRINT %i: \n",f);
                c = write(oledScreen.m_iDescIo, & TabMob[f].model, sizeof(TabMob[f].model));
                //fprintf(stdout," nombre d'octets envoyé  : %i\n",c);
                int b = read(oledScreen.m_iDescIo, & iReceive, 1);
                //fprintf(stdout," nombre d'octets lus  : %i \n",b); 
                //fprintf(stdout,"Char receive : %i\n",iReceive);

                usleep(10000);

              }

            }
          }
          if (enablemiss == 1) {
            enablemiss = 0;
            for (int g = 0; g < 20; g++) {

              if ((TabMissile[g].x) != -1) {
                Rond[3] = TabMissile[g].model[3];
                Rond[5] = TabMissile[g].model[5] + 1;

                write(oledScreen.m_iDescIo, & Rond, sizeof(Rond));

                read(oledScreen.m_iDescIo, & iReceive, 1);

                usleep(10000);

                //fprintf(stdout,"PASSAGE PRINT %i: \n",g);
                c = write(oledScreen.m_iDescIo, & TabMissile[g].model, sizeof(TabMissile[g].model));
                //fprintf(stdout," nombre d'octets envoyé  : %i\n",c); 
                int b = read(oledScreen.m_iDescIo, & iReceive, 1);
                //fprintf(stdout," nombre d'octets lus  : %i \n",b); 
                //fprintf(stdout,"Char receive : %i\n",iReceive);

                usleep(10000);

              }
            }

            //c = write(oledScreen.m_iDescIo,&RondBas,sizeof(RondBas));

            //  fprintf(stdout," nombre d'octets lus  : %i \n",b);   
            // fprintf(stdout," nombre d'octets envoyé  : %i\n",c);
            // fprintf(stdout,"Char receive : %i\n",iReceive);

            // fprintf(stdout,"Char tunning : %c\n",TabMob->model[1]);

          }
      
      
      
      
      
      
      

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
          iResult = read(a2iFdPipe[0], & receiveData, 4);
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
              //float valPoten = 0;
      //int valButton = 0;
        
        
        // decontatenate
        
        
        
        
        
        pourcent = atof(tempString);
        
      	valButton = receiveData/100000;
        valPoten = (receiveData-(valButton*100000)-100)/100.0;
        //printf("Bouton : %i\n",valButton);
        //printf("Poten : %f\n",valPoten);
        //printf("BOUTON : %i\n",receiveData);
        
				//printf("%'.2f\n",potenReceive);
        
        
        
        /*if((x>8) && (potenReceive<0)){
        	//printf("%'.2f\n",(2*potenReceive));
					x = x + (2*potenReceive);
        }
        else if((x< 120) && (potenReceive>0)){
        	//printf("%'.2f\n",(2*potenReceive));
					x = x + (2*potenReceive);
        }*/
        
				if(((xbis>8) && (valPoten<0))||((xbis< 120) && (valPoten>0))){
					vitesse = 2*valPoten;
					xbis = xbis + vitesse;
				}
		    
		    
		    
		    if (vitesse !=0){
		    
		    	
          for (int iBcl4 = 1;iBcl4<=abs(vitesse);iBcl4++){
          
          	
          	// vers la gauche
						if ((xbis>8) && (valPoten<0)){
							CircleBlack[3] = BufferTableCircle[3]+(-iBcl4+1);
							//printf("%i\n",iBcl4);
						}
						// vers la droite
						if ((xbis< 120) && (valPoten>0)){
							CircleBlack[3] = BufferTableCircle[3]-(iBcl4-1);
						}
            SendCommand(oledScreen.m_iDescIo, & CircleBlack, sizeof(CircleBlack),5000);
          }
				  // modifications on the table of char
			  	
			  	BufferTableCircle[3] = xbis;
				  //tmp = blackRectangle;
				  //SendCommand(oledScreen.m_iDescIo,tmp);
				  /*printf("Etat read : %i\n",returnVAlue);
				  printf("Reponse : %i\n",response);*/

				  // sending command to display main  character
				  SendCommand(oledScreen.m_iDescIo, & BufferTableCircle, sizeof(BufferTableCircle),5000);
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
