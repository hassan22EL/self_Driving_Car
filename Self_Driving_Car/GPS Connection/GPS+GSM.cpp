/*
 * GPS_GSM.cpp
 *
 * Created: 7/20/2018 10:13:17 PM
 *  Author: hassa
 */ 
/*
* GPS_GMS.cpp
*
* Created: 7/18/2018 5:30:56 PM
*  Author: Hassan Elsaied mohammed mohammed
*/
#include "SIM808.h"
#include "GPS+GSM.h"
#include "/projects/AVR/EUSART/EUSART/EUSART.h"
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
/*General Packet Radio Service GPRS*/
GPS_GSM_SIM808* GPS_GSM_SIM808 :: inst;
char Recevied_Stack_Index =0 ;
char Recevied_Stack[130];
const char *des="$GPRMC" ;
//char *receivedStack="$GPRMC,165445.000,A,3110.8635,N,12133.4627,E,0.58,70.26,220916,,,A*57";
//GPS_GSM_SIM808:GPS_GSM_SIM808(uint8_t Tx ,uint8_t Tx ,uint8_t BaudRate=9600);
bool GPS_GSM_SIM808::INIT(void){
	if(!SIM808_check_Wait_Command("AT\r\n" , "OK\r\n" ,Command)){
		return false ;
	}
	if(!SIM808_check_Wait_Command("AT+CFUN=1\r\n","OK\r\n ",Command)){
		//ME has entered full functionality mod
		return false ;
	}
	if(!GPS_GSM_SIM808::check_SIMS_Status()){
		return false ;
		
	}
	return true ;
}
bool GPS_GSM_SIM808::check_power_up(void){
	return SIM808_check_Wait_Command("AT\r\n","OK\r\n",Command);
}
void GPS_GSM_SIM808 ::  Power_UP_Down(uint8_t pin){
	pin =0 ;
	_delay_ms(1000);
	pin=1;
	_delay_ms(2000);
	pin=0;
	_delay_ms(3000);
}
void  GPS_GSM_SIM808::  power_Rest(uint8_t pin){
	pin =0 ;
	_delay_ms(1000);
	pin=1;
	_delay_ms(3000);
}
bool GPS_GSM_SIM808 :: check_SIMS_Status(void){
	char gprsbuffer[32];
	int count=0;
	SIM808_Clean_Buffer(gprsbuffer,32);
	while (count <3)
	{
		SIM808_Send_Command("AT+CPIN?\r\n"); //CPIN this command check status Ready and Not Ready
		SIM808_Read_Buffer(gprsbuffer,32);
		if(NULL !=strstr(gprsbuffer,"+CPIN : READY")){
			break ;
		}
		
		count++;
		_delay_ms(300);
	}
	if(count==3){
		return false ;
	}
	return true ;
}
bool GPS_GSM_SIM808 ::  Send_SMS(char *Number ,char *Data){
	/*CMGf command :	Set SMS system into text mode, as
	opposed to PDU mode.
	*/
	if(!SIM808_check_Wait_Command("AT+CMGF=1\r\n" ,"OK\r\n",Command)){
		return false ;
	}
	_delay_ms(500);
	sim808_flush_serial();
	SIM808_Send_Command("AT+CMGF=\"");
	SIM808_Send_Command(Number);
	if(!SIM808_check_Wait_Command("\"\r\n",">",Command)){
		return false ;
	}
	_delay_ms(1000);
	SIM808_Send_Command(Data);
	_delay_ms(500);
	SIM808_Send_End_Mark();
	return SIM808_Wait_For_Respons("oK\r\n",Command);
}
char GPS_GSM_SIM808 ::  IS_SMS_unread(){
	char gprsbuffer[48];
	char *s ;
	SIM808_check_Wait_Command("AT+CMGF=1\r\n","OK\r\n",Command);
	_delay_ms(1000);
	//Read SMS message that has just arrived.
	//	Note: the number should be the same as
	//that given in the +CMTI notification
	//List of all UNREAD SMS and DON'T change the SMS UNREAD STATUS
	SIM808_Send_Command(("AT+CMGL=\"REC UNREAD\",1\r\n"));
	/*If you want to change SMS status to READ you will need to send:
	AT+CMGL=\"REC UNREAD\"\r\n
	This command will list all UNREAD SMS and change all of them to READ
	
	If there is not SMS, response is (30 chars)
	AT+CMGL="REC UNREAD",1  --> 22 + 2
	--> 2
	OK                      --> 2 + 2
	If there is SMS, response is like (>64 chars)
	AT+CMGL="REC UNREAD",1
	+CMGL: 9,"REC UNREAD","XXXXXXXXX","","14/10/16,21:40:08+08"
	Here SMS text.
	OK
	
	or
	AT+CMGL="REC UNREAD",1
	+CMGL: 9,"REC UNREAD","XXXXXXXXX","","14/10/16,21:40:08+08"
	Here SMS text.
	+CMGL: 10,"REC UNREAD","YYYYYYYYY","","14/10/16,21:40:08+08"
	Here second SMS
	OK
	*/
	SIM808_Clean_Buffer(gprsbuffer,31);
	SIM808_Read_Buffer(gprsbuffer,30);
	if(NULL!=(s=strstr(gprsbuffer,"OK"))){
		_delay_ms(50);
		return 0 ;
	}
	else {
		//More buffer to read
		//We are going to flush serial data until OK is recieved
		SIM808_Wait_For_Respons("OK\r\n", Command);
		//sim808_flush_serial();
		//We have to call command again
		SIM808_Send_Command("AT+CMGL=\"REC UNREAD\",1\r\n");
		SIM808_Clean_Buffer(gprsbuffer,48);
		SIM808_Read_Buffer(gprsbuffer,47);
		//Serial.print("Buffer isSMSunread 2: ");Serial.println(gprsBuffer);
		if(NULL != ( s = strstr(gprsbuffer,"+CMGL:"))) {
			//There is at least one UNREAD SMS, get index/position
			s = strstr(gprsbuffer,":");
			if (s != NULL) {
				//We are going to flush serial data until OK is recieved
				SIM808_Wait_For_Respons("OK\r\n", Command);
				return atoi(s+1);
			}
			} else {
			return -1;

		}
	}
	return -1;
}

