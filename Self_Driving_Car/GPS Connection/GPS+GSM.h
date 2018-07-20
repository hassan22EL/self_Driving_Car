/*
 * GPS_GSM.h
 *
 * Created: 7/20/2018 10:13:56 PM
 *  Author: hassa
 */ 


#ifndef GPS+GSM_H_
#define GPS+GSM_H_
/* TCP (Tran mission control Protocol )
under layer 4
Byte Stream is broken  up into segments
TCP Format
------------------------------------------------------------------------------------
Sources port (16 bits)      |            Destinations (16bit)                       |
------------------------------------------------------------------------------------
Sequences number(32 bit)                                    |
------------------------------------------------------------------------------------
Acknowledgment number (32 bit)                                |
------------------------------------------------------------------------------------
offset(4bit)   | Reserved(4 bit) | control bits(8bit) |   Windows size (16bit)      |
------------------------------------------------------------------------------------
check Sum (16 bit)       |              urgent pointer (16 bit              |
-------------------------------------------------------------------------------------
options                                    |
------------------------------------------------------------------------------------
Data                                     |
------------------------------------------------------------------------------------

*/
/* UDP (user DataDigram protocol under layer 4

UDP format
----------------------------------------------------------------
Sources port (16 bits) |    Destinations (16bit)                |
----------------------------------------------------------------
length                 |   checkSum                             |
----------------------------------------------------------------
Data                                     |
-----------------------------------------------------------------
*/
enum protocol {
	CLOSED =0,
	TCP=1 ,
	UDP=2
};
class GPS_GSM_SIM808{
	public:
	//GPS_GSM_SIM808(uint8_t Tx ,uint8_t Tx ,uint8_t BaudRate=9600);
	static GPS_GSM_SIM808 *Get_Instancs(){
		return inst ;
	};
	/** initialize SIM808 module including SIM card check & signal strength
	*  @return true if connected, false otherwise
	*/
	bool INIT(void);
	/** check if SIM808  module is powered on or not
	*  @returns
	*      true on success
	*      false on error
	*/
	bool check_power_up(void);
	
	void Power_UP_Down(uint8_t pin);
	void power_Rest(uint8_t pin);
	/** send text SMS
	*  @param  *number phone number which SMS will be send to
	*  @param  *data   message that will be send to
	*  @returns
	*      false on success
	*      true on error
	*/
	bool Send_SMS(char *Number ,char *Data);
	/** Check if there is any UNREAD SMS: this function DOESN'T change the UNREAD status of the SMS
	*  @returns
	*      1..20 on success, position/index where SMS is stored, suitable for the function ReadSMS
	*      -1 on error
	*       0 - there is no SMS with specified status (UNREAD)
	*/
	char IS_SMS_unread();
	/** read SMS, phone and date if getting a SMS message. It changes SMS status to READ
	*  @param  messageIndex  SIM position to read
	*  @param  message  buffer used to get SMS message
	*  @param  length  length of message buffer
	*  @param  phone  buffer used to get SMS's sender phone number
	*  @param  Datetime  buffer used to get SMS's send datetime
	*  @returns
	*      true on success
	*      false on error
	*/
	bool Read_SMS(int messageIndex ,char * message ,int length ,char *phone ,char *DateTime );
	/** read SMS if getting a SMS message
	*  @param  buffer  buffer that get from DFRobot_SIM808 module(when getting a SMS, DFRobot_SIM808 module will return a buffer array)
	*  @param  message buffer used to get SMS message
	*  @param  check   whether to check phone number(we may only want to read SMS from specified phone number)
	*  @returns
	*      true on success
	*      false on error
	*/
	bool Read_SMS(int index,char *Buffer ,int length);
	bool Delete_SMS(int index);
	/** call someone
	*  @param  number  the phone number which you want to call
	*  @returns
	*      true on success
	*      false on error
	*/
	
	bool Call_UP(char *Number);
	
	/** auto answer if coming a call
	*  @returns
	*/
	bool Answer(void);
	
