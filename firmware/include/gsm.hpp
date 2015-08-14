#pragma once
#include <global.hpp>
using namespace uart;

void prepareGSM (Uart& port);
void sendSMS (Uart& port, char *number, char *text);
extern int gsmerr;
