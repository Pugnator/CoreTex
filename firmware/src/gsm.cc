#include <global.hpp>
using namespace uart;
/* very important for correct SMS handling and sending
  "+CMGF=1", 1,                // PDU mode off
  "+CSCS=\"UCS2\"", 1,          
  "+CSMP=17,167,0,8", 1,       // format to UCS2 16bit(8)

  //modem_write_c( 0x1A ); //Ctrl+Z
  */

void prepareGSM (Uart port)
{
	port < "+CMGF=1";
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
