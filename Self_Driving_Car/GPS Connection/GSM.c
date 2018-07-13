/*/*
* GSM.c
*
* Created: 7/10/2018 10:06:04 PM
*  Author: hassa
*/
#include "/projects/AVR/TRacking Car/TRacking Car/GSM.h"
#include "/projects/AVR/EUSART/EUSART/EUSART.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#define F_CPU 16000000UL  //16MHz unsigned long 
char SIM300_Buffer[128];
int8_t SIM300_Command(const char command){
	EUSART_Write_Text(command);//send command to SIM300
	EUSART_Write(0x0D);//CR
	uint8_t len =strlen(command); //calculate of the size Command
	len++;
	
	uint16_t i=0;
	//wait for echo
	while (i<10*len){
		if(EUSART_Available()<len)
		{
			i++;
			_delay_ms(10);
			continue;
		}
		else{
			EUSART_Read_Text(SIM300_Buffer,len);
			return SIM300_OK ; //Response of The Command
		}
	}
	return SIM300_TIMEOUT;
}

int8_t	SIM300_Init(){
	EUART_Start(9600);
	SIM300_Command("AT");
	uint16_t i=0;
	while (i<10){
		if(EUSART_Available()<6){
			i++;
			_delay_ms(10);
			continue;
			}else{
			EUSART_Read_Text(SIM300_Buffer,6);
			return SIM300_CheckResponse(SIM300_Buffer,"OK",6);
		}
	}
	return SIM300_TIMEOUT ;
}
int8_t	SIM300_CheckResponse(const char *response,const char *check,uint8_t len){
	len =-2 ;
	if(response[0]!=0x0D|response[len+1]!=0x0A) //Start =0x0D ,End =0x0A
	return SIM300_INVALID_Respons;
	for(uint8_t i=2;i<len;i++){
		if(response[i]!=check[i-2])
		return SIM300_FIAL ;
	}
	return SIM300_OK ;
}
int8_t	SIM300_WaitForResponse(uint16_t timeout){
	uint8_t i=0;
	uint8_t n=0;
	while(1){
		while(EUSART_Available()==0 && n<timeout){
			n++;
			_delay_ms(1);
			}
			if(n==timeout)
			return 0;
			else {
				SIM300_Buffer[i]=EUSART_Read();
				if(SIM300_Buffer[i]==0x0D&&i!=0){
			EUSART_Flush();
			return i+1 ;
				}else{
					i++;
				}
			}
			
	}
}
int8_t	SIM300_GetNetStat(){
	SIM300_Command("AT+CREG?");//NETWORK REGISTRATION
	uint16_t i=0;
	while(i<10){
		if(EUSART_Available()<20){
			i++;
			_delay_ms(10);
			continue;
		}else{
			EUSART_Read_Text(SIM300_Buffer,20);
			if(SIM300_Buffer[11]=='1')
			return SIM300_NW_REGISTERED_HOME;
			else if(SIM300_Buffer[11]=='2')
			return SIM300_NW_SHARCHING ;
			else if(SIM300_Buffer[11]=='5')
			return SIM300_NW_REGISTED_ROAMING ;
			else 
			return SIM300_NW_ERROR ;
		}
		
	}
	return SIM300_TIMEOUT ;
}
int8_t	SIM300_IsSIMInserted(){
	EUSART_Flush();
	SIM300_Command("AT+CSMINS?");//SIM INSERTED STATUS REPORTING
	uint16_t i=0;
	while(i<30){
		if(EUSART_Available()<22){
			i++;
			_delay_ms(10);
			continue;
		}else{
			EUSART_Read_Text(SIM300_Buffer,22);
			if(SIM300_Buffer[13]=='1')
			return SIM300_SIM_PRESENT ;
			else 
			return SIM300_SIM_NOT_PRESENT ;
			
		}
		
	}
	return SIM300_TIMEOUT;
}
uint8_t SIM300_GetProviderName(char *Name){
	EUSART_Flush();
	SIM300_Command("AT+CSPN?"); //GET SERVICE PROVIDER NAME FORM SIM
	uint8_t len =SIM300_WaitForResponse(1000);
	if(len ==0)
	return SIM300_TIMEOUT;
	char *Start ,*End ;
	Start =strchr(SIM300_Buffer,"");
	Start++;
	End =strchr(Start,"");
	*End='\0';
	strcpy(Name,Start);
	return strlen(Name);
	
	
}
int8_t	SIM300_GetIMEI(char *EMEI){
	EUSART_Flush();
	SIM300_Command("AT+GSN"); //AT+GSN Request TA serial number identification(IMEI)
	uint8_t len =SIM300_WaitForResponse(1000);
	if(len==0)
	return SIM300_TIMEOUT ;
	SIM300_Buffer[len-1]='\0';
	strcpy(EMEI,SIM300_Buffer+2);
	return SIM300_OK;

}
int8_t	SIM300_GetManufacturer(char *man_id){
	EUSART_Flush();
	SIM300_Command("AT+GMI");//AT+GMI Request manufacture identification
	uint8_t len =SIM300_WaitForResponse(1000);
	if(len==0)
	return SIM300_TIMEOUT;
	SIM300_Buffer[len-1]='\0';
	strcpy(man_id,SIM300_Buffer+2);
	return SIM300_OK ;
}
int8_t	SIM300_GetModel(char *Model){
	EUSART_Flush();
	SIM300_Command("AT+GMM");// AT+GMM Request TA model identification
	uint8_t len =SIM300_WaitForResponse(1000);
	if(len==0)
	return SIM300_TIMEOUT ;
	SIM300_Buffer[len-1]='\0';
	strcpy(Model,SIM300_Buffer+2);
	return SIM300_OK ;
	}
