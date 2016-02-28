#pragma once

#define BO_CLIENT_VERSION 0x01000000 //1.0.0.0

#define BO_DEBUG 1
#define MAX_SIZE_RELOCATION_DATA 0

#include "crypt.h"
typedef struct
{
	Crypt::RC4KEY baseKey;
}BASECONFIG;