bool GPS_GSM_SIM808 ::  Read_SMS(int messageIndex ,char * message ,int length ,char *phone ,char *DateTime ){
	/* Response is like:
	AT+CMGR=2
	
	+CMGR: "REC READ","XXXXXXXXXXX","","14/10/09,17:30:17+08"
	SMS text here
	
	So we need (more or lees), 80 chars plus expected message length in buffer. CAUTION FREE MEMORY
	*/
	int i=0;
	char gprsbuffer[80+length];
	char num[16];
	char *p,*p2,*s ;
	SIM808_check_Wait_Command("AT+CMGF=1\r\n","OK\r\n",Command);
	_delay_ms(1000);
	SIM808_Send_Command("AT+CMGR=");
	itoa(messageIndex ,num,10);
	SIM808_Send_Command(num);
	SIM808_Send_Command("\r\n");
	SIM808_Clean_Buffer(gprsbuffer,sizeof(gprsbuffer));
	SIM808_Read_Buffer(gprsbuffer ,sizeof(gprsbuffer));
	if(NULL !=(s=strstr(gprsbuffer,"+CMGR:"))){
		p=strstr(s,",");// Extract phone number string
		p2=p+2 ;// //We are in the first phone number character
		p=strstr((char *)p2,"\"");
		if(NULL !=p){
			i=0;
			while(p2<p){
				phone[i++]=*(p2++);
				
			}
			phone[i]='\0';
			// Extract date time string
		}
		p=strstr((char *)p2,",");
		p2=p+1 ;
		p=strstr((char *)p2 ,",");
		p2=p+2;
		p=strstr((char *)p2 ,"\"");
		if(NULL !=p){
			i=0;
			while(p2<p){
				DateTime[i++]=*(p2++);
			}
			DateTime[i]='\0';
		}
		if(NULL != (s=strstr(s,"\r\n"))){
			i=0;
			p=s+2 ;
			while((*p != '\r')&& (i<length-1)){
				message[i++]=*(p++);
			}
			message[i]='\0';
		}
		return true ;
		
	}
	return false ;
}
bool GPS_GSM_SIM808 ::  Read_SMS(int index,char *Buffer ,int length){
	int i=0 ;
	char gprsbuffer[100];
	char num[4];
	char *p,*s ;
	SIM808_check_Wait_Command("AT+CMGF=1\r\n","OK\r\n",Command);
	_delay_ms(1000);
	SIM808_Send_Command("AT+CMGR=");
	itoa(index,num,10);
	SIM808_Send_Command("\r\n");
	SIM808_Clean_Buffer(gprsbuffer,sizeof(gprsbuffer));
	SIM808_Read_Buffer(gprsbuffer,sizeof(gprsbuffer));
	if(NULL!=(s=strstr(gprsbuffer ,"+CMGR:"))){
		if(NULL !=(s=strstr(s,"\r\n"))){
			p=p+2 ;
			while((*p!='r')&&(i<length -1)){
				Buffer[i++]=*(p++);
			}
			Buffer[i]='\0';
			return true ;
		}
		
	}
	return false ;
}
bool GPS_GSM_SIM808:: Delete_SMS(int index){
	char num[4];
	SIM808_Send_Command("AT+CMGD=");
	itoa(index,num,10);
	SIM808_Send_Command(num);
	return SIM808_check_Wait_Command("\r","OK\r\n",Command);
}

