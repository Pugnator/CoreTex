#pragma once
#include <global.hpp>

#define GSM_DEFAULT_TIMEOUT 30000

class Modem
{
public:
	Modem ( uart::Uart& p, uart::Uart* d = NULL );
	bool rawcmd ( char* command, char* expectedResult, bool partial=false );
	bool smsw ( char* number, char* text );
	int sigstr (void);
private:
	void init ( void );
	void reset_buf ( void );
	uart::Uart& port;
	uart::Uart* debug;
	char max_retry_count;
	int32_t last_result;
};
