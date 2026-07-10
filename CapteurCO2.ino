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
#include "VariableTracee.h"


GestionLedWS_t * g_t_GestionMultiLedWS;
GestionClignotementLedWS * g_t_ClignotementLedWS;
//GestionClignotementLed g_t_ClignotementLedInterne(CMD_LED_BOUTON, false, false);
//GestionSonBuzzer g_t_GestionBuzzer(CMD_BUZZER, false, false, 200);


// définition objet moncapteur
SCD4x g_t_CapteurSCD41(SCD4x_SENSOR_SCD41);
// configuration afficheur OLED
Adafruit_SSD1306 g_t_EcranLCD(128,32, &Wire,-1 );//-1 pour signifier qu'aucune  des broches
// n'est utilisée pour initialiser l'affichage

TimerEvent_t g_t_TimerTempoMesure;
TimerEvent_t g_t_TimerGestionGenerale;


//ConvertAnalogValue TensionBatterie(0, 0, 0.0, 10.0, 0, 1023);


VariableTracee<mode_operation_t> g_t_Etat_En_Ecours(mode_extinction, "g_t_Etat_En_Ecours", true);
boolean g_e_Alarme_En_Cours = false;

void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg);

///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  MsTimer2 :: set (DEC_TIMESTAMP, Inc_Timer); //execution de la routine "alarme_cligno" toutes les 100ms
  MsTimer2:: start();

  Serial.begin(115200);
  Serial.println("CapteurCO2");

//  Init_Trace_Debug();
  g_t_TimerGestionGenerale.Init(GestionTimningGeneral, 50, Periodic_Timer, &g_t_TimerGestionGenerale);
  g_t_TimerGestionGenerale.Start();

  g_t_GestionMultiLedWS = new GestionLedWS_t(NUM_PIXELS, CMD_LEDWS);

  g_t_GestionMultiLedWS->Nouvelle_Valeur(0, HTMLColorCode::Purple, true);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Purple, true);

  Init_EntreesSorties();

  Wire.begin();// initialisation de la liaison I2C

  // Initialisation afficheur Oled 128 x 32
  if (!g_t_EcranLCD.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
//    Serial.println(F("SSD1306 erreur d'allocation "));
    for (;;);
  }
  else
  {
//    Serial.println(F(" display detecté"));//
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

  g_t_GestionMultiLedWS->Nouvelle_Valeur(0, HTMLColorCode::Black, true);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);

  g_t_ClignotementLedWS = new GestionClignotementLedWS(0, g_t_GestionMultiLedWS, 50);
  g_t_ClignotementLedWS->ReglerLuminosite(64);
}

///////F: fonction PRINCIPALE//////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  uint16_t l_u16_TensionBatterieInt = 1000;

  if(g_t_TimerTempoMesure.IsTop() == true)
  {
    acquerir_afficher();

//    l_u16_TensionBatterieInt = (uint16_t)(10.0*TensionBatterie.GetConvertedValue(analogRead(0)));

    if(l_u16_TensionBatterieInt < 65)
    {
      // Extinction immédiate
      g_t_Etat_En_Ecours.EcrireValeur(mode_extinction);
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

  Machine_Etat_Generale();
}

void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg)
{
  static uint32_t l_u32_TempsEcoule = 0;
  static uint16_t l_u16_TempsAppuieBP = 0;
  static uint16_t l_u16_TempsRelacheBP = 0;
  static VariableTracee<uint16_t> l_b_EtatBP(1, "l_b_EtatBP", true);
  TimerEvent_t * l_pt_TimerGestionBP = (TimerEvent_t *)p_v_arg;

  l_b_EtatBP.EcrireValeur(digitalRead(ENTREE_BP));

  if(l_b_EtatBP.LireValeur() == 1)
  {
    if(l_u16_TempsAppuieBP < 0xffff)
    {
      l_u16_TempsAppuieBP += l_pt_TimerGestionBP->GetValue();
      Serial.println(l_u16_TempsAppuieBP);
    }

    if(l_u32_TempsEcoule < 2000)
    {
      if(l_u16_TempsAppuieBP > 1000)
      {
        //ACTIVER MODE_ON
        g_t_Etat_En_Ecours.EcrireValeur(mode_normal_debut);
        digitalWrite(CMD_LED_BOUTON,1);
      }
    }
    else
    {
      digitalWrite(CMD_LED_BOUTON,1);
    }

  }
  else
  {
    digitalWrite(CMD_LED_BOUTON,0);

    if(l_u16_TempsRelacheBP < 0xffff)
    {
      l_u16_TempsRelacheBP += l_pt_TimerGestionBP->GetValue();
    }


    if((l_u16_TempsAppuieBP > 100) && (l_u16_TempsAppuieBP < 1000))
    {
      // Couper Alarme si ON
//      g_t_GestionBuzzer.ClearSequence();

      // Remettre 15 min de temps ON si mode normal
      if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
      {
        Serial.println("Remise 0");
        g_t_Etat_En_Ecours.EcrireValeur(mode_normal_debut);
        l_u32_TempsEcoule = 0;
      }
    }

    l_u16_TempsAppuieBP = 0;
  }


  if(l_u16_TempsAppuieBP > 3000)
  {
    //ACTIVER MODE OFF
    g_t_Etat_En_Ecours.EcrireValeur(mode_extinction);
  }

  if(l_u32_TempsEcoule > 900000)
  {
    // Si Mode normal, extinction car allumé depuis 15min.
    if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
    {
      g_t_Etat_En_Ecours.EcrireValeur(mode_extinction);
    }
  }
  else if(l_u32_TempsEcoule > 840000)
  {
    // Si Mode normal, alarme pour prévenir extinction prochain.
    if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
    {
//      g_t_GestionBuzzer.SetSequence(1);
    }
  }

  l_u32_TempsEcoule += l_pt_TimerGestionBP->GetValue();
}

void Machine_Etat_Generale(void)
{
  static mode_operation_t g_e_Etat_Precedent = mode_extinction;


  if((g_t_Etat_En_Ecours.LireValeur() != g_e_Etat_Precedent))
  {
    if((g_e_Etat_Precedent == mode_extinction) && (g_t_Etat_En_Ecours.LireValeur() != mode_extinction))
    {
      Mode_ON();
    }
    else if((g_e_Etat_Precedent != mode_extinction) && (g_t_Etat_En_Ecours.LireValeur() == mode_extinction))
    {
      Mode_OFF();
    }

    if(g_t_Etat_En_Ecours.LireValeur() == mode_normal_debut)
    {
      Mode_Normal_Debut();
      g_t_Etat_En_Ecours.EcrireValeur(mode_normal);
    }

    if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
    {
      Mode_Normal();
    }
    else if(g_t_Etat_En_Ecours.LireValeur() == mode_continu)
    {
      Mode_Continu();
    }

    g_e_Etat_Precedent = g_t_Etat_En_Ecours.LireValeur();

  }

}


