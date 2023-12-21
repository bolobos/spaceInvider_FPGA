#include "prjOled.h"

/*int main(int iArgCount, char* apstrArgValue[]){
   int iResult=0;
	int iFdPipeFifo;
	const char strTubeX[] = "/tmp/tubex";
	char strMessage[] = "Hello";
   
   iResult = open(strTubeX,O_WRONLY);
   if (iResult >= 0){
      iFdPipeFifo = iResult;
      iResult = write(iFdPipeFifo,strMessage,8);
      close(iFdPipeFifo);
   }
	
	return iResult;
}*/
