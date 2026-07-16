/*
 * tools.cpp
 *
 *  Created on: 18 juin 2026
 *      Author: julien
 */

#include "tools.h"
#include <Arduino.h>
//#include "GestionClignotementLed.h"
#include "GestionSonBuzzer.h"
#include "GestionClignotementLedWS.h"
#include "timer_sw.h"
#include "DefinitionES.h"
#include "gestion_Led_WS.h"
#include "SparkFun_SCD4x_Arduino_Library.h" // pour le captur SCD41
#include <Adafruit_GFX.h> // pour les graphiques
#include <Adafruit_SSD1306.h> // pour le circuit de commande de l'afficheur
#include <Adafruit_NeoPixel.h>
#include "convertAnalogValue.h"
#include "DefinitionES.h"
#include "VariableTracee.h"


extern GestionLedWS_t * g_t_GestionMultiLedWS;
extern GestionClignotementLedWS * g_t_ClignotementLedWS;
//extern GestionClignotementLed g_t_ClignotementLedInterne;
extern GestionSonBuzzer g_t_GestionBuzzer;
extern SCD4x g_t_CapteurSCD41;
extern Adafruit_SSD1306 g_t_EcranLCD;
extern TimerEvent_t g_t_TimerTempoMesure;
extern TimerEvent_t g_t_TimerGestionGenerale;


extern ConvertAnalogValue TensionBatterie;

//extern mode_operation_t g_e_Etat_En_Cours;
extern boolean g_e_Alarme_En_Cours;

extern VariableTracee<mode_operation_t> g_t_Etat_En_Ecours;

qualite_air_t Determiner_Qualite_Air(const float & p_f_tauxCO2)
{
  qualite_air_t l_e_ValeurRetour = Danger;

  if(p_f_tauxCO2 < 0.18)
  {
    l_e_ValeurRetour = Acceptable;
  }
  else if(p_f_tauxCO2 < 0.5)
  {
    l_e_ValeurRetour = Mediocre;
  }
  else if(p_f_tauxCO2 < 2.0)
  {
    l_e_ValeurRetour = Mauvaise;
  }
  else if(p_f_tauxCO2 < 3.0)
  {
    l_e_ValeurRetour = Tres_Mauvaise;
  }
  else
  {
    l_e_ValeurRetour = Danger;
  }

  return l_e_ValeurRetour;
}

void Init_EntreesSorties(void)
{
  pinMode(CMD_BUZZER, OUTPUT);
  digitalWrite(CMD_BUZZER, 0);

  pinMode(CMD_ONOFF, OUTPUT);
  digitalWrite(CMD_ONOFF, 0);

  pinMode(CMD_LED_BOUTON, OUTPUT);
  digitalWrite(CMD_LED_BOUTON, 0);

}

void Mode_Normal_Debut(void)
{
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Green, true);
  digitalWrite(CMD_BUZZER, 1);
  delay(200);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);
  digitalWrite(CMD_BUZZER, 0);
}

void Mode_Normal(void)
{
  g_t_TimerTempoMesure.SetValue(5000);

  g_t_CapteurSCD41.stopPeriodicMeasurement();
  g_t_CapteurSCD41.startPeriodicMeasurement();

  Serial.println("Mode Normal");
}

void Mode_Continu(void)
{
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Orange, true);
  digitalWrite(CMD_BUZZER, 1);
  delay(200);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);
  digitalWrite(CMD_BUZZER, 0);

  delay(200);

  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Orange, true);
  digitalWrite(CMD_BUZZER, 1);
  delay(200);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);
  digitalWrite(CMD_BUZZER, 0);

  delay(200);

  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Orange, true);
  digitalWrite(CMD_BUZZER, 1);
  delay(200);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);
  digitalWrite(CMD_BUZZER, 0);

  g_t_ClignotementLedWS->ReglerIndexLed(1);

  g_t_TimerTempoMesure.SetValue(30000);

  g_t_CapteurSCD41.stopPeriodicMeasurement();
  g_t_CapteurSCD41.startLowPowerPeriodicMeasurement();

  Serial.println("Mode Continu");
}

void Mode_ON(void)
{
  digitalWrite(CMD_ONOFF, 1);

//  g_t_ClignotementLedInterne.Demarrer();
  g_t_ClignotementLedWS->Demarrer();
//  g_t_GestionBuzzer.Demarrer();

  Serial.println("Mode ON");
}

void Mode_OFF(void)
{
  digitalWrite(CMD_ONOFF, 0);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Red, true);

//  g_t_ClignotementLedInterne.Arreter();
  g_t_ClignotementLedWS->Arreter();
//  g_t_GestionBuzzer.Arreter();

  Serial.println("Mode OFF");
}

void Mode_Stop_Alarme(void)
{
//  g_t_GestionBuzzer.ClearSequence();
}

///////F:fonction faire la première mesure pour ne pas l'afficher car est à 0   /////////////////////////////////////
void acquerir()
{
  g_t_CapteurSCD41.getCO2();
  g_t_CapteurSCD41.getTemperature();
  g_t_CapteurSCD41.getHumidity();
}

///////F:fonction faire la mesure, l'afficher  //////////////////////////////////////////////
void acquerir_afficher()
{
  // variables taux de CO2
  unsigned int taux_co2; // sortie du capteur  en ppm
  float taux_pourcent = 0.0; // taux de CO2 en % en type float
  char taux_string[4];// taux de CO2 en type string pour l'afficheur
  qualite_air_t l_e_Qualite_Air;

  // acquerir taux de CO2
  taux_co2=(int)g_t_CapteurSCD41.getCO2();
  taux_pourcent=(float)taux_co2 /10000;
  dtostrf(taux_pourcent,4,2,taux_string);

  g_t_EcranLCD.clearDisplay();
  g_t_EcranLCD.setTextSize(3);
  g_t_EcranLCD.setTextColor(WHITE);
  g_t_EcranLCD.setCursor(0,0);
  g_t_EcranLCD.print(taux_string);
  g_t_EcranLCD.print("%");
  g_t_EcranLCD.setTextSize(2);
  g_t_EcranLCD.print("co2");
  g_t_EcranLCD.display();

  l_e_Qualite_Air = Determiner_Qualite_Air(taux_pourcent);

  switch(l_e_Qualite_Air)
  {
  case Acceptable:
    g_t_ClignotementLedWS->ReglerLuminosite(64);
    g_t_ClignotementLedWS->SetSequence(1);
    break;

  case Mediocre:
    g_t_ClignotementLedWS->ReglerLuminosite(64);
    g_t_ClignotementLedWS->SetSequence(2);
    break;

  case Mauvaise:
    g_t_ClignotementLedWS->ReglerLuminosite(128);
    g_t_ClignotementLedWS->SetSequence(3);
//    g_t_GestionBuzzer.SetSequence(2);
    break;

  case Tres_Mauvaise:
    g_t_ClignotementLedWS->ReglerLuminosite(200);
    g_t_ClignotementLedWS->SetSequence(4);
//    g_t_GestionBuzzer.SetSequence(3);
    break;

  case Danger:
    g_t_ClignotementLedWS->ReglerLuminosite(200);
    g_t_ClignotementLedWS->SetSequence(5);
//    g_t_GestionBuzzer.SetSequence(4);
    break;

  }

}
