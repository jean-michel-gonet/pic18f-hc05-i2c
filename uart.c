#include <xc.h>
#include <stdio.h>
#include "file.h"
#include "test.h"
#include "uart.h"

#ifdef TEST
char dernierCaractereTransmis = 0;
void demarreTransmissionUart(char data) {
    dernierCaractereTransmis = data;
}
#else
void demarreTransmissionUart(char data) {
    TXREG1 = data;
}
#endif

/**
 * File des caractères reçus.
 * La file est peuplée par {@link #uartReception} et
 * vidée par {@link #uartGetCh}.
 */
File fileReception;

/**
 * File des caractères à transmettre.
 * La file est peuplée par {@link #uartPutch} et
 * vidée par {@link uartTransmission}.
 */
File fileTransmission;

typedef enum {
    UART_TRANSMISSION_REPOS,
    UART_TRANSMISSION_EN_COURS
} UartStatus;

/**
 * État actuel de la transmission.
 */
UartStatus uartEtat = UART_TRANSMISSION_REPOS;

/**
 * Ajoute un caractère à la file de transmission.
 * Si la file de transmission est pleine, la méthode attend qu'il
 * y ait de la place.
 * @param data Le caractère à placer.
 */
void uartPutch(char data) {
    if (uartEtat == UART_TRANSMISSION_REPOS) {
        demarreTransmissionUart(data);
        uartEtat = UART_TRANSMISSION_EN_COURS;
    } else {
        while (fileEstPleine(&fileTransmission));
        fileEnfile(&fileTransmission, data);    
    }
}

/**
 * Récupère un caractère de la file de réception.
 * Si la file de réception est vide, la méthode attend qu'un
 * caractère soit disponible.
 * @return Un caractère de la file de réception
 */
char uartGetch() {
    return fileDefile(&fileReception);    
}

/**
 * Fonction à appeler chaque fois que la UART reçoit un caractère.
 * Cette fonction est normalement appelée depuis la routine de gestion
 * des interruptions, en réponse à une interruption de réception.
 * <pre>
 * void interrupt interruptions() {
 *  if (PIR1bits.RC1IF) {
 *      uartReception(T1REG);
 *      PIR1bits.RC1IF = 0;
 *  }
 * }
 * </pre>
 * @param c Le caractère reçu.
 */
void uartReception(unsigned char c) {
    fileEnfile(&fileReception, c);
}

/**
 * Indique qu'il n'y a plus de caractères à transmettre.
 * @return 255 si il n'y a plus de caractères à transmettre.
 * @see uartTransmission
 */
unsigned char uartCaracteresDisponiblesPourTransmission() {
    if (fileEstVide(&fileTransmission)) {
        uartEtat = UART_TRANSMISSION_REPOS;
        return 0;
    } else {
        return 255;
    }
}

/**
 * Fonction à appeler pour récupérer le prochain caractère à transmettre
 * à travers la UART.
 * Cette fonction est typiquement appelée depuis la routine de transmission
 * des interruptions, en réponse à une interruption de transmission.
 * <pre>
 * void interrupt interruptions() {
 *  if (IPR1bits.TX1IF) {
 *      if (uartPasDeCaracteresATransmettre()) {
 *          T1REG = uartTransmission();
 *      }
 *  }
 * }
 * </pre>
 * @return Le caractère à envoyer.
 */
unsigned char uartTransmission() {
    return fileDefile(&fileTransmission);
}

/**
 * Réinitialise la UART.
 */
void uartReinitialise() {
    fileReinitialise(&fileReception);
    fileReinitialise(&fileTransmission);
    uartEtat = UART_TRANSMISSION_REPOS;
}

#ifndef TEST
/**
 * Implémentation d'une fonction système qui est appelée par <code>printf</code>.
 * Cette implémentation ajoute le caractère à la file de transmission.
 * @param data Le code ASCII du caractère à afficher.
 */
void putch(char data) {
    uartPutch(data);
}

/**
 * Implémentation d'une fonction système qui est appelée par <code>scanf</code>, 
 * <code>getc</code>, <code>getchar</code>, etc.
 * Cette implémentation récupère le caractère à la file de réception.
 * @param data Le code ASCII du récupéré.
 */
char getche() {
    return uartGetch();
}

/**
 * Implémentation d'une fonction système qui est appelée par <code>scanf</code>, 
 * <code>getc</code>, <code>getchar</code>, etc.
 * Cette implémentation récupère le caractère à la file de réception.
 * @param data Le code ASCII du récupéré.
 */
char getch() {
    return uartGetch();
}
#endif

#ifdef TEST

void testAT() {
    uartPutch('A');
    uartPutch('T');
    testeEgaliteChars  ("UPAT00", uartEtat, UART_TRANSMISSION_EN_COURS);
    testeEgaliteChars  ("UPAT01", dernierCaractereTransmis, 'A');
    testeEgaliteChars  ("UPAT02", uartTransmission(), 'T');
    testeEgaliteEntiers("UPAT03", uartCaracteresDisponiblesPourTransmission(), 0);
}

void testUart() {
    testAT();
}
#endif 