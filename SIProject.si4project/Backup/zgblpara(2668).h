

#ifndef _ZGBLPARA_H__
#define _ZGBLPARA_H__
#include <stdio.h>

//the distance value measured by LaserDistanceMeasure module.
extern uint32_t g_LaserDistance;

//crc32 check.
extern uint32_t g_CRC32Table[];
extern uint32_t g_CRC32Calculate(uint8_t* s, int len);

#endif

