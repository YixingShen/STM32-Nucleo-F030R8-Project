#ifndef __ADC_H
#define __ADC_H

void ADC_Calibration(void);

void ADC_enable(void);
void ADC_disable(void);

void ADC_Clock_selection(void);
void ADC_configures(void);

void ADC_startConversion(void);
uint16_t ADC_readDR(void);

#endif
