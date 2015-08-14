#include <global.hpp>
using namespace uart;
uart::Stack gsmData;
/* very important for correct SMS handling and sending
  "+CMGF=1", 1,                // PDU mode off
  "+CSCS=\"UCS2\"", 1,          
  "+CSMP=17,167,0,8", 1,       // format to UCS2 16bit(8)

  //modem_write_c( 0x1A ); //Ctrl+Z
  */

void prepareGSM (Uart& port)
{
	port < "ATE0"; 
  delay_ms(1000);
  port < "AT+CMGF=1";  
  delay_ms(1000);
}

void sendSMS (Uart& port, char *number, char *text)
{  
  port << "!AT+CMGS=\"";
  port << number;  
  port < "\"";
  delay_ms(1000);     
  port < text;   
  delay_ms(1000);     
  port < (char)26;   
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
