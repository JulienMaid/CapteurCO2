/*
 * tools.cpp
 *
 *  Created on: 18 juin 2026
 *      Author: julien
 */

#include "tools.h"
#include <Arduino.h>
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
extern SCD4x g_t_CapteurSCD41;
extern Adafruit_SSD1306 g_t_EcranLCD;
extern TimerEvent_t g_t_TimerTempoMesure;
extern TimerEvent_t g_t_TimerGestionGenerale;


extern ConvertAnalogValue TensionBatterie;

extern VariableTracee<mode_operation_t> g_t_EtatEnEcours;
extern VariableTracee<uint16_t> g_t_ModeAlarme;


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

  g_t_ClignotementLedWS->Demarrer();

  Serial.println("Mode ON");
}

void Mode_OFF(void)
{
  digitalWrite(CMD_ONOFF, 0);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Red, true);

  g_t_ClignotementLedWS->Arreter();

  Serial.println("Mode OFF");
}

void Mode_Stop_Alarme(void)
{

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
  qualite_air_t l_e_Qualite_Air = Acceptable;
  static qualite_air_t l_e_Qualite_Air_Prec = Acceptable;

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
  g_t_EcranLCD.print("CO2");
  g_t_EcranLCD.display();

  l_e_Qualite_Air = Determiner_Qualite_Air(taux_pourcent);

  switch(l_e_Qualite_Air)
  {
  case Acceptable:
    g_t_ClignotementLedWS->ReglerLuminosite(64);
    g_t_ClignotementLedWS->SetSequence(1);

    if(g_t_ModeAlarme.LireValeur() != alarme_fin_TempsON)
    {
      g_t_ModeAlarme.EcrireValeur(alarme_off);
    }
    break;

  case Mediocre:
    g_t_ClignotementLedWS->ReglerLuminosite(64);
    g_t_ClignotementLedWS->SetSequence(2);

    if(g_t_ModeAlarme.LireValeur() != alarme_fin_TempsON)
    {
      g_t_ModeAlarme.EcrireValeur(alarme_off);
    }
    break;

  case Mauvaise:
    g_t_ClignotementLedWS->ReglerLuminosite(128);
    g_t_ClignotementLedWS->SetSequence(3);

    if(l_e_Qualite_Air_Prec != l_e_Qualite_Air)
    {
      g_t_ModeAlarme.EcrireValeur(alarme_attention);
    }
    break;

  case Tres_Mauvaise:
    g_t_ClignotementLedWS->ReglerLuminosite(200);
    g_t_ClignotementLedWS->SetSequence(4);

    if(l_e_Qualite_Air_Prec != l_e_Qualite_Air)
    {
      g_t_ModeAlarme.EcrireValeur(alarme_attention);
    }
    break;

  case Danger:
    g_t_ClignotementLedWS->ReglerLuminosite(200);
    g_t_ClignotementLedWS->SetSequence(5);

    if(l_e_Qualite_Air_Prec != l_e_Qualite_Air)
    {
      g_t_ModeAlarme.EcrireValeur(alarme_alerte);
    }
    break;

  }

  l_e_Qualite_Air_Prec = l_e_Qualite_Air;

}

///////F:fonction qui génère l'icone état piles////////////////////////////////////////////////////////////////
void Icone_Etat_Piles(uint8_t p_u8_Valeur)
{
  g_t_EcranLCD.drawRoundRect(108,22, 20, 10,3, WHITE);
  g_t_EcranLCD.fillRect(110,24, 16, 6, WHITE);
  g_t_EcranLCD.fillRect(110,24,p_u8_Valeur,6,BLACK);
}

///////F:fonction qui génère le symbole du mode en cours////////////////////////////////////////////////////////////////
void Symbole_Mode_En_Cours(mode_operation_t p_e_ModeEnCours)
{
  g_t_EcranLCD.drawRoundRect(90,20, 14, 12,6, WHITE);
  g_t_EcranLCD.setCursor(94, 22);
  g_t_EcranLCD.setTextSize(1);
  g_t_EcranLCD.setTextColor(WHITE);

  switch(p_e_ModeEnCours)
  {
  case mode_normal:
    g_t_EcranLCD.println("N");
    break;
  case mode_continu:
    g_t_EcranLCD.println("C");
    break;
  default:
    g_t_EcranLCD.println("?");
    break;
  }
}

void Afficher_Temps_ON(uint8_t p_u8_MinutesON)
{
  for(auto index=0; index<15; index++)
  {
    if(index<p_u8_MinutesON)
    {
      g_t_EcranLCD.fillRect(1+5*index,24, 4, 8, WHITE);
    }
    else
    {
      g_t_EcranLCD.drawRoundRect(1+5*index,24, 4, 8,1, WHITE);
    }
  }
}

uint8_t Tester_Batterie(void)
{
  uint16_t l_u16_TensionBatterieInt = 0;
  uint8_t l_u8_NiveauBatterie = 0;

  l_u16_TensionBatterieInt = ((uint16_t)10*(uint16_t)analogRead(ANALOG_BATTERIE))/102;

  if(l_u16_TensionBatterieInt < 65)
  {
    // Extinction immédiate
#if DEBUG != 1
    g_t_EtatEnEcours.EcrireValeur(mode_extinction);
#endif
    l_u8_NiveauBatterie = 16;
  }
  else if(l_u16_TensionBatterieInt < 67)
  {
    l_u8_NiveauBatterie = 12;
    g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Red, true);
    g_t_ModeAlarme.EcrireValeur(alarme_alerte);
  }
  else if(l_u16_TensionBatterieInt < 69)
  {
    l_u8_NiveauBatterie = 12;
  }
  else if(l_u16_TensionBatterieInt < 73)
  {
    l_u8_NiveauBatterie = 8;
  }
  else if(l_u16_TensionBatterieInt < 78)
  {
    l_u8_NiveauBatterie = 4;
  }
  else
  {
    l_u8_NiveauBatterie = 0;
  }

  return l_u8_NiveauBatterie;
}
