/*
* SIM808.cpp
*
* Created: 7/18/2018 10:11:58 AM
*  Author: hassa
*/
#include <stdbool.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdint.h>
#include <avr//interrupt.h>
#include "SIM808.h"
#include "/projects/AVR/EUSART/EUSART/EUSART.h"
#include <util/atomic.h>
#define  F_CPU 16000000UL
uint64_t _millis = 0;
uint16_t _1000us = 0;
uint64_t old_millis = 0;

void  SIM808_INIT(unsigned long Baud_Rate){
	EUSART_Start(Baud_Rate);
}
int SIM808_check_Readable(){
	return EUSART_Available();
}
int SIM808_Wait_Readable(int Wait_Time){
	unsigned long Time_Start ;
	int Data_Length =0;
	Time_Start=millis();
	while ((unsigned long)(millis()-Time_Start)>Wait_Time *1000UL)
	{
		_delay_ms(500);
		Data_Length=SIM808_check_Readable();
		if(Data_Length >0){
			break;
		}
	}
	return Data_Length ;
}
void sim808_flush_serial(){
	while(SIM808_check_Readable()){
		EUSART_Read();
	}
}
void SIM808_Read_Buffer(char *Buffer,int count ,unsigned Time_out ,unsigned int charTimeout){
	int i=0;
	unsigned long TimerStart,prevChar ;
	TimerStart=millis();
	prevChar=0;
	while(1){
		while (SIM808_check_Readable)
		{
			char c =EUSART_Read();
			prevChar=millis();
			Buffer[i++]=c;
			if(i>count)
			break ;
		}
		if(i >= count)
		break;
		if((unsigned long)(millis()-TimerStart)>Time_out*1000UL){
			break;
		}
		if (((unsigned long) (millis() - prevChar) > charTimeout) && (prevChar != 0)) {
			break ;
		}
	}
}
void SIM808_Clean_Buffer(char *Buffer ,int count){
	for(int i=0; i<count ; i++){
		Buffer[i]='\0';
	}
}
void SIM808_Send_Byte(uint8_t Data){
	EUSART_Write(Data);
}
void SIM808_Send_Char(const char Data){
	EUSART_Write(Data);
}
void SIM808_Send_Command(const char *Command){
	for (int i=0;i<strlen(Command);i++)
	{
		SIM808_Send_Byte(Command[i]);
	}
}
void SIM808_Send_Command_P(const char *Command){
	while (pgm_read_byte(Command) !=0x00)
	SIM808_Send_Byte(pgm_read_byte(Command++));
}
bool SIM808_Send_AT(){
	return SIM808_check_Wait_Command("AT\n\r" ,"OK",Command);
}
void SIM808_Send_End_Mark(){
	SIM808_Send_Byte((char)26);
}
bool SIM808_Wait_For_Respons(const char *Respones ,DataType Type,unsigned int Time_out ,unsigned int charTimeOut){
	int len =strlen(Respones);
	int sum =0;
	unsigned long TimerStart,prevChar ;
	TimerStart=millis();
	prevChar=0;
	while(1){
		if(SIM808_check_Readable()){
			char c =EUSART_Read();
			prevChar=millis();
			sum=(c==Respones[sum])?sum+1 :0 ;
			if(sum==len)break;
		}
		if((unsigned long)(millis()-TimerStart)>Time_out*1000UL){
			return false ;
		}
		if ((unsigned long )(millis()-prevChar)>charTimeOut && prevChar !=0x00)
		{
			return false ;
		}
	}
	if(Type==Command)
	sim808_flush_serial();
	return true ;
}
bool SIM808_check_Wait_Command(const char *Command ,const char *Response ,DataType type,unsigned int Time_out ,unsigned int charTimeOut){
	SIM808_Send_Command(Command);
	return SIM808_Wait_For_Respons(Response,type,Time_out,charTimeOut);
}

uint64_t millis() {
	uint64_t m;
	cli();
	m = _millis;
	sei();
	return m;
}
ISR(TIM0_OVF_vect) {
	_1000us += 256;
	while (_1000us > 1000) {
		_millis++;
		_1000us -= 1000;
	}
}