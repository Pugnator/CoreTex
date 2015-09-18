#pragma once
#include <global.hpp>

#define GSM_DEFAULT_TIMEOUT 5000

class Modem
{
public:
	Modem ( uart::Uart& p, uart::Uart* d = NULL );
	bool rawcmd ( char* command, char* expectedResult, bool partial=false );
	bool smsw ( char* number, char* text );
	int signal_level ( void );
	bool ready ( void );
private:
	void init ( void );
	void reset_buf ( void );
	uart::Uart& port;
	uart::Uart* debug;
	char max_retry_count;
	int32_t last_result;
	/* URC events */
	bool ring;
	bool powerDown;
	bool callReady;
};
