#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "CB_TX1.h"

#define CBTX1_BUFFER_SIZE 128

// Déclaration des variables pour le buffer circulaire
int cbTx1Head = 0;
int cbTx1Tail = 0;
unsigned char cbTx1Buffer[CBTX1_BUFFER_SIZE];
unsigned char isTransmitting = 0;  // Flag pour vérifier si une transmission est en cours

// Fonction pour envoyer un message
void SendMessage(unsigned char* message, int length) {
    unsigned char i = 0;
    if (CB_TX1_GetRemainingSize() > length) {
        // On peut écrire le message
        for (i = 0; i < length; i++) {
            CB_TX1_Add(message[i]);
        }
        // Si une transmission n'est pas déjà en cours, commencer la transmission
        if (!CB_TX1_IsTranmitting()) {
            SendOne();
        }
    }
}

// Ajouter un caractère dans le buffer circulaire
void CB_TX1_Add(unsigned char value) {
    int next = (cbTx1Tail + 1) % CBTX1_BUFFER_SIZE;  // Calcul de l'indice suivant de manière circulaire
    if (next != cbTx1Head) {  // Vérifier si le buffer n'est pas plein
        cbTx1Buffer[cbTx1Tail] = value;  // Ajouter l'élément dans le buffer
        cbTx1Tail = next;  // Mettre à jour la position du tail
    }
}

// Récupérer un caractère depuis le buffer circulaire
unsigned char CB_TX1_Get(void) {
    unsigned char value = cbTx1Buffer[cbTx1Head];  // Lire la valeur à la position head
    cbTx1Head = (cbTx1Head + 1) % CBTX1_BUFFER_SIZE;  // Mettre à jour head de manière circulaire
    return value;
}

// Routine d'interruption UART pour la transmission
void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;  // Effacer le flag d'interruption TX
    if (cbTx1Tail != cbTx1Head) {
        // Il y a encore des caractères à transmettre
        SendOne();
    } else {
        isTransmitting = 0;  // Aucune transmission en cours
    }
}

// Fonction pour envoyer un seul caractère via UART
void SendOne(void) {
    isTransmitting = 1;  // Indiquer qu'une transmission est en cours
    unsigned char value = CB_TX1_Get();  // Récupérer un caractère depuis le buffer circulaire
    U1TXREG = value;  // Transmettre le caractère via UART
}

// Vérifie si une transmission est en cours
unsigned char CB_TX1_IsTranmitting(void) {
    return isTransmitting;
}

// Récupère la taille des données stockées dans le buffer circulaire
int CB_TX1_GetDataSize(void) {
    int dataSize = (cbTx1Tail - cbTx1Head + CBTX1_BUFFER_SIZE) % CBTX1_BUFFER_SIZE;
    return dataSize;
}

// Récupère la taille de l'espace libre restant dans le buffer circulaire
int CB_TX1_GetRemainingSize(void) {
    int remainingSize = (CBTX1_BUFFER_SIZE - CB_TX1_GetDataSize() - 1) % CBTX1_BUFFER_SIZE;
    return remainingSize;
}
