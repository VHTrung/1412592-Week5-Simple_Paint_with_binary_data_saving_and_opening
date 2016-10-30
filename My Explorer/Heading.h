#pragma once
#include "stdafx.h"
#define MAX_HEADING_LEN 256
typedef struct
{
	WCHAR tchHeading[MAX_HEADING_LEN];
	int tchLevel;
	int nImages;
} Heading;
