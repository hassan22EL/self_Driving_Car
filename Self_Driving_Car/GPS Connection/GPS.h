/*
 * GPS.h
 *
 * Created: 7/10/2018 4:16:59 PM
 *  Author: hassa
 */ 


#ifndef GPS_H_
#define GPS_H_

struct _nmeaData {
	///Pointer to the NEMA string
	char *nmeaString;
	///Pointer to the next NEMA object
	struct _nmeaData * next;
};
typedef volatile struct _nmeaData nmeaData;
///Struct to store fixes as addressible parts rather than a string - 26 bytes
struct _fixData {
	
	char latDir;///N/S direction
	char lonDir;//E/W direction
	uint32_t date;//Date as ddmmyy
	uint32_t time;//Time as hhmmss UTC
	uint32_t lat1;//First portion of the latitude decimal
	uint32_t lat2;//Second portion of the latitude decimal
	uint32_t lon1;	//First portion of the longitude decimal
	uint32_t lon2;//Second portion of the longitude decimal
};

typedef struct _fixData fixData;

//Functions
/*Interpreted sentences
`$GPBOD - Bearing, origin to destination
$GPBWC - Bearing and distance to waypoint, great circle
$GPGGA - Global Positioning System Fix Data
$GPGLL - Geographic position, latitude / longitude
$GPGSA - GPS DOP and active satellites
$GPGSV - GPS Satellites in view
$GPHDT - Heading, True
$GPR00 - List of waypoints in currently active route
$GPRMA - Recommended minimum specific Loran-C data
$GPRMB - Recommended minimum navigation info
$GPRMC - Recommended minimum specific GPS/Transit data
$GPRTE - Routes
$GPTRF - Transit Fix Data
$GPSTN - Multiple Data ID
$GPVBW - Dual Ground / Water Speed
$GPVTG - Track made good and ground speed
$GPWPL - Waypoint location
$GPXTE - Cross-track error, Measured
$GPZDA - Date & Time*/
int parseGPRMC(char *item);// Recommended minimum specific GPS/Transit data
void Entry_New();
GPS_INIT();



#endif /* GPS_H_ */