bool GPS_GSM_SIM808 ::  Call_UP(char *Number){
	//Connected Line Identification Presentation COLP
	if(!SIM808_check_Wait_Command("AT+COLP=1\r\n","OK\r\n",Command)){
		return false ;
	}
	_delay_ms(1000);
	SIM808_Send_Command("ATD");
	SIM808_Send_Command(Number);
	SIM808_Send_Command(";\r\n");
	return true ;
}
bool GPS_GSM_SIM808 ::  Answer(void){
	SIM808_Send_Command("ATA\r\n");
	/*Make a voice call using the last number
	facility. The initial call is established and
	then cancelled. The second call is made
	using the previous dial string */
}
bool GPS_GSM_SIM808 ::  hangup(void){
	return SIM808_check_Wait_Command("ATH\r\n","OK\r\n",Command); //Hang up a call
}

bool GPS_GSM_SIM808 ::  Disable_CLTP_pring(void){
	return SIM808_check_Wait_Command("AT+CLIP=0\r\n","OK\r\n",Command);
}
bool GPS_GSM_SIM808 ::  get_subscribler_Number(char *Number){
	//AT+CNUM								--> 7 + CR = 8
	//+CNUM: "","+628157933874",145,7,4		--> CRLF + 45 + CRLF = 49
	//										-->
	//OK									--> CRLF + 2 + CRLF = 6
	short i =0 ;
	char gprsbuffer[65];
	char *p ,*s ;
	sim808_flush_serial();
	SIM808_Send_Command("AT+CNUM\r\n");
	SIM808_Clean_Buffer(gprsbuffer,65);
	SIM808_Read_Buffer(gprsbuffer ,65);
	if(NULL !=(s=strstr(gprsbuffer ,"+CNUM:"))){
		s=strstr((char *)s ,",");
		s=s+2;
		p=strstr((char *)s,"\"");
		if(NULL!=s){
			i=0;
			while (s<p)
			{
				Number[i++]=*(s++);
			}
			Number[i]='\0';
		}
		return true;
	}
	return false ;
}
bool GPS_GSM_SIM808 ::  IS_Call_Active(char *Number){
	
	char gprsbuffer[46];
	char *p,*s ;
	int i=0 ;
	SIM808_Send_Command("AT+CPAS\r\n");  // CPAS :Phone Activity Status
	/*Result code:
	Response
	TA returns the activity status of ME.
	+CPAS: <pas>
	0: Ready (MT allows commands from TA/TE)
	2 :Unknown (MT is not guaranteed to respond to tructions)
	3 : Ringing (MT is ready for commands from TA/TE, but theringer is active)
	4 :Call in progress (MT is ready for commands from TA/TE,a call is in progress)

	
	AT+CPAS   --> 7 + 2 = 9 chars
	--> 2 char
	+CPAS: 3  --> 8 + 2 = 10 chars
	--> 2 char
	OK        --> 2 + 2 = 4 chars
	
	AT+CPAS
	
	+CPAS: 0
	
	OK
	*/
	
	/*Result Code
	
	*/
	SIM808_Clean_Buffer(gprsbuffer,29);
	SIM808_Read_Buffer(gprsbuffer,27);
	SIM808_Wait_For_Respons("OK\r\n",Command);
	if(NULL !=(s=strstr(gprsbuffer,"+CPAS:"))){
		s=s+7;
		if(*s !='0'){
			SIM808_Send_Command("AT+CLCC\r\n");
			/*Command. A call’s ID is
			required to switch between held and
			active calls. Held calls are not
			automatically resumed when all other
			calls are terminated
			+CLCC:1,0,0,0,0,"62
			418148",129,""
			+CLCC:2,1,1,0,0, "139
			18186089",129, ""
			OK
			*/
			SIM808_Clean_Buffer(gprsbuffer,46);
			SIM808_Read_Buffer(gprsbuffer,45);
			if(NULL !=(s=strstr(gprsbuffer,"+CLCC:"))){
				s=strstr((char *)s ,"\"");
				s=s+1;
				p=strstr((char *)s,"\"");
				if(NULL!=s){
					i=0;
					while(s<p){
						Number[i++]=*(s++);
					}
					Number[i]='\0';
				}
				return SIM808_Wait_For_Respons("OK\r\n",Command);
			}
			
		}
	}
	return false ;
}
bool GPS_GSM_SIM808 ::  get_Date_Time(char *Buffer){
	//If it doesn't work may be for two reasons:
	//		1. Your carrier doesn't give that information
	//		2. You have to configurate the SIM808 IC.
	//			- First with SIM808_Serial_Debug example try this AT command: AT+CLTS?
	//			- If response is 0, then it is disabled.
	//			- Enable it by: AT+CLTS=1
	//			- Now you have to save this config to EEPROM memory of SIM808 IC by: AT&W
	//			- Now, you have to power down and power up again the SIM808
	//			- Try now again: AT+CCLK?
	//			- It should work
	
	//AT+CCLK?						--> 8 + CR = 9
	//+CCLK: "14/11/13,21:14:41+04"	--> CRLF + 29+ CRLF = 33
	//
	//OK							--> CRLF + 2 + CRLF =  6
	short i = 0;
	char gprsBuffer[50];
	char *p,*s;
	sim808_flush_serial();
	SIM808_Send_Command("AT+CCLK?\r");
	SIM808_Clean_Buffer(gprsBuffer,50);
	SIM808_Read_Buffer(gprsBuffer,50,Time_OUT);
	if(NULL != ( s = strstr(gprsBuffer,"+CCLK:"))) {
		s = strstr((char *)(s),"\"");
		s = s + 1;  //We are in the first phone number character
		p = strstr((char *)(s),"\""); //p is last character """
		if (NULL != s) {
			i = 0;
			while (s < p) {
				Buffer[i++] = *(s++);
			}
			Buffer[i] = '\0';
		}
		return true;
	}
	return false;
}
bool GPS_GSM_SIM808 ::  get_Signal_Strength(int *buffer){
	//AT+CSQ	(Signal quality report)					                                        --> 6 + CR = 10
	//+CSQ: <rssi>,<ber>		+CSQ: (list of supported <rssi>s),(list of supported <ber>s)	--> CRLF + 5 + CRLF = 9
	//OK							                                                            --> CRLF + 2 + CRLF =
	short i =0 ;
	char gprsbuffer[26] ;
	char *p ,*s ;
	char buffers[4];
	sim808_flush_serial();
	SIM808_Send_Command("AT+CSQ\r");
	SIM808_Clean_Buffer(gprsbuffer,26);
	SIM808_Read_Buffer(gprsbuffer,26,Time_OUT);
	if(NULL != (s=strstr(gprsbuffer , "+CSQ:"))){
		s=strstr((char *)s ," ");
		s=s+1;
		p=strstr((char *)s , ",");
		if(NULL !=s){
			i=0;
			while (s<p)
			{
				buffers[i++]=*(s++);
				
			}
			buffers[i]='\0';
		}
		*buffer =atoi(buffers);
		return true ;
	}
	
	return false ;
}
bool GPS_GSM_SIM808 :: Send_USSD__Synchronous(char *UesdCommand ,char *Result_Code ,char *Response){
	//AT+CUSD=1,"{command}"(Unstructured Supplementary Service Data)
	//OK
	//
	//+CUSD:1,"{response}",{int}
	short i = 0;
	char gprsBuffer[200];
	char *p,*s;
	SIM808_Clean_Buffer(Response, sizeof(Response));
	
	sim808_flush_serial();
	SIM808_Send_Command("AT+CUSD=1,\"");
	SIM808_Send_Command(UesdCommand);
	SIM808_Send_Command("\"\r");
	if(!SIM808_Wait_For_Respons("OK\r\n", Command))
	return false;
	SIM808_Clean_Buffer(gprsBuffer,200);
	SIM808_Read_Buffer(gprsBuffer,200,Time_OUT);
	if(NULL != ( s = strstr(gprsBuffer,"+CUSD: "))) {
		*Result_Code = *(s+7);
		Result_Code[1] = '\0';
		if(!('0' <= *Result_Code && *Result_Code <= '2'))
		return false;
		s = strstr(s,"\"");
		s = s + 1;  //We are in the first phone number character
		p = strstr(s,"\""); //p is last character """
		if (NULL != s) {
			i = 0;
			while (s < p) {
				Response[i++] = *(s++);
			}
			Response[i] = '\0';
		}
		return true;
	}
	return false;
}

