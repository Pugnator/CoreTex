#include <global.hpp>
using namespace uart;
uart::Stack gsmStack;
uint32_t gsmtimeout = 0;
char retry_count = 0;

/* very important for correct SMS handling and sending
  "+CMGF=1", 1,                // PDU mode off
  "+CSCS=\"UCS2\"", 1,
  "+CSMP=17,167,0,8", 1,       // format to UCS2 16bit(8)

  //modem_write_c( 0x1A ); //Ctrl+Z
  */

Modem::Modem ( Uart& p, Uart* d )
	: port ( p )
{
	last_result = 0;
	max_retry_count = 1;
	debug = d;
	init();
}

bool Modem::rawcmd ( char* command, char* expectedResult, bool partial )
{
	gsmStack.reset();
	if ( partial )
	{
		port << command;
		return true;
	}
	port < command;
	tickcounter = GSM_DEFAULT_TIMEOUT;
	while ( !gsmStack.ready && tickcounter );
	if ( expectedResult )
	{
		if ( !strstr ( gsmStack.str(), expectedResult ) )
		{
			return false;
			/*   delay_ms(1500);
			   gsmStack.reset();
			   rawcmd(command, expectedResult);*/
		}
	}
	return tickcounter ? true : false;
}

void Modem::init ( void )
{
	PIN_LOW ( GSMDTR );
	rawcmd ( "ATE0", "OK" );
	rawcmd ( "AT+CMGF=1", "OK" );
	/*rawcmd ( "AT+IFC=2,2", "OK" );
	rawcmd ("AT+CSCS=\"UCS2\"", NULL, true);
	rawcmd ("AT+CSMP=17,167,0,8", NULL);
	port < (char)26;*/
}

bool Modem::smsw ( char* number, char* text )
{
	rawcmd ( "AT+CMGS=\"", NULL, true );
	rawcmd ( number, NULL, true );
	rawcmd ( "\"", ">" );
	rawcmd ( text, NULL );
	port < ( char ) 26;
	return true;
}

int Modem::signal_level ( void )
{
	return rawcmd ( "AT+CSQ", "OK" ) ? str10_to_uint ( gsmStack.str() ) : -1;
}

bool Modem::ready ( void )
{
	rawcmd ( "AT+CPIN?", "OK" );
	return strstr ( gsmStack.str(), "READY" ) ? true : false;
}

/*static const char hex2char[] = "0123456789ABCDEF";
static void modem2u16( uint16_t val )
{
  char i = 4;
  while( i-- )
  {
    modem_write_c( hex2char[(val>>12)&0x0F] );
    val<<=4;
  }
}*/