	/** hang up if coming a call
	*  @returns
	*      true on success
	*      false on error
	*/
	bool hangup(void);
	/*
	** Disable +CLIP notification when an incoming call is active, RING text is always shown. See isCallActive function
	*  This is done in order no to overload serial outputCheck if there is a call active and get the phone number in that case
	*  @returns
	*      true on success
	*      false on error
	*/
	bool Disable_CLTP_pring(void);
	
	/** Get Subscriber Number (your number) using AT+CNUM command, but if nothing returns, then
	*  you need to command this to your SIM900. (See AT+CPBS, AT+CPBW)
	*	AT+CPBS="ON"
	*	AT+CPBW=1,"+{Your Number}",145
	*	AT+CPBS="SM"
	*  @param
	*	@return
	*		true on success
	*		false on error
	*/
	bool get_subscribler_Number(char *Number);
	/** Check if there is a call active and get the phone number in that case
	*  @returns
	*      true on success
	*      false on error
	*/
	bool IS_Call_Active(char *Number);
	
	/** get DateTime from SIM900 (see AT command: AT+CLTS=1) as string
	*  @param
	*  @returns
	*      true on success
	*      false on error
	*
	* 	   If it doesn't work may be for two reasons:
	*	 	1. Your carrier doesn't give that information
	*		2. You have to configuration the SIM900 IC.
	*			- First with SIM900_Serial_Debug example try this AT command: AT+CLTS?
	*			- If response is 0, then it is disabled.
	*			- Enable it by: AT+CLTS=1
	*			- Now you have to save this config to EEPROM memory of SIM900 IC by: AT&W
	*			- Now, you have to power down and power up again the SIM900
	*			- Try now again: AT+CCLK?
	*			- It should work now
	*/
	bool get_Date_Time(char *Buffer);
	/** get Signal Strength from SIM900 (see AT command: AT+CSQ) as integer
	*  @param
	*  @returns
	*      true on success
	*      false on error
	*/
	bool get_Signal_Strength(int *buffer);
	/** Send USSD Command Synchronously (Blocking call until unsolicited response is received)
	*  @param
	*		*ussdCommand string command UUSD, ex: *123#
	*		*resultCode	char Result Code, see AT+CUSD command
	*		*response	string response
	*		*cellBroadcast	int Cell Broadcast Data Coding Scheme
	*  @returns
	*      true on success
	*      false on error
	*/
	bool Send_USSD__Synchronous(char *UesdCommand ,char *Result_Code ,char *Response);
	/** Cancel USSD Session
	*  @returns
	*      true on success cancel active session
	*      false on error or because no active session
	*/
	bool cancel_USSD_Synchronous(void);
	/**  Connect the DFRobot_SIM808 module to the network.
	*  @return true if connected, false otherwise
	*/
	/**  Connect the DFRobot_SIM808 module to the network.
	*  @return true if connected, false otherwise
	APN :Access point Name
	*/
	bool join(const char *APN ,const char *UserName ,const char *Password);

	/** Disconnect the DFRobot_SIM808 module from the network
	*  @returns
	*/
	void Disconnect(void);
	/*
	** Open a tcp/udp connection with the specified host on the specified port
	*  @param socket an endpoint of an inter-process communication flow of SIM808 module,for SIM900 module, it is in [0,6]
	*  @param ptl protocol for socket, TCP/UDP can be choosen
	*  @param host host (can be either an ip address or a name. If a name is provided, a dns request will be established)
	*  @param port port
	*  @param timeout wait seconds till connected
	*  @param chartimeout wait milliseconds between characters from DFRobot_SIM808 module
	*  @returns true if successful
	*/
	bool connect(protocol ptl, const char * host, int port, int timeout = 2 * Time_OUT, int chartimeout = 2 * INTERCHAR_TIMEOUT);
	/** Check if a tcp link is active
	*  @returns true if successful
	*/
	bool IS_Connected(void);
	/** Close a tcp connection
	*  @returns true if successful
	*/
	bool Close();
	/** check if DFRobot_SIM808 module is readable or not
	*  @returns true if readable
	*/
	bool Readable(void);
	/** wait a few time to check if DFRobot_SIM808 module is readable or not
	*  @param socket socket
	*  @param wait_time time of waiting
	*/
	bool wait_Readable(int wait_Time);
	bool wait_Writable(int req_size);
	/** send data to socket
	*  @param socket socket
	*  @param str string to be sent
	*  @param len string length
	*  @returns return bytes that actually been send
	
	*/
	int Send(const char *String ,int length);
	/** read data from socket
	*  @param socket socket
	*  @param buf buffer that will store the data read from socket
	*  @param len string length need to read from socket
	*  @returns bytes that actually read
	*/
	int Receive(char *Buffer ,int length);
	
