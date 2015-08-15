#pragma once
#include <global.hpp>

#define GSM_DEFAULT_TIMEOUT 10000

class Modem
{
public:
	Modem ( uart::Uart& p );
	bool cmd(char *command, bool partial=false);
	bool cmd (char *command, int timeout, bool partial= false);
	bool smsw (char *number, char *text);
private:
	void init (void);
	void reset_buf (void);
	uart::Uart& port;
	char max_retry_count;
	int32_t last_result;
	char response[32];
};

extern int gsmerr;
extern char gsmResponse[16];
extern char *gsmResponsePnt;
extern bool gsmResponseRcvd;
