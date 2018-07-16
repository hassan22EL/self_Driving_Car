/*
* GPS.h
*
* Created: 7/10/2018 4:16:59 PM
*  Author: hassa
*/


#ifndef GPS_H_
#define GPS_H_
/*
GGA Global Positioning System Fix Data. Time, Position and fix related data
for a GPS receiver

1         2       3 4        5 6 7  8   9   10 11 12 13  14 15
|         |       | |        | | |  |   |   |  |  |  |   |  |
Data =$--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
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
#define Buffer_Size 150
#define degrees_buffer_size 20
#define SREG    _SFR_IO8(0x3f)
#define F_CPU 16000000UL
char    Latitude_Buffer[15];
char    Longitude_Buffer[15];
char    Time_Buffer[15];
char    Altitude_Buffer[8];
char    degrees_buffer[degrees_buffer_size];   /* save latitude or longitude in degree */
char    GGA_Buffer[Buffer_Size];               /* save GGA string */
uint8_t GGA_Pointers[20];                   /* to store instances of ',' */
char    GGA_CODE[3];
volatile uint16_t GGA_Index, CommaCounter;
bool GGA_String ,Flag1, Flag2;
void convert_time_to_UTC();
char *convert_to_degrees(char *Row);
void GPS_Time();/* parse Time in GGA string stored in buffer */
char *GPS_latitude(uint16_t lat_pointer);/* parse Latitude in GGA string stored in buffer */
char *GPS_longitude(uint16_t long_pointer);/* parse Longitude in GGA string stored in buffer */
void GPS_Altitude(uint16_t Alt_pointer);/* parse Altitude in GGA string stored in buffer */
void GPS_INIT();

#endif /* GPS_H_ */