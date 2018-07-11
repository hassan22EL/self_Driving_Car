/*
 * GPS.c
 *
 * Created: 7/10/2018 4:17:22 PM
 *  Author: hassa
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "/projects/AVR/TRacking Car/TRacking Car/GPS.h"
#include "/projects/AVR/EUSART/EUSART/EUSART.h"
#define F_CPU 16000000UL
#define NMEA_BUFFER_LEN 85 //Maximum length of a NEMA string for buffer allocation
volatile char *buffer;  //Pointer to our current RX buffer
volatile uint8_t buffer_index = 0;//Counter for our current position in the buffer.
//We don't check that (buffer_index < bufferSize) because we know the NEMA limits
nmeaData *gpsData; //Pointer to our linked list of NEMA strings
int parseGPRMC(char *item){
	char out[NMEA_BUFFER_LEN]="";
	fixData Fix ;
	memset(&Fix,0x00,sizeof(Fix));//Date ,Time , latitude ,longitude equles zeros
	/* split string */
	uint8_t pos =0 ;
	uint8_t DecimalPos =0;
	char *token =strtok(item,","); 
	while(token !=NULL){
		pos++;
		switch(pos){
			case 1:
			break ;
			case 2:
			Fix.time =atol(token);//converted String to long int 
			break;
			case 3 :
			if(token[0]!=0x41)return 1 ;
			break ;
			case 4: //Copy latitude to fixData element
			for(DecimalPos=0;DecimalPos<strlen(token) ; DecimalPos++){
				if(token[DecimalPos]==0x2E) //look for "." 
				break;
			}
			if(DecimalPos ==strlen(token)) return 1 ;
			//Read First half
			token[DecimalPos]=0x00 ; //set value in point off 
			Fix.lat1=atol(token); //converted String to long int 
			Fix.lat2=atol(&token[DecimalPos+1]); //
			break;
			case  5://Add North / South to fixData element (N/S)
			memcpy(&Fix.latDir,token,sizeof(char));//Copy token in latDir 
			break;
			case 6://Copy longitude to fixData element
			for(DecimalPos=0;DecimalPos <strlen(token);DecimalPos++){
				if(token[DecimalPos]==0x2E); //look for "." 
				break ;
			}
			if(token[DecimalPos]==strlen(token)) return 1 ;
			//Read First half ;
			token[DecimalPos]==0x00 ;
			Fix.lon1=atol(token);
			Fix.lon2=atol(&token[DecimalPos+1]);
			break;
			case 7 :
			//Add East /west to Fix Elemnt 
			memcpy(&Fix.lonDir,token,sizeof(char));
			break ;
			case  8 :
			break ;
			case 9 :
			break ;
			case 10:
			//data 
			Fix.date =atol(token);
			break ;
			case 11:
			break ;
			case 12 :
			break ;
			case 13 :
			break ;
			default:
			break ;
	} 

	}
	
	return 0;
}
		
void Entry_New(){
	if(gpsData->next !=0){
		cli();// Clear Global Interrupt Flag
		volatile nmeaData *itme =gpsData ;
		gpsData=gpsData->next ;
		sei();//set 
		if(strncmp(itme->nmeaString,"$GPRMC",6)==0){//compared between String 1 and string 2 
            parseGPRMC(itme->nmeaString);
			} 
		free((void *)itme->nmeaString);
		free((void *)itme);
	}
}
GPS_INIT(){
	gpsData=malloc(sizeof(nmeaData));
	gpsData->next=0;
	buffer=malloc(sizeof(char)*NMEA_BUFFER_LEN);
	_delay_ms(80);
	EUART_Start(9600);
}
ISR(EUSART_RX_Vector){//External interrupt From Rx pin
	uint8_t c;
	c=UDR;
	buffer[buffer_index]=c;
	buffer_index++;
	if(c=='\n'){
		volatile nmeaData *item =gpsData ;
		while (item->next!=0)
		item=item->next;
		item=(nmeaData *)malloc(sizeof(nmeaData));
		if(item->next==NULL){
			buffer_index=0;
			return ;
			
		}
		item=item->next;
		item->nmeaString=buffer;
		buffer=malloc(sizeof(char)*NMEA_BUFFER_LEN);
		if(buffer==NULL){
			buffer=item->nmeaString;
			buffer_index=0;
			return;
		}
		item->next=0;
		buffer_index=0;
	}
	
	
}