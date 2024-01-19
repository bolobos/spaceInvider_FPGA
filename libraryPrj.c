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

int SendCommand(int fd,char *data,size_t dataSize,int time){ 
   int nbrOctSent = 0;
   int returnVAlue = 0;
   char responseOled[40];

   nbrOctSent = write(fd, data, dataSize);

   // adapted to 115200 baud
   usleep(time);

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



void writeTextEvolve(int fd, int x, int y, char * text, size_t size) {

   char response[5];
  char responseOled[5];
  memset(responseOled,0,sizeof(responseOled));
  int modulo;
  int divide;
  moveOrigin[3] = x;
  moveOrigin[5] = y;
  write(fd, & moveOrigin, sizeof(moveOrigin));
  usleep(7000);
  read(fd, & response, 1);
  divide = size / 5;
  modulo = size % 5;
  int iBcl4 = 0;
  printf("SizeInit = %i\n",size);
  if (size > 5){
   divide = size / 5;
   size = 5;
  }
  else{
   divide = 0;
  }

 
  
  for(int iBcl1 = 0; iBcl1 <= divide; iBcl1++){
      if(iBcl1 == divide){
         size = modulo;
      }
      for(int iBcl2 = 0; iBcl2 < size; iBcl2++){
         printChar[2] = 0;
         printChar[3] = text[(iBcl1*5)+iBcl2];
         printf("Char = %c\n",text[(iBcl1*5)+iBcl2]);
         printf("(iBcl1*5)+iBcl2 = %i\n",(iBcl1*5)+iBcl2);
         for(int iBcl3 = 0; iBcl3 < 4; iBcl3++){
               printf("responseOled[%i] = printChar[%i]\n",iBcl4,iBcl3);
               iBcl4++;
              responseOled[iBcl4] = printChar[iBcl3];
              //usleep(500000);
         }
      }
      printf("WRITE!!!!\n");
      write(fd, & responseOled, sizeof(responseOled));
      memset(responseOled,0,sizeof(responseOled));
      usleep(7000);
      read(fd, & responseOled, 1);
      iBcl4 = 0;
  }
  

}


void writeText(int fd, int x, int y, char * text,size_t size) {

  char responseOled[size+2];
  char response[5];
  responseOled[0] = 0x00;
  responseOled[1] = 0x06;
  moveOrigin[3] = x;
  moveOrigin[5] = y;
  write(fd, & moveOrigin, sizeof(moveOrigin));
  usleep(7000);
  read(fd, & response, 1);

  for (int iBcl = 0; iBcl < size; iBcl++) {
    //fprintf(stdout, "iBcl %i\n",iBcl);
    responseOled[2+iBcl] = text[iBcl];
    
  }
  responseOled[size+1] = 0x00;
  write(fd, & responseOled, sizeof(responseOled));
   usleep(7000);
   read(fd, & response, 1);
}

void printTriangle(int fd, int x, int y, int taille) {
   
   Triangle[3] = x;
   Triangle[5] = y;
   Triangle[7] = x+taille;
   Triangle[9] = y-taille;
   Triangle[11] = x-taille;
   Triangle[13] = y-taille;
   
   SendCommand(fd, & Triangle, sizeof(Triangle),5000);
}

void logoPrint(int fd,char table[8][11], int x, int y){

   Pixel[3] = x;
   Pixel[5] = y;
   for(int iBcl=1;iBcl<=8;iBcl++){
      Pixel[5] = Pixel[5]+1;
      for(int iBcl2=1;iBcl2<=11;iBcl2++){
         Pixel[3] = Pixel[3]+1;
         if(table[iBcl][iBcl2] == 1){
            SendCommand(fd, & Pixel, sizeof(Pixel),3000);
         }         
      }
      Pixel[3] = x;
   }
}

/*void readButtons(int fd, int x, int y, int taille) {
   
   
}*/



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

