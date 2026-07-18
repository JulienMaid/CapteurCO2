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
//#include "GestionClignotementLed.h"
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


VariableTracee<uint16_t> g_t_Etat_En_Ecours(mode_extinction, "g_t_Etat_En_Ecours", DEBUG);
//VariableTracee<mode_operation_t> g_t_Etat_En_Ecours(mode_extinction, "g_t_Etat_En_Ecours", DEBUG);
boolean g_e_Alarme_En_Cours = false;

void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg);

///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  uint16_t l_u16_TensionBatterieInt = 1000;

  if(g_t_TimerTempoMesure.IsTop() == true)
  {
    Serial.print("Etat:");
    Serial.println(g_t_Etat_En_Ecours.LireValeur());

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

  icone_etat_piles();
}

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
//  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Purple, true);

  Init_EntreesSorties();

  Wire.begin();// initialisation de la liaison I2C

  // Initialisation afficheur Oled 128 x 32
  g_t_EcranLCD.begin(SSD1306_SWITCHCAPVCC, 0x3C);

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

  acquerir();

  g_t_TimerTempoMesure.Init(NULL, 5000);
  g_t_TimerTempoMesure.Start();

  g_t_GestionMultiLedWS->Nouvelle_Valeur(0, HTMLColorCode::Black, true);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);

  g_t_ClignotementLedWS = new GestionClignotementLedWS(0, g_t_GestionMultiLedWS, 50);
  g_t_ClignotementLedWS->ReglerLuminosite(64);


  //Pour débug..
#if DEBUG == 1
  g_t_Etat_En_Ecours.EcrireValeur(mode_normal_debut);
#endif
}

///////F: fonction PRINCIPALE//////////////////////////////////////////////////////////////////////////////////////
void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg)
{
  static uint32_t l_u32_TempsEcoule = 0;
  static uint16_t l_u16_TempsAppuieBP = 0;
  static uint16_t l_u16_TempsRelacheBP = 0;
  static VariableTracee<uint16_t> l_b_EtatBP(1, "l_b_EtatBP", DEBUG);
  TimerEvent_t * l_pt_TimerGestionBP = (TimerEvent_t *)p_v_arg;
//  static uint8_t l_u8_NbreAppui = 0;
  static VariableTracee<uint16_t> l_u8_NbreAppui(0, "l_u8_NbreAppui", DEBUG);


  l_b_EtatBP.EcrireValeur(digitalRead(ENTREE_BP));

  if(l_b_EtatBP.LireValeur() == 1)
  {
    if(l_u16_TempsRelacheBP != 0)
    {
      l_u16_TempsAppuieBP = 0;
      l_u16_TempsRelacheBP = 0;
    }

    if(l_u8_NbreAppui.LireValeur() == 0)
    {
      l_u8_NbreAppui.EcrireValeur(1);
    }
    else if((l_u16_TempsRelacheBP < 200) && (l_u16_TempsRelacheBP != 0))
    {
      l_u8_NbreAppui.EcrireValeur(l_u8_NbreAppui.LireValeur() + 1);
    }
    l_u16_TempsRelacheBP = 0;

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

    if(l_u8_NbreAppui.LireValeur() >= 3)
    {
      g_t_Etat_En_Ecours.EcrireValeur(mode_continu);
    }

    if(l_u16_TempsRelacheBP > 1000)
    {
      l_u8_NbreAppui.EcrireValeur(0);
    }

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

  if(g_t_Etat_En_Ecours.LireValeur() != mode_extinction)
  {
    if(l_u16_TempsAppuieBP > 2000)
    {
      g_t_Etat_En_Ecours.EcrireValeur(mode_continu);
    }

    if(l_u16_TempsAppuieBP > 4000)
    {
      //ACTIVER MODE OFF
      g_t_Etat_En_Ecours.EcrireValeur(mode_extinction);
    }
  }

  if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
  {
    if(l_u32_TempsEcoule > 900000)
    {
      // Si Mode normal, extinction car allumé depuis 15min.
        g_t_Etat_En_Ecours.EcrireValeur(mode_extinction);
    }
    else if(l_u32_TempsEcoule > 840000)
    {
      // Si Mode normal, alarme pour prévenir extinction prochain.
//      g_t_GestionBuzzer.SetSequence(1);
    }
  }
  l_u32_TempsEcoule += l_pt_TimerGestionBP->GetValue();
}

void Machine_Etat_Generale(void)
{
  static mode_operation_t g_e_Etat_Precedent = mode_extinction;


  if(((g_t_Etat_En_Ecours.LireValeur() != g_e_Etat_Precedent))
      || (g_t_Etat_En_Ecours.LireValeur() == mode_normal_debut))
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

      if(g_e_Etat_Precedent != mode_normal)
      {
        Mode_Normal();
      }
      g_e_Etat_Precedent = g_t_Etat_En_Ecours.LireValeur();
      g_t_Etat_En_Ecours.EcrireValeur(mode_normal);
    }

//    if(g_t_Etat_En_Ecours.LireValeur() == mode_normal)
//    {
//      Mode_Normal();
//    }
    else if(g_t_Etat_En_Ecours.LireValeur() == mode_continu)
    {
      Mode_Continu();
    }

    g_e_Etat_Precedent = g_t_Etat_En_Ecours.LireValeur();

  }

}

///////F:fonction qui génère l'icone état piles////////////////////////////////////////////////////////////////
 void icone_etat_piles()
{
   uint8_t Nblack= 8;

  g_t_EcranLCD.drawRoundRect(108,0, 20, 10,3, WHITE);
  g_t_EcranLCD.fillRect(110,2, 16, 6, WHITE);
//  Ncan=analogRead(kVbat);
//  // SI Vbat<6,8V (822) rectangle rempli de noir
// if(Ncan<=seuil_piles_haut)Nblack=16;
// // à Vbat=7,96V (962)jauge à moitié (Demi-charge)
// if (Ncan>seuil_piles_haut and Ncan<demi_charge) Nblack=map(Ncan,seuil_piles_haut,demi_charge,16,8);
// // à Vbat=8,1V  jauge pleine (de blanc)
// if (Ncan>demi_charge) Nblack=map(Ncan,demi_charge,pleine_charge,8,0);
 g_t_EcranLCD.fillRect(110,2,Nblack,6,BLACK);
}

///////F:fonction qui génère le symbole du mode en cours////////////////////////////////////////////////////////////////
 void symbole_mode_encours()
{
   g_t_EcranLCD.drawRoundRect(114,20, 14, 12,6, WHITE);
   g_t_EcranLCD.setCursor(118, 22);
   g_t_EcranLCD.setTextSize(1);
   g_t_EcranLCD.setTextColor(WHITE);
  /*if(mode_normal==1)display.println("N");
  else*/ g_t_EcranLCD.println("C");
}
