#ifndef UART__H
#define UART__H

void uartReception(unsigned char c);
unsigned char uartCaracteresDisponiblesPourTransmission();
unsigned char uartTransmission();
void uartReinitialise();


#ifdef TEST
void testUart();
#endif

#endif