	/** convert the host to ip
	*  @param host host ip string, ex. 10.11.12.13
	*  @param ip long int ip address, ex. 0x11223344
	*  @returns true if successful
	*/
	//NOT USED bool gethostbyname(const char* host, uint32_t* ip);
	char *get_IP_Adderss();
	unsigned long get_IP_Number();
	bool get_location(const char *APN ,float *longitude ,float *latitude);
	//open and close GPS
	bool Attach_GPS();
	bool Detach_GPS();
	// Parse a (potentially negative) number with up to 2 decimal digits -xxxx.yy
	void get_Time(uint32_t time);
	void get_Date(uint32_t  Date);
	int32_t  parse_Decimal(const char *term);
	void latitude_To_DMS();
	void longitude_TO_DMS();
	//parser Serial data
	// GPRMC Recommended minimum specific GPS/Transit data
	/*
	Recommended minimum specific GPS/Transit data

	eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
	eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68


	225446       Time of fix 22:54:46 UTC
	A            Navigation receiver warning A = OK, V = warning
	4916.45,N    Latitude 49 deg. 16.45 min North
	12311.12,W   Longitude 123 deg. 11.12 min West
	000.5        Speed over ground, Knots
	054.7        Course Made Good, True
	191194       Date of fix  19 November 1994
	020.3,E      Magnetic variation 20.3 deg East
	*68          mandatory checksum


	eg3. $GPRMC,220516,A,5133.82,N,00042.24,W,173.8,231.8,130694,004.2,W*70
	1    2    3    4    5     6    7    8      9     10  11 12


	1   220516     Time Stamp
	2   A          validity - A-ok, V-invalid
	3   5133.82    current Latitude
	4   N          North/South
	5   00042.24   current Longitude
	6   W          East/West
	7   173.8      Speed in knots
	8   231.8      True course
	9   130694     Date Stamp
	10  004.2      Variation
	11  W          East/West
	12  *70        checksum


	eg4. $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
	1    = UTC of position fix
	2    = Data status (V=navigation receiver warning)
	3    = Latitude of fix
	4    = N or S
	5    = Longitude of fix
	6    = E or W
	7    = Speed over ground in knots
	8    = Track made good in degrees True
	9    = UT date
	10   = Magnetic variation degrees (Easterly var. subtracts from true course)
	11   = E or W
	12   = Checksum
	*/
	bool parse_GPRMC(char *GPS_Buffer);
	bool get_GPRMC();
	//get GPS signal
	bool get_gps();
	public :
	struct gps_data{
		uint16_t year ;
		uint8_t month ;
		uint8_t day ;
		uint8_t hour ;
		uint8_t minute;
		uint8_t second ;
		uint8_t centisecond;
		float latitude ;
		float longitude ;
		float speed_kph ;
		float heading ;
		float Altitude ;
	}Gps_Data;
	struct DMS_Data{
		int Degress ;
		int minutes ;
		float seconds;
	}latDMS,logDMS;
	private:
	short serail_flag ;
	bool check_SIMS_Status(void);
	uint32_t str_to_ip(const char *str);
	static GPS_GSM_SIM808 *inst ;
	uint32_t _ip ;
	char ip_string[16];//XXX.YYY.ZZZ.WWW + \0
	
};





#endif /* GPS+GSM_H_ */