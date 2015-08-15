#include <global.hpp>
using namespace uart;

uint32_t gsmtimeout = 0;


/* very important for correct SMS handling and sending
  "+CMGF=1", 1,                // PDU mode off
  "+CSCS=\"UCS2\"", 1,          
  "+CSMP=17,167,0,8", 1,       // format to UCS2 16bit(8)

  //modem_write_c( 0x1A ); //Ctrl+Z
  */

Modem::Modem(Uart& p)
: port(p)
{
  memset(response, 0, sizeof response);
  last_result = 0;
  max_retry_count = 1;
  init();
}

bool Modem::cmd (char *command, bool partial)
{  
  if(partial)
  {
    port << command;  
    return true;
  }
  reset_buf();
  port < command;    
  gsmtimeout = GSM_DEFAULT_TIMEOUT;
  while (!gsmResponseRcvd);  
  delay_ms(1500);
  return gsmtimeout ? true : false;
}

bool Modem::cmd (char *command, int timeout, bool partial)
{  
  if(partial)
  {
    port << command;  
    return true;
  }   
  reset_buf();
  port < command;    
  gsmtimeout = timeout;  
  while (!gsmResponseRcvd);  
  return gsmtimeout ? true : false;
}

void Modem::init (void)
{
  memset(gsmResponse, 0, sizeof gsmResponse);
  gsmResponsePnt = gsmResponse;
  PIN_LOW ( GSMDTR );
  cmd ("ATE0");  
  cmd ("AT+CMGF=1"); 
}

bool Modem::smsw (char *number, char *text)
{  
  cmd("AT+CMGS=\"", true);  
  cmd(number, true);  
  cmd("\"");  
  cmd(text);  
  port < (char)26;
  return true;
}

void Modem::reset_buf (void)
{
  gsmResponseRcvd = false;
  gsmResponsePnt = gsmResponse;
  memset(gsmResponse, 0, sizeof gsmResponse);
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
