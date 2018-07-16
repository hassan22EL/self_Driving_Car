/*
 * GPS.c
 *
 * Created: 7/10/2018 4:17:22 PM
 *  Author:Hassan Elsaied 
                                         *****The National Marine Electronics Association Protocol (NMEA)) 
										 /*
										 NMEA Working as asynchronous serial interface with the following parameters:
										 Baud rate: 9
										 Number of data bits: 8 (bit 7 is 0)
										 Stop bits: 1 (or more)
										 Parity: none
										 Handshake: none
										 NMEA 0183 allows a single talker and several listeners on one circuit
										 Data sender From NMEA protocol in
										 GGA Global Positioning System Fix Data. Time, Position and fix related data for a GPS receiver
										 
										$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
										1) Time (UTC)
										2) Latitude
										3) N or S (North or South)
										4) Longitude
										5) E or W (East or West)
										6) GPS Quality Indicator,
										0 - fix not available,
										1 - GPS fix,
										2 - Differential GPS fix
										7) Number of satellites in view, 00 - 12
										8) Horizontal Dilution of precision
										9) Antenna Altitude above/below mean-sea-level (geoid)
										10) Units of antenna altitude, meters
										11) Geoidal separation, the difference between the WGS-84 earth
										ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level below ellipsoid
										12) Units of geoidal separation, meters
										13) Age of differential GPS data, time in seconds since last SC104
										type 1 or 9 update, null field when DGPS is not used
										14) Differential reference station ID, 0000-1023
										15) Checksum
 */ 
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "/projects/AVR/TRacking Car/TRacking Car/GPS.h"
#include "/projects/AVR/EUSART/EUSART/EUSART.h"
#define F_CPU 16000000UL
void convert_time_to_UTC(){
	unsigned int Hour,Minuet,Second ;
	uint32_t Time_Value ;
	Time_Value=atol(Time_Buffer); //  convert string to long integer value 
	// for Example  the Time Value= 145436  (14:54:36)  Hour =14 , minuet =54 ,sec=36 
	Hour =(Time_Value/10000); // extract hour from integer  145436/10000 =14 remained =5436
	//if Remained /100 =54 
	Minuet=(Time_Value%10000)/100 ; // extract minuet from integer 5436/100=54 and Remained2=36 
	Second=(Time_Value%10000)%100; // extract second from integer 
	sprintf(Time_Buffer,"%d:%d:%d",Hour,Minuet,Second);
	
}
char *convert_to_degrees(char *Row){
	double value;
	float Decimal_Vaule ,temp ;
	int32_t  Degrees;
	float Position;
	value=atof(Row); //Convert String (latitude or longitude ) to float Number ;
	/* convert Raw latitude/longitude into degree format */
	Decimal_Vaule =(value/100 );
	Degrees =(int)Decimal_Vaule ;
	temp=(Decimal_Vaule-(int )Decimal_Vaule)/.6 ;
	Position =(float)Degrees+temp;
 char *String=dtostrf(Position,6,4,degrees_buffer); //convert float value into string 
 return String ;

	}
void GPS_Time(){
	cli(); //clear Global interrupt 
	uint8_t time_index =0 ;
	/* parse Time in GGA string stored in buffer */
	for(uint8_t index=0; GGA_Buffer[index]!=',' ;index++){
		Time_Buffer[time_index]=GGA_Buffer[index];
		time_index++ ;
		}
		convert_time_to_UTC();
		sei(); //Set Global Interrupt Flag
	}
char *GPS_latitude(uint16_t lat_pointer){
	cli(); //clear Global interrupt 
	uint8_t lat_index ;
	uint8_t index=lat_pointer+1;
	lat_index=0;
	/* parse Latitude in GGA string stored in buffer */
	for(;GGA_Buffer[index]!=',';index++){
	Latitude_Buffer[lat_index]=GGA_Buffer[index];
	lat_index++;
	}
	/*Get  Directions*/
	Latitude_Buffer[lat_index++]=GGA_Buffer[index++];
	Latitude_Buffer[lat_index]=GGA_Buffer[index] ;//Get directions N/S
	char * latitude =convert_to_degrees(Latitude_Buffer);
	sei();//Set Global Interrupt Flag
	return latitude ;
	}
char *GPS_longitude(uint16_t long_pointer){
    cli(); //clear Global interrupt 
	uint8_t long_index ;
	uint8_t index =long_pointer+1;
	long_index=0;
		/* parse Longitude in GGA string stored in buffer */
		for (;GGA_Buffer[index]!=',';index++){
	    Longitude_Buffer[long_index]=GGA_Buffer[index];
		long_index++;
		}
		/*Get  Directions*/
		Longitude_Buffer[long_index++]=GGA_Buffer[index++];
		Longitude_Buffer[long_index]=GGA_Buffer[index]; //Get directions E/W
		char *longitude=convert_to_degrees(Longitude_Buffer);
		sei();//Set Global Interrupt Flag
		return longitude ;
		
	}
void GPS_Altitude(uint16_t Alt_pointer){
	cli(); //clear Global interrupt 
	uint8_t alt_index ;
	uint8_t   index =Alt_pointer+1;
	alt_index=0;
	/* parse Altitude in GGA string stored in buffer */
	for (;GGA_Buffer[index]!=',';index++)
	{
		Altitude_Buffer[alt_index]=GGA_Buffer[index];
		alt_index++;
	}
	Altitude_Buffer[alt_index]=GGA_Buffer[index+1];
	sei();//Set Global Interrupt Flag
	
}
void GPS_INIT(){
	memset(GGA_Buffer,0,Buffer_Size);
	memset(degrees_buffer,0,degrees_buffer_size);
	EUSART_Start(9600);
}
ISR (USART_RXC_vect){
	uint8_t oldsrg=SREG;
	cli(); //clear Global interrupt 
	char Recived_char=UDR ;
	if(Recived_char=='$'){
		GGA_Index =0;
		CommaCounter=0;
		GGA_String=false;
	}else if(GGA_String==true){
		   /* if true save GGA info. into buffer */
		   if(Recived_char==',')
		   GGA_Pointers[CommaCounter++]=GGA_Index ;    /* store instances of ',' in buffer */
		   GGA_Buffer[GGA_Index++]=Recived_char ;
		     }else if(GGA_CODE[0]=='G' && GGA_CODE[1]=='G' && GGA_CODE[2]=='A'){ /* check for GGA string */
				 GGA_String=true ;
				 GGA_CODE[0]=0;
				 GGA_CODE[1]=0;
				 GGA_CODE[2]=0;
				 
			 }else{
				 GGA_CODE[0]=GGA_CODE[1];
				 GGA_CODE[1]=GGA_CODE[2];
				 GGA_CODE[2]=Recived_char;
			 }
			 SREG = oldsrg;
}