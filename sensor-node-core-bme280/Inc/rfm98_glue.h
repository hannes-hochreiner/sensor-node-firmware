#ifndef __rfm98_glue_h
#define __rfm98_glue_h

#include "main.h"
#include "rfm9x.h"

void RFM98Glue_Init(rfm9x_t* const rfm98);
void RFM98Glue_Set_Reset_Pin();
void RFM98Glue_Reset_Reset_Pin();
void RFM98Glue_Set_NSS_Pin();
void RFM98Glue_Reset_NSS_Pin();
void RFM98Glue_SPI_Transfer(uint8_t* const data);
void RFM98Glue_Delay(uint8_t millisec);

#endif
