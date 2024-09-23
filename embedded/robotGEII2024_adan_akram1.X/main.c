/* 
 * File:   main.c
 * Author: Table 9
 *
 * Created on 11 septembre 2024, 14:06
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "robot.h"
#include "ToolBox.h"
#include "ADC.h"
#include "main.h"

























































int main(void) {
    /***********************************************************************************************/
    //Initialisation oscillateur
    /***********************************************************************************************/
    InitOscillator();
    /***********************************************************************************************/
    //Configuration des input et output (IO)
    /***********************************************************************************************/
    InitIO();


    InitTimer1();
    InitTimer4();
    InitTimer23();
    InitPWM();
    InitADC1();

    /*LED_BLANCHE_1 = 1;
    LED_BLEUE_1 = 1;
    LED_ORANGE_1 = 1;
    LED_ROUGE_1 = 1;
    LED_VERTE_1 = 1;

    LED_BLANCHE_2 = 1;
    LED_BLEUE_2 = 1;
    LED_ORANGE_2 = 1;
    LED_ROUGE_2 = 1;
    LED_VERTE_2 = 1;*/

    /*******************   Boucle Principale   ******************************************************/
    while (1) {
        /*if (ADCIsConversionFinished()) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            ADCValue0 = result[0];
            ADCValue1 = result[1];
            ADCValue2 = result[2];
            ADCValue3 = result[3];
            ADCValue4 = result[4];
        }*/
        if (ADCIsConversionFinished() == 1) {
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            float volts = ((float) result [0])* 3.3 / 4096;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            volts = ((float) result [1])* 3.3 / 4096;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            volts = ((float) result [2])* 3.3 / 4096;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            volts = ((float) result [3])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeDroite = 34 / volts - 5;
            volts = ((float) result [4])* 3.3 / 4096;
            robotState.distanceTelemetreExtremeGauche = 34 / volts - 5;
        }
        if (robotState.distanceTelemetreCentre < 30) {
            LED_ORANGE_1 = 1;
        } else {
            LED_ORANGE_1 = 0;
        }
        if (robotState.distanceTelemetreGauche < 30) {
            LED_BLEUE_1 = 1;
        } else {
            LED_BLEUE_1 = 0;
        }
        if (robotState.distanceTelemetreDroit < 30) {
            LED_ROUGE_1 = 1;
        } else {
            LED_ROUGE_1 = 0;
        }
        if (robotState.distanceTelemetreExtremeDroite < 30) {
            LED_VERTE_1 = 1;
        } else {
            LED_VERTE_1 = 0;
        }
        if (robotState.distanceTelemetreExtremeGauche < 30) {
            LED_BLANCHE_1 = 1;
        } else {
            LED_BLANCHE_1 = 0;
        }
    } // fin main

}

