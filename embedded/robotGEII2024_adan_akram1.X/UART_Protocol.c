
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "UART_Protocol.h"
#include "CB_TX1.h"
#include "robot.h"
#include "ADC.h"

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* payload) {
    unsigned char c = 0;
    c ^= 0xFE;
    c ^= (char) (msgFunction >> 8);
    c ^= (char) (msgFunction);
    c ^= (char) (msgPayloadLength >> 8);
    c ^= (char) (msgPayloadLength);
    for (int i = 0; i < msgPayloadLength; i++) {
        c ^= payload[i];
    }
    return c;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* payload) {
    unsigned char message [6 + msgPayloadLength];
    int pos = 0;
    message[pos++] = 0xFE;
    message[pos++] = (unsigned char) (msgFunction >> 8);
    message[pos++] = (unsigned char) (msgFunction);
    message[pos++] = (unsigned char) (msgPayloadLength >> 8);
    message[pos++] = (unsigned char) (msgPayloadLength);
    for (int i = 0; i < msgPayloadLength; i++) {
        message[pos++] = payload[i];
    }
    char c = UartCalculateChecksum(msgFunction, msgPayloadLength, payload);
    message[pos++] = c;
    SendMessage(message, pos);
}

int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

StateReception rcvState = Waiting;

void UartDecodeMessage(unsigned char c) {
    unsigned char receivedChecksum;
    unsigned char calculatedChecksum;
    switch (rcvState) {
        case Waiting:
            if (c == 0xFE) {
                rcvState = FunctionMSB;
            }
            break;
        case FunctionMSB:
            msgDecodedFunction = c << 8;
            rcvState = FunctionLSB;
            break;
        case FunctionLSB:
            msgDecodedFunction |= c;
            rcvState = PayloadLengthMSB;
            break;
        case PayloadLengthMSB:
            msgDecodedPayloadLength = c << 8;
            rcvState = PayloadLengthLSB;
            break;
        case PayloadLengthLSB:
            msgDecodedPayloadLength |= c;
            if (msgDecodedPayloadLength > 128) {
                rcvState = Waiting; // Payload trop grand
            } else if (msgDecodedPayloadLength > 0) {
                msgDecodedPayloadIndex = 0;
                rcvState = Payload;
            } else {
                rcvState = CheckSum;
            }
            break;
        case Payload:
            msgDecodedPayload[msgDecodedPayloadIndex++] = c;

            if (msgDecodedPayloadIndex >= msgDecodedPayloadLength) {
                rcvState = CheckSum;
            }
            break;
        case CheckSum:
            receivedChecksum = c;
            calculatedChecksum = UartCalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            if (receivedChecksum == calculatedChecksum) {
                UartProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
            }
            rcvState = Waiting;
            break;
        default:
            rcvState = Waiting;
            break;
    }
}

void UartProcessDecodedMessage(int function, int payloadLength, unsigned char* payload) {
    switch (function) {
        case 0x0030:
            if (payloadLength >= 3) {
                robotState.distanceTelemetreExGauche = payload [0];
                robotState.distanceTelemetreGauche = payload [1];
                robotState.distanceTelemetreCentre = payload [2];
                robotState.distanceTelemetreDroit = payload [3];
                robotState.distanceTelemetreExDroite = payload [4];
            }
            break;
            /*case 0x0040:
            
                break;*/
    }
}
