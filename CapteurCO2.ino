#include <EEPROM.h>

#include <Wire.h> // pour la communication I2C
#include "SparkFun_SCD4x_Arduino_Library.h" // pour le captur SCD41
#include <Adafruit_GFX.h> // pour les graphiques
#include <Adafruit_SSD1306.h> // pour le circuit de commande de l'afficheur
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


#define PIN_WS2812B  4   // ESP32 pin that connects to WS2812B
#define NUM_PIXELS     1  // The number of LEDs (pixels) on WS2812B

Adafruit_NeoPixel g_t_LedWS2812(NUM_PIXELS, PIN_WS2812B, NEO_RGB + NEO_KHZ800);

GestionLedWS_t * g_t_GestionMultiLedWS;


// variables taux de CO2
unsigned int taux_co2; // sortie du capteur  en ppm
float taux_pourcent = 0.0; // taux de CO2 en % en type float
char taux_string[4];// taux de CO2 en type string pour l'afficheur

// définition objet moncapteur
SCD4x g_t_CapteurSCD41(SCD4x_SENSOR_SCD41);
// configuration afficheur OLED
Adafruit_SSD1306 g_t_EcranLCD(128,32, &Wire,-1 );//-1 pour signifier qu'aucune  des broches
// n'est utilisée pour initialiser l'affichage

TimerEvent_t g_t_TimerTempoMesure;
TimerEvent_t g_t_TimerGestionBP;


ConvertAnalogValue TensionBatterie(0, 0, 0.0, 10.0, 0, 1023);

GestionClignotementLed g_t_LedInterne(0);
GestionSonBuzzer g_t_GestionBuzzer(0);

GestionClignotementLedWS * LedWS;

void GestionTimningBP(uint32_t p_u32_arg, void* p_v_arg);

///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  MsTimer2 :: set (DEC_TIMESTAMP, Inc_Timer); //execution de la routine "alarme_cligno" toutes les 100ms
  MsTimer2:: start();

//  Init_Trace_Debug();
  g_t_TimerGestionBP.Init(GestionTimningBP, 50, Periodic_Timer, &g_t_TimerGestionBP);
//  g_t_TimerGestionBP.Start();

  g_t_GestionMultiLedWS = new GestionLedWS_t(NUM_PIXELS, PIN_WS2812B);

//  SEND_VTRACE(INFO, "SCD41");

  Wire.begin();// initialisation de la liaison I2C

  // Initialisation afficheur Oled 128 x 32
  if (!g_t_EcranLCD.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 erreur d'allocation "));
    for (;;);
  }
  else
  {
    Serial.println(F(" display detecté"));//
  }
  g_t_EcranLCD.clearDisplay();

  g_t_EcranLCD.setTextSize(2);
  g_t_EcranLCD.setTextColor(WHITE);
  g_t_EcranLCD.setCursor(0,0);
  g_t_EcranLCD.print("CapteurCO2");

  g_t_EcranLCD.setCursor(0,20);
  g_t_EcranLCD.setTextSize(1);
  g_t_EcranLCD.print("Initialisation...");

  g_t_EcranLCD.display();


  // Initialisation capteur SCD41
  g_t_CapteurSCD41.begin();
  // moncapteur.enableDebugging();  // message debug sur Serial
  if (g_t_CapteurSCD41.begin() == false)
  {
//    SEND_VTRACE(INFO, "SCD41 non détecté");
    for (;;);
  }
  else
  {
//    SEND_VTRACE(INFO, "SCD41  détecté");
  }

  acquerir();

  g_t_TimerTempoMesure.Init(NULL, 5000);
  g_t_TimerTempoMesure.Start();


  LedWS = new GestionClignotementLedWS(0, g_t_GestionMultiLedWS, 50);
  LedWS->ReglerLuminosite(64);
  LedWS->Demarre();
}

///////F: fonction PRINCIPALE//////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  uint16_t l_u16_TensionBatterieInt;

  if(g_t_TimerTempoMesure.IsTop() == true)
  {

    digitalWrite(13, 1);

    acquerir_afficher();
    delay(100);
    digitalWrite(13, 0);

    l_u16_TensionBatterieInt = (uint16_t)(10.0*TensionBatterie.GetConvertedValue(analogRead(0)));

    if(l_u16_TensionBatterieInt < 65)
    {
// Extinction immédiate
    }
    else if(l_u16_TensionBatterieInt < 74)
    {
// Batterie à 50%
    }
    else
    {
// Batterie pleine
    }

  }

}

void GestionTimningBP(uint32_t p_u32_arg, void* p_v_arg)
{
  static uint32_t l_u32_TempsEcoule = 0;
  static uint16_t l_u32_TempsAppuieBP = 0;
  TimerEvent_t * l_pt_TimerGestionBP = (TimerEvent_t *)p_v_arg;


  if(digitalRead(ENTREE_BP) == 1)
  {
    l_u32_TempsAppuieBP += l_pt_TimerGestionBP->GetValue();
  }
  else
  {
    if((l_u32_TempsAppuieBP > 100) && (l_u32_TempsAppuieBP < 500))
    {
      // Couper Alarme si ON

      // Remettre 15 min de temps ON si mode normal
    }


    l_u32_TempsAppuieBP = 0;
  }

  if(l_u32_TempsEcoule < 2000)
  {
    if(l_u32_TempsAppuieBP > 1000)
    {
//ACTIVER MODE_ON
    }


  }
  else
  {
    if(l_u32_TempsAppuieBP > 3000)
    {
//ACTIVER MODE OFF
    }

    if(l_u32_TempsEcoule > 900000)
    {
// Si Mode normal, extinction car allumé depuis 15min.

    }
    else if(l_u32_TempsEcoule > 840000)
    {
// Si Mode normal, alarme pour prévenir extinction prochain.

    }

  }

  l_u32_TempsEcoule += l_pt_TimerGestionBP->GetValue();
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
  qualite_air_t l_e_Qualite_Air;

  // acquerir taux de CO2
  taux_co2=(int)g_t_CapteurSCD41.getCO2();
  taux_pourcent=(float)taux_co2 /10000;
  dtostrf(taux_pourcent,4,2,taux_string);

//  taux_pourcent += 0.05;
//  dtostrf(taux_pourcent,4,2,taux_string);

//  Serial.println("");
//  Serial.print(F("taux de CO2 en ppm : "));
//  Serial.print(taux_co2);
//  Serial.print(F(" PPM "));
//  Serial.print("   en % : " );
//  Serial.print(taux_pourcent,1);
//  Serial.print(F(" % "));
//  Serial.println("");
//  Serial.print("taux d'humidité en %H : ");
//  Serial.print(moncapteur.getHumidity(), 1);
//  Serial.print("%H");
//  Serial.println("");
//
//  Serial.print(F("temoerature : "));
//  Serial.print(moncapteur.getTemperature());
//  Serial.print(F(" oC "));
//  Serial.println("");

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
    LedWS->SetSequence(1);
    break;

  case Mediocre:
    LedWS->SetSequence(2);
    break;

  case Mauvaise:
    LedWS->SetSequence(3);
    break;

  case Tres_Mauvaise:
    LedWS->SetSequence(4);
    break;

  case Danger:
    LedWS->SetSequence(5);
    break;

  }

}

