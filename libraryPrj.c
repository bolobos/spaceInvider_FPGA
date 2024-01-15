#include "prjOled.h"



/*void InitComp(const char* fileDescriptor,termios *pon){
	// get back current config
	iVerif = tcgetattr(oledScreen.m_iDescIo,&termiosOled);
	
   // manual config
   // put default setup (merguez)
   cfmakeraw(&termiosOled);
   
   // ignore signals of terminals
   termiosOled.c_iflag &= ~CLOCAL;
         
   // set profile	
   iVerif = tcsetattr(oledScreen.m_iDescIo,TCSANOW,&termiosOled);
   
   // block mode
   iVerif  = fcntl(oledScreen.m_iDescIo,F_SETFL,~O_NONBLOCK);      
   
   // set parameter
   termiosOled.c_cflag &= ~(CSTOPB);      
   
   // set profile	a second time
   iVerif = tcsetattr(oledScreen.m_iDescIo,TCSANOW,&termiosOled);
   
   cfsetispeed(&termiosOled,B9600);
   cfsetospeed(&termiosOled,B9600);
}*/

size_t DateLocale(const char strFormatDate[], char strDate[],size_t sztTaillestrDate){

   time_t TimeNow  ;

   time_t* ptrTime = &TimeNow ;
   struct tm * ptrTime2;

   TimeNow = time(ptrTime); // null);
   ptrTime2  = localtime( &TimeNow);///ptrTime) ;
   strftime(strDate, sztTaillestrDate, strFormatDate, ptrTime2);
   

   return 0;
};

int SendCommand(int fd,char *data,size_t dataSize){ 
   int nbrOctSent = 0;
   int returnVAlue = 0;
   char responseOled[40];

   nbrOctSent = write(fd, data, dataSize);

   // adapted to 115200 baud
   usleep(10000);

   returnVAlue = read(fd, & responseOled, 1);

   if((nbrOctSent == dataSize) && (returnVAlue != -1)){
      return 1;
   }
   else{
      return -1;
   }
}



int ReadAdcIA (int iDescAdcIA, char strValue[ ], int iStrSize) {

	/* reservation et initialisation */
	long 		iResult=0;
	size_t 	sztNbrBytesRead=0;
	
	sztNbrBytesRead=0;
	while ((sztNbrBytesRead < iStrSize) && (iResult>=0)) {
		iResult=read(iDescAdcIA, &strValue[sztNbrBytesRead], (iStrSize-sztNbrBytesRead));
		if (iResult >= 0) {
			sztNbrBytesRead = sztNbrBytesRead + iResult;
		}
		else {
			//perror("read AdcIA:");
		}
	}
	if (sztNbrBytesRead==iStrSize) {
		iResult= sztNbrBytesRead;
	}
	return (iResult);
}



void writeText(int fd, int x, int y, char * text) {

  char responseOled[40];
  moveOrigin[3] = x;
  moveOrigin[5] = y;
  write(fd, & moveOrigin, sizeof(moveOrigin));
  usleep(7000);
  read(fd, & responseOled, 1);

  for (int iBcl = 0; iBcl < sizeof(text); iBcl++) {
    //fprintf(stdout, "iBcl %i\n",iBcl);
    printChar[2] = 0;
    printChar[3] = text[iBcl];
    write(fd, & printChar, sizeof(printChar));
    usleep(7000);
    read(fd, & responseOled, 1);
  }
}

void printTriangle(int fd, int x, int y, int taille) {
   
   Triangle[3] = x;
   Triangle[5] = y;
   Triangle[7] = x+taille;
   Triangle[9] = y+taille;
   Triangle[11] = x-taille;
   Triangle[13] = y+taille;
   
   SendCommand(fd, & Triangle, sizeof(Triangle));
}

/*void InitOled(int fd, termios *termiosOled){
 // ignore signals of terminals
 termiosOled.c_iflag &= ~CLOCAL;

 // set profile	
 iVerif = tcsetattr(fd, TCSANOW, & termiosOled);

 // block mode
 iVerif = fcntl(fd, F_SETFL, ~O_NONBLOCK);

 // manual config
 // put default setup (merguez)
 cfmakeraw( & termiosOled);

 // set parameter
 termiosOled.c_cflag &= ~(CSTOPB);

 /*cfsetispeed( & termiosOled, B9600);
 cfsetospeed( & termiosOled, B9600);
 cfsetispeed( & termiosOled, B115200);
 cfsetospeed( & termiosOled, B9600);
 iVerif = tcsetattr(fd, TCSANOW, & termiosOled);
 usleep(110000);
 nbrOctSent = write(fd, & setBaudRate, sizeof(setBaudRate));
 usleep(110000);
 //returnVAlue = read(oledScreen.m_iDescIo, & response, 1);
 //fprintf(stdout, "Return value : %i\n", returnVAlue);
 cfsetospeed( & termiosOled, B115200);
 usleep(110000);
 // set profile	a second time
 iVerif = tcsetattr(fd, TCSANOW, & termiosOled);
 usleep(110000);
}*/

