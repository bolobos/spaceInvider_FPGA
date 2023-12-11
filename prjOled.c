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

#include <stdio.h>   
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>   
#include <errno.h>   
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <poll.h>
#include <termios.h>

#include "prjOled.h"

sig_atomic_t  sigFlagStop=0;

void signal_handler(int iSignal);
void signal_handler(int iSignal){

switch (iSignal) {

   case SIGINT :
      sigFlagStop = -1;
      break;
   case SIGALRM :
      sigFlagStop = -1;
	default:
				break;
	}
}

struct dataMix16Separatebits{

   short int dataAlone8bitsLOW;
   short int dataAlone8bitsHIGH;
};

union buffer{

   struct dataMix16Separatebits commandStruct;
   unsigned short command;
};
   

void send16bits(int iFd, union buffer data){
      write(iFd, &data.commandStruct.dataAlone8bitsHIGH,sizeof(data.commandStruct.dataAlone8bitsHIGH));
      write(iFd, &data.commandStruct.dataAlone8bitsLOW,sizeof(data.commandStruct.dataAlone8bitsHIGH));
}




int main(int iArgCount, char* apstrArgValue[]){

	int iResult=0;
	int iVerif =0;
	
	char BufferTable[12] = {0xff,0xCF,0x00,0x0A,0x00,0x14,0x00,0x5A,0x00,0x3C,0xF8,0x11}  ;
	char BufferTableCircle[12] = {0xFF, 0xCC, 0x00, 0x2D, 0x00, 0x28, 0x00, 0x23, 0x84, 0x10}; 
	
	union buffer dataToReceive;

   union buffer data;
   memset(&dataToReceive,0,2);
   

	
	struct TDevice oledScreen = {
		.m_strPathDev = "/dev/ttyAL0",
		.m_iDescIo = -1,
	};
	
	struct termios termiosOled;
	

   oledScreen.m_iDescIo = open(oledScreen.m_strPathDev,(O_RDWR | O_NONBLOCK));
	
	signal(SIGINT,signal_handler);
	signal(SIGALRM,signal_handler);
	
	alarm(5);
	
	// get back current config
	iVerif = tcgetattr(oledScreen.m_iDescIo,&termiosOled);
	if (iVerif == 0){
	
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
      
      if (iVerif == 0){
         //fprintf(stdout,"C est good\n");
      }
      else{
         //fprintf(stdout,"JOSSELIN PÉTAUX\n");
      }
      
      // block descriptor file (pas sur)
      //int temp = (F_GETFL & 2047);
      
      
	   if(iVerif != -1){
	      

	      
	      /*data.command = 0x000B;
         send16bits(oledScreen.m_iDescIo,data);
         
	      data.command = 0x0138;
         send16bits(oledScreen.m_iDescIo,data);
         
         alarm(1);
         pause();
      read(oledScreen.m_iDescIo,&dataToReceive.command,sizeof(dataToReceive.command));
         fprintf(stdout,"Noyeux Joel : %d\n",dataToReceive.command);*/
         
         
         
        for(int i = 0; i < 12;i++){
                          
           write(oledScreen.m_iDescIo,&BufferTableCircle[i],sizeof(BufferTable[i]));
        }
        
        alarm(1);
        pause();
        read(oledScreen.m_iDescIo,&dataToReceive.command,sizeof(dataToReceive.command));
        fprintf(stdout,"Noyeux Joel : %d\n",dataToReceive.command);
        
        
	   }	   
	}
	
	
	
	close(oledScreen.m_iDescIo);
	return iResult;
}
	
	
	


