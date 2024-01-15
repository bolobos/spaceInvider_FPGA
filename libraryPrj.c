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
   usleep(7000);

   returnVAlue = read(fd, & responseOled, 1);

   if((nbrOctSent == dataSize) && (returnVAlue != -1)){
      return 1;
   }
   else{
      return -1;
   }
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

