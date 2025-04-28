<<<<<<< HEAD
/* 
 * File:   QEI.c
 * Author: E306-PC7
 *
 * Created on 22 avril 2025, 15:25
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * 
 */
int main(int argc, char** argv) {

    return (EXIT_SUCCESS);
}

=======
#include "QEI.h"
#include "timer.h"
#include "IO.h"
#include "UART_Protocol.h"
#include "UART.h"
#include <math.h>
#include "Utilities.h"
#include "robot.h"
#include <xc.h>

#define DISTROUES 0.2812
#define FREQ_ECH_QEI 250

static float QeiDroitPosition_T_1 = 0.0;
static float QeiDroitPosition = 0.0;
static float QeiGauchePosition_T_1 = 0.0;
static float QeiGauchePosition = 0.0;
static float delta_d = 0.0;
static float delta_g = 0.0;

void InitQEI1() {
    QEI1IOCbits.SWPAB = 1; //QEAx and QEBx are swapped
    QEI1GECL = 0xFFFF;
    QEI1GECH = 0xFFFF;
    QEI1CONbits.QEIEN = 1; // Enable QEI Module
}

void InitQEI2() {
    QEI2IOCbits.SWPAB = 1; //QEAx and QEBx are not swapped
    QEI2GECL = 0xFFFF;
    QEI2GECH = 0xFFFF;
    QEI2CONbits.QEIEN = 1; // Enable QEI Module
}

void QEIUpdateData() {
    //On sauvegarde les anciennes valeurs
    QeiDroitPosition_T_1 = QeiDroitPosition;
    QeiGauchePosition_T_1 = QeiGauchePosition;

    //On actualise les valeurs des positions
    long QEI1RawValue = POS1CNTL;
    QEI1RawValue += ((long) POS1HLD << 16);
    long QEI2RawValue = POS2CNTL;
    QEI2RawValue += ((long) POS2HLD << 16);

    //Conversion en mm (regle pour la taille des roues codeuses)
    QeiDroitPosition = 0.00001620 * QEI1RawValue;
    QeiGauchePosition = -0.00001620 * QEI2RawValue;

    //Calcul des deltas de position
    delta_d = QeiDroitPosition - QeiDroitPosition_T_1;
    delta_g = QeiGauchePosition - QeiGauchePosition_T_1;

    //Calcul des vitesses
    //attention a remultiplier par la frequence d echantillonnage
    robotState.vitesseDroitFromOdometry = delta_d*FREQ_ECH_QEI;
    robotState.vitesseGaucheFromOdometry = delta_g*FREQ_ECH_QEI;
    robotState.vitesseLineaireFromOdometry = (robotState.vitesseDroitFromOdometry + robotState.vitesseGaucheFromOdometry) / 2.0;
    robotState.vitesseAngulaireFromOdometry = (robotState.vitesseDroitFromOdometry - robotState.vitesseGaucheFromOdometry) / DISTROUES;

    //Mise a jour du positionnement terrain a t-1
    robotState.xPosFromOdometry_1 = robotState.xPosFromOdometry;
    robotState.yPosFromOdometry_1 = robotState.yPosFromOdometry;
    robotState.angleRadianFromOdometry_1 = robotState.angleRadianFromOdometry;

    //Calcul des positions dans le referentiel du terrain
    robotState.xPosFromOdometry = (robotState.vitesseLineaireFromOdometry * cos( robotState.angleRadianFromOdometry))*FREQ_ECH_QEI;
    robotState.yPosFromOdometry = (robotState.vitesseLineaireFromOdometry * sin( robotState.angleRadianFromOdometry))*FREQ_ECH_QEI;
    robotState.angleRadianFromOdometry = robotState.vitesseAngulaireFromOdometry / FREQ_ECH_QEI;
    if (robotState.angleRadianFromOdometry > PI)
        robotState.angleRadianFromOdometry -= 2 * PI;
    if (robotState.angleRadianFromOdometry < -PI)
        robotState.angleRadianFromOdometry += 2 * PI;
}

#define POSITION_DATA 0x0061
void SendPositionData()
{
unsigned char positionPayload[24];
getBytesFromInt32(positionPayload, 0, timestamp);
getBytesFromFloat(positionPayload, 4, (float)(robotState.xPosFromOdometry));
getBytesFromFloat(positionPayload, 8, (float)(robotState.yPosFromOdometry));
getBytesFromFloat(positionPayload, 12, (float)(robotState.angleRadianFromOdometry));
getBytesFromFloat(positionPayload, 16, (float)(robotState.vitesseLineaireFromOdometry));
getBytesFromFloat(positionPayload, 20, (float)(robotState.vitesseAngulaireFromOdometry));
UartEncodeAndSendMessage(POSITION_DATA, 24, positionPayload);
}
>>>>>>> 23771b5f426a252b99bc62a6ebc6b9456f10e45c
