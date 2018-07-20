/*
 * SIM808.h
 *
 * Created: 7/18/2018 9:45:15 AM
 *  Author: hassa
 */ 
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <avr/pgmspace.h>
#ifndef SIM808_H_
#define SIM808_H_
#define  Time_OUT  5 //second
#define  INTERCHAR_TIMEOUT 1500  //millisecond
#define pgm_read_byte(address_short) \
pgm_read_byte_near(address_short)
enum DataType{
	Command =0,
	Data=1
};
void SIM808_INIT(unsigned long Baud_Rate);
int SIM808_check_Readable();
int SIM808_Wait_Readable(int Wait_Time);
void sim808_flush_serial();
void SIM808_Read_Buffer(char *Buffer,int count ,unsigned Time_out=Time_OUT ,unsigned int charTimeout=INTERCHAR_TIMEOUT);
void SIM808_Clean_Buffer(char *Buffer ,int count);
void SIM808_Send_Byte(uint8_t Data);
void SIM808_Send_Char(const char Data);
void SIM808_Send_Command(const char *Command);
void SIM808_Send_Command_P(const char *Command);
bool SIM808_Send_AT();
void SIM808_Send_End_Mark();
bool SIM808_Wait_For_Respons(const char *Respones ,DataType Type,unsigned int Time_out=Time_OUT ,unsigned int charTimeOut=INTERCHAR_TIMEOUT);
bool SIM808_check_Wait_Command(const char *Command ,const char *Response ,DataType type,unsigned int Time_out=Time_OUT ,unsigned int charTimeOut=INTERCHAR_TIMEOUT);
uint64_t millis();
#endif /* SIM808_H_ */