bool GPS_GSM_SIM808 :: cancel_USSD_Synchronous(void){
	return SIM808_check_Wait_Command("AT+CUSD=2\r\n","OK\r\n",Command);
}

bool GPS_GSM_SIM808 :: join(const char *APN ,const char *UserName ,const char *Password){
	short i =0 ;
	char *p ,*s ;
	char IP_Addr[32];
	//CSTT	Start task and set APN, user name, password
	SIM808_Send_Command("AT+CSTT=\"");
	if(APN){
		SIM808_Send_Command(APN);
	}
	SIM808_Send_Command("\",\"");
	if(UserName){
		SIM808_Send_Command(UserName);
	}
	SIM808_Send_Command("\",\"");
	if(Password){
		SIM808_Send_Command(Password);

	}
	SIM808_check_Wait_Command("\"\r\n","OK\r\n",Command);
	//Get local IP address
	SIM808_Send_Command("AT+CIFR\r\n");
	SIM808_Clean_Buffer(IP_Addr,32);
	SIM808_Read_Buffer(IP_Addr,32);
	//Response:
	//AT+CIFSR\r\n       -->  8 + 2
	//\r\n				 -->  0 + 2
	//10.160.57.120\r\n  --> 15 + 2 (max)   : TOTAL: 29
	//Response error:
	//AT+CIFSR\r\n
	//\r\n
	//ERROR\r\n
	if(NULL !=strstr(IP_Addr ,"ERROR")){
		return false ;
	}
	s=IP_Addr+11;
	p=strstr((char *)s ,"\r\n");

	if(NULL !=s){
		i=0 ;
		while(s<p){
			ip_string[i++]=*(s++);
		}
		ip_string[i]='\0';
	}
	_ip=str_to_ip(ip_string);
	if(_ip !=0){
		return true ;
	}
	return false ;
}
void GPS_GSM_SIM808 :: Disconnect(void){
	SIM808_Send_Command("AT+CIPSHUT\r\n");//Deactivate GPRS PDP Context .....
}
bool GPS_GSM_SIM808 :: connect(protocol pt1 ,const char *Host,int port ,int time_out ,int charTimeout){
	char num[4];
	char reps[96];
	// AT+CIPSTART Start Up TCP or UDP Connection...
	/*
	Response
	1) If AT+CIPMUX=0
	+CIPSTART: (list of supported <mode>),(<IP address>),(<port>)
	+CIPSTART: (list of supported <mode>),(<domain name>),(<port>)
	OK
	2) If AT+CIPMUX=1
	+CIPSTART: (list of supported <n>),(list of supported <mode>),(<IP
	address>),(<port>)
	+CIPSTART: (list of supported <n>),(list of supported <mode>),(<domain
	AT+CIPSTART<mode>,<IPaddress>,<port>
	*/
	if(pt1==TCP){
		SIM808_Send_Command("AT+CIPSTART=\"TCP\",\""); //Mode
		SIM808_Send_Command(Host); //IP Address
		SIM808_Send_Command("\","); //","
		itoa(port ,num,10) ;
		SIM808_Send_Command(num);  //port
		SIM808_Send_Command("\r\n");
		}else if(pt1==UDP){
		SIM808_Send_Command("AT+CIPSTART=\"UDP\",\""); //Mode
		SIM808_Send_Command(Host); //IP Address
		SIM808_Send_Command("\","); //","
		itoa(port ,num,10) ;
		SIM808_Send_Command(num);  //port
		SIM808_Send_Command("\r\n");
		}else {
		return false ;
	}
	SIM808_Read_Buffer(reps ,96,time_out ,charTimeout);
	if(NULL !=strstr(reps ,"CONNECT")){
		return true ;
	}
	return false ;
}
bool GPS_GSM_SIM808 :: IS_Connected(void){
	char resp[96];
	SIM808_Send_Command("AT+CIPSTATUS\r\n");
	SIM808_Read_Buffer(resp ,sizeof(resp) ,Time_OUT);
	if(NULL !=strstr(resp,"CONNECTED")){
		//+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CONNECTED"
		return true ;
		}else{
		//+CIPSTATUS: 1,0,"TCP","216.52.233.120","80","CLOSED"
		//+CIPSTATUS: 0,,"","","","INITIAL"
		return false ;
	}
}
bool GPS_GSM_SIM808 :: Close(){
	if(!IS_Connected()){
		return true ;
	}
	return SIM808_check_Wait_Command("AT+CIPCLOSE\r\n","CLOSE OK \r\n",Command);
}
bool GPS_GSM_SIM808 :: Readable(void){
	return 	SIM808_check_Readable();
	
}
bool GPS_GSM_SIM808 :: wait_Readable(int wait_Time){
	return SIM808_Wait_Readable(wait_Time);
}
bool GPS_GSM_SIM808:: wait_Writable(int req_size){
	return req_size+1;
}
int  GPS_GSM_SIM808 :: Send(const char *String ,int length){
	//AT+CIPSEND Send data through TCP or UDP connection
	//AT+CIPSEND=<length>
	char num[4];
	if(length >0){
		SIM808_Send_Command("AT+CIPSEND=");
		itoa(length,num,10);
		SIM808_Send_Command(num);
		if(!SIM808_check_Wait_Command("\r\n",">",Command)) {
			//if(!sim808_check_with_cmd(cmd,">",CMD)) {
			return 0;
		}
		/*if(0 != sim808_check_with_cmd(str,"SEND OK\r\n", DEFAULT_TIMEOUT * 10 ,DATA)) {
		return 0;
		}*/
		_delay_ms(500);
		SIM808_Send_Command(String);
		_delay_ms(500);
		SIM808_Send_End_Mark();
		if(!SIM808_Wait_For_Respons("SEND OK\r\n", Data, Time_OUT * 10, INTERCHAR_TIMEOUT * 10)) {
			return 0;
		}
	}
	return length;
}

