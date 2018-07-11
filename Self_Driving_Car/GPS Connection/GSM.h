/*
 * GSM.h
 *
 * Created: 7/10/2018 10:06:21 PM
 *  Author: hassa
 */ 

#include <inttypes.h>
#include <string.h>
#include <stdint-gcc.h>
#ifndef GSM_H_
#define GSM_H_
//Error list 
#define SIM300_OK  1
#define SIM300_INVALID_Respons  -1
#define SIM300_FIAL -2
#define  SIM300_TIMEOUT   -3 
//status 
#define  SIM300_NW_REGISTERED_HOME  1
#define SIM300_NW_SHARCHING         2
#define SIM300_NW_REGISTED_ROAMING	5
#define SIM300_NW_ERROR             99

#define SIM300_SIM_PRESENT         1
#define SIM300_SIM_NOT_PRESENT      0

int8_t SIM300_Command(const char command);
int8_t	SIM300_Init();
int8_t	SIM300_CheckResponse(const char *response,const char *check,uint8_t len);
int8_t	SIM300_WaitForResponse(uint16_t timeout);
int8_t	SIM300_GetNetStat();
int8_t	SIM300_IsSIMInserted();
uint8_t SIM300_GetProviderName(char  *Name);
int8_t	SIM300_GetIMEI(char *EMEI);
int8_t	SIM300_GetManufacturer(char *man_id);
int8_t	SIM300_GetModel(char *Model);




#endif /* GSM_H_ */