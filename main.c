#include <xc.h>
#include <stdio.h>
#include "uart.h"
#include "test.h"

/**
 * Bits de configuration:
 */
#pragma config FOSC = INTIO67  // Osc. interne, A6 et A7 comme IO.
#pragma config IESO = OFF      // Pas d'osc. au démarrage.
#pragma config FCMEN = OFF     // Pas de monitorage de l'oscillateur.

// Nécessaires pour ICSP / ICD:
#pragma config MCLRE = EXTMCLR // RE3 est actif comme master reset.
#pragma config WDTEN = OFF     // Watchdog inactif.
#pragma config LVP = OFF       // Single Supply Enable bits off.

#ifndef TEST

void interrupt low_priority interruptionsBassePriorite() {
    if (PIR1bits.TX1IF) {
        if (uartCaracteresDisponiblesPourTransmission()) {
            TXREG1 = uartTransmission();
        }
        PIR1bits.TX1IF = 0;
    }
}

/**
 * Initialise la sortie 1 de la EUSART.
 * Si le UP est à 1MHz, le Virtual Terminal dans Proteus
 * doit être configuré comme suit:
 * - 1200 bauds.
 * - Transmission 8 bits.
 * - Bit de stop activé.
 */
void initialiseHardware() {
    // Pour une fréquence de 1MHz, ceci donne 1200 bauds:
    SPBRG = 12;
    SPBRGH = 0;

    // Configure RC6 et RC7 comme entrées digitales, pour que
    // la EUSART puisse en prendre le contrôle:
    TRISCbits.RC6 = 1;
    TRISCbits.RC7 = 1;
    
    // Configure la EUSART:
    // (BRGH et BRG16 sont à leur valeurs par défaut)
    // (TX9 est à sa valeur par défaut)
    RCSTAbits.SPEN = 1;  // Active la EUSART.
    TXSTAbits.SYNC = 0;  // Mode asynchrone.
    TXSTAbits.TXEN = 1;  // Active l'émetteur.
    
    // Active les interruptions (basse priorité):
    PIE1bits.TX1IE = 1;
    IPR1bits.TX1IP = 0;
    PIE1bits.RC1IE = 1;
    IPR1bits.RC1IP = 0;
    
    // Active les interruptions générales:
    RCONbits.IPEN = 1;
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;
}

/**
 * Point d'entrée du programme.
 */
void main(void) {
    unsigned char n;
    
    initialiseHardware();
    uartReinitialise();

    for (n = 0; n < 100; n++) {
        printf("Hello world: %d\r\n", n);
    }
}
#endif

#ifdef TEST
void main() {
    initialiseTests();
    testUart();
    finaliseTests();
    while(1);
}
#endif