int  GPS_GSM_SIM808 :: Receive(char *Buffer ,int length){
	SIM808_Clean_Buffer(Buffer,length);
	SIM808_Read_Buffer(Buffer,length);
	return strlen(Buffer);
}
char* GPS_GSM_SIM808 :: get_IP_Adderss(){
	return ip_string ;
}
unsigned long GPS_GSM_SIM808 :: get_IP_Number(){
	return _ip ;
}
bool  GPS_GSM_SIM808 :: get_location(const char *APN ,float *longitude ,float *latitude){
	int i = 0;
	char gprsBuffer[80];
	char buffer[20];
	char *s;
	//AT+SAPBR Bearer settings for applications based on IP
	//Respones : +SAPBR: (0-4),(1-3), "ConParamTag","ConParamValue"
	//AT+SAPBR=<cmd_type>,<cid>[,<ConParamTag>,<ConParamValue>]
	//if cmd_type = 2
	/*
	Response
	OK
	If<cmd_type> = 2
	+SAPBR: <cid>,<Status>,<IP_Addr>
	OK
	If <cmd_type>=4
	+SAPBR:
	<ConParamTag>,<ConParamValue>
	OK
	Unsolicited Result Code
	+SAPBR <cid>: DEACT
	Parameters
	-------------------------------
	<cmd_type>
	0 Close bearer
	1 Open bearer
	2 Query bearer
	3 Set bearer parameters
	4 Get bearer parameters
	<cid> Bearer profile identifier
	----------------------------------
	<Status>
	0 Bearer is connecting
	1 Bearer is connected
	-------------------------------------------
	<ConParamTag> Bearer parameter
	"CONTYPE" Type of Internet connection. Value refer to
	<ConParamValue_ConType>
	"APN" Access point name string: maximum 64
	characters
	"USER" User name string: maximum 32 characters
	"PWD" Password string: maximum 32 characters
	"PHONENUM" Phone number for CSD call
	"RATE" CSD connection rate. For value refer to
	<ConParamValue_Rate>
	<ConParamValue> Bearer paramer value
	<ConParamValue_ConType>
	"CSD" Circuit-switched data call.
	"GPRS" GPRS connection.
	<ConParamValue_Rate>
	0 2400
	1 4800
	2 9600
	3 14400
	<IP_Addr> The IP address of bearer
	*/
	//select cmd_tye =3  (open bearer) and cid  =1 Bearer is Connected
	SIM808_check_Wait_Command("AT+SAPBR=3,1,\"CONTYPE\",\"SIM808\"\r","OK\r\n",Command);
	SIM808_Send_Command("AT+SAPBR=3,1,\"APN\",\"");
	if(APN){
		SIM808_Send_Command(APN);
	}
	SIM808_check_Wait_Command("\"\r","OK\r\n",Command);
	sim808_flush_serial();
	//AT+CIPGSMLOC GSM location and time
	//AT+CIPGSMLOC=<type>,<cid>
	/*Response
	If <type>=1:
	+CIPGSMLOC:<locationcode>[,<longitude>,<latitude>,<date>,<time>]
	OK
	If <type>=2:
	+CIPGSMLOC:<locationcode>[,<date>,<time>]
	<type> 1 View the longitude, latitude and time
	2 View time
	<cid> network parameters, refer to AT+SAPBR
	<locationcode> 0 Success
	If the operation failed, the location code is not 0,
	such as:404 Not Found
	*/
	/*<longitude> Current longitude in degrees
	<latitude> Current latitude in degrees
	<date> Format is YYYY/MM/DD, the time zone is GMT E.g.
	2011/01/26
	<time> Format is hh/mm/ss,the time zone is GMT.E.g. 06:10:46*/
	SIM808_Send_Command("AT+CIPGSMLOC=1,1\r");
	SIM808_Clean_Buffer(gprsBuffer ,sizeof(gprsBuffer));
	SIM808_Read_Buffer(gprsBuffer ,sizeof(gprsBuffer),2*Time_OUT ,6*INTERCHAR_TIMEOUT);
	if(NULL !=(s=strstr(gprsBuffer ,"CIPGSMLOC:"))){
		s=strstr((char *)s,",");
		s=s+1;
		while(*(++s) !=',')
		buffer[i++]=*s;
		buffer[i]=0 ;
		*longitude=atof(buffer);
		i=0;
		while(*(++s)!=',')
		buffer[i]=0;
		*latitude=atof(buffer);
		return true ;
	}
	return false ;
}

