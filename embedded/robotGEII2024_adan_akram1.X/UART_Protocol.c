
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"

unsigned char UartCalculateChecksum(int msgFunction,int msgPayloadLength, unsigned char* payload)
{
 char c = 0;
 c ^= 0xFE;
 c ^= (char)(msgFunction >> 8);
 c ^= (char)(msgFunction);
 c ^= (char)(msgPayloadLength >> 8);
 c ^= (char)(msgPayloadLength);
    for (unsigned int i = 0; i < msgPayloadLength; i++) {
        c ^= payload[i];
    }
 return c;

}
void UartEncodeAndSendMessage(int msgFunction,int msgPayloadLength, unsigned char* payload)
{
 unsigned char message [6 + msgPayloadLength];
 unsigned int pos = 0;
 message[pos++] = 0xFE;
 message[pos++] = (char)(msgFunction >> 8);
 message[pos++] = (char)(msgFunction);
 message[pos++] = (char)(msgPayloadLength >> 8);
 message[pos++] = (char)(msgPayloadLength);
 for (int i = 0; i < msgPayloadLength; i++)
 {
     message[pos++] = payload[i];
 }
 char c = UartCalculateChecksum(msgFunction, msgPayloadLength, payload);
 message[pos++] = c;
 SendMessage(message, 0, pos);
}

/*
int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;
void UartDecodeMessage(unsigned char c)
{
//Fonction prenant en entree un octet et servant a reconstituer les trames


}
void UartProcessDecodedMessage(int function,
int payloadLength, unsigned char* payload)
{
//Fonction appelee apres le decodage pour executer l?action
//correspondant au message recu

}

*/