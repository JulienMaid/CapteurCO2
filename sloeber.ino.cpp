#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2026-06-20 16:47:02

#include "Arduino.h"
#include <EEPROM.h>
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MsTimer2.h>
#include <Adafruit_NeoPixel.h>
#include "timer_sw.h"
#include "DefinitionES.h"
#include "gestion_Led_WS.h"
#include "convertAnalogValue.h"
#include "GestionClignotementLed.h"
#include "GestionSonBuzzer.h"
#include "GestionClignotementLedWS.h"
#include "tools.h"

void setup() ;
void loop() ;
void GestionTimningBP(uint32_t p_u32_arg, void* p_v_arg) ;
void acquerir() ;
void acquerir_afficher() ;

#include "CapteurCO2.ino"


#endif