bool  GPS_GSM_SIM808 :: Attach_GPS(){
	if(!SIM808_check_Wait_Command("AT+CGNSPWR=1\r\n","OK\r\n",Command)){
		return false ;
	}
	if(SIM808_check_Wait_Command("AT+CGNSTST=1\r\n","OK\r\n",Command)){
		return false ;
	}
	return true ;
}

bool  GPS_GSM_SIM808 :: Detach_GPS(){
	if(!SIM808_check_Wait_Command("AT+CGNPWR=0\r\n","OK\r\n",Command)){
		return false ;
	}
	return false ;

}

bool GPS_GSM_SIM808  :: get_GPRMC(){
	char c ;
	static bool endflag ;
	static char count ;
	while (EUSART_Available())
	{
		c=EUSART_Read();
		if(endflag){
			if(count--){
				
				Recevied_Stack[Recevied_Stack_Index++]=c ;
				}else{
				endflag =false;
				Recevied_Stack[Recevied_Stack_Index]='\0';
				return true ;
			}
			}else{
			switch(c){
				case  '$' :
				Recevied_Stack_Index=0;
				Recevied_Stack[Recevied_Stack_Index++]=c;
				break ;
				case '*' :
				endflag =true ;
				count=2 ;
				Recevied_Stack[Recevied_Stack_Index++]=c;
				break;
				default:
				if(Recevied_Stack_Index<128)
				Recevied_Stack[Recevied_Stack_Index++]=c ;
				break;
			}
			return false ;
		}
		return false ;
	}
	return false ;
}
void  GPS_GSM_SIM808 :: get_Time(uint32_t time){
	Gps_Data.hour=time/1000000;
	Gps_Data.minute=(time/10000)%100 ;
	Gps_Data.second=(time/100)%100;
	Gps_Data.centisecond=(time%100);
}
void  GPS_GSM_SIM808 :: get_Date(uint32_t  Date){
	uint16_t year =Date %100;
	Gps_Data.year=year+2000;
	Gps_Data.month=(Date/100)%100 ;
	Gps_Data.day=Date/10000;
}
int32_t   GPS_GSM_SIM808 :: parse_Decimal(const char *term){
	bool negative ;
	int32_t ret ;
	negative=*term=='-' ;
	if(negative)
	++term ;
	ret =100*(int32_t)atol(term);
	while(isdigit(*term))
	++term ;
	if (*term=='.' && isdigit(term[1])){
	ret=ret+10*(term[1]-'0');
	if(isdigit(term[2]))
	ret=ret +(term[2]-'0');
}

return negative ?-ret:ret;

}
void  GPS_GSM_SIM808 :: latitude_To_DMS(){
	float Temp ;
	latDMS.Degress=(int)Gps_Data.latitude ;
	Temp=(Gps_Data.latitude-latDMS.Degress)*60;
	latDMS.minutes=(int)Temp ;
	latDMS.seconds=(Temp-latDMS.minutes)*60 ;
}
void  GPS_GSM_SIM808 :: longitude_TO_DMS(){
	float Temp ;
	logDMS.Degress=(int)Gps_Data.longitude;
	Temp=(Gps_Data.longitude-logDMS.Degress)*60;
	logDMS.minutes=(int)Temp ;
	logDMS.seconds=(Temp-logDMS.minutes)*60 ;
}
bool  GPS_GSM_SIM808 ::parse_GPRMC(char *GPS_Buffer){
	if(strstr(GPS_Buffer ,des) ==NULL){
		Recevied_Stack_Index =0;
		return false ;
		} else {
		if(GPS_Buffer[18]=='A')
		return true ;
		else {
			return false ;
		}
	}
}
//get GPS signal
bool  GPS_GSM_SIM808 ::get_gps(){
	if(!get_GPRMC())
	return false ;
	if(!parse_GPRMC(Recevied_Stack))
	return false ;
	char *tok =strtok(Recevied_Stack ,",");
	if(! tok)
	return false ;
	// grab time
	char *time=strtok(NULL,",");
	if(!time)
	return false ;
	uint32_t newTime=(uint32_t)parse_Decimal(time);
	get_Time(newTime);
	//skip fix
	tok =strtok(NULL ,",");
	if(!tok)
	return false ;
	//garp latitude 
	char *latp=strtok(NULL,",");
	if(!latp) 
	return false ;
	// grab latitude direction 
	char *latdir =strtok(NULL,",");
	if(!latdir)
	return false ;
		//garp longitude
		char *logp=strtok(NULL,",");
		if(!logp)
		return false ;
		// grab longitude direction
		char *logdir =strtok(NULL,",");
		if(!logdir)
		return false ;
	float latitude =atof(latp);
	float longitude =atof(logp);
	Gps_Data.latitude=latitude/100 ;
	// convert longitude from minutes to decimal
	Gps_Data.longitude=longitude/100 ;
	char *speedp=strtok(NULL,",");
	if(!speedp)
	return false ;
	 // convert to kph
	 //*speed_kph = atof(speedp) * 1.852;
	 Gps_Data.speed_kph=atof(speedp)*1.852 ;
	 char *coursep=strtok(NULL,",");
	 if(!coursep)
	 return false ;
	 Gps_Data.heading=atof(coursep);
	 // grab date 
	 char *date =strtok(NULL ,",");
	 if(!date)
	 return false ;
	 uint32_t newDate =atol(date);
	 get_Date(newDate);
	 return true ;
}
uint32_t  GPS_GSM_SIM808 :: str_to_ip(const char *str){
	uint32_t ip =0 ;
	char *p=(char *)(str);
	for(int i=0;i<4;i++){
		ip |=atoi(p);
		p=strchr(p,'.');
		if(p==NULL)
		break;
		ip =ip<<8 ;
		p++;
	}
	return ip ;
}