int8_t SIM300_WaitForMsg(uint8_t *id){
	uint8_t len =SIM300_WaitForResponse(250);
	if(len==0)
	return SIM300_TIMEOUT ;
	SIM300_Buffer[len-1]='\0';
	if(strncasecmp(SIM300_Buffer+2,"+CMTI:",6)==0){
		char str_id[4];
		char *start ;
		start=strchr(SIM300_Buffer,',');
		start++;
		strcpy(str_id,start);
		*id=atoi(str_id);
		return SIM300_OK;
		}else{
			return SIM300_FIAL;
		}
}
uint8_t SIM300_ReadMsg(uint8_t i,char *msg){
		EUSART_Flush();
		char cmd[16];
		sprintf(cmd,"AT+CMGR=%d",i);//READ SMS MESSAGE
		SIM300_Command(cmd);
		uint8_t len = SIM300_WaitForResponse(1000);
		if(len==0)
			return SIM300_TIMEOUT ;
			SIM300_Buffer[len-1]='\0' ;
			if(strcasecmp(SIM300_Buffer,"+CMS ERROR: 517")==0){
				return SIM300_SIM_NOT_PRESENT ;
				
			}
			if(strcasecmp(SIM300_Buffer+2,"OK")==0){
				return SIM300_SIM_PRESENT ;
			}
			len =SIM300_WaitForResponse(1000);
			if(len==0)
			return SIM300_TIMEOUT ;
			SIM300_Buffer[len-1]='\0';
			strcpy(msg,SIM300_Buffer+1);
			return SIM300_OK ;
		
	}
uint8_t SIM300_SendMsg(const char *num ,const char *mag , uint8_t *msg_ref){
		EUSART_Flush();
		char cmd[25];
		sprintf(cmd,"AT+CMGS=%s",num);//Send SMS message
		cmd[8]=0x22;//"
		uint8_t n=strlen(cmd);
		cmd[n]=0x22;//"
		cmd[n+1]='\0';
		SIM300_Command(cmd);
		_delay_ms(100);
		EUSART_Write_Text(mag);
		EUSART_Write(0x1A);
		while(EUSART_Available()<(strlen(mag)+5));
		EUSART_Read_Text(SIM300_Buffer,strlen(mag)+5);
		uint8_t len =SIM300_WaitForResponse(6000);
		if(len==0)
		return SIM300_TIMEOUT ;
		SIM300_Buffer[len-1]='\0';
		if(strncasecmp(SIM300_Buffer+2,"CMGS:",5)==0){
			*msg_ref=atoi(SIM300_Buffer+8);
			EUSART_Flush();
			return SIM300_OK ;
			}else
			{
				EUSART_Flush();
				return SIM300_FIAL ;
			}
			
		
		
	}
	int8_t   SIM300DeleteMsg(uint8_t i){
		EUSART_Flush();
		char cmd[16];
		sprintf(cmd,"AT+CMGD=%d",i);
		SIM300_Command(cmd);
		
		uint8_t len=SIM300_WaitForResponse(1000);
		   if(len==0)
		   return SIM300_TIMEOUT;

		   SIM300_Buffer[len-1]='\0';

		   //Check if the response is OK
		   if(strcasecmp(SIM300_Buffer+2,"OK")==0)
		   return SIM300_OK;
		   else
		   return SIM300_FIAL;	
	}
