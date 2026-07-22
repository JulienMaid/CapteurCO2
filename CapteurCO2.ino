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
#include "GestionClignotementLedWS.h"

#include "tools.h"
#include "VariableTracee.h"


GestionLedWS_t * g_t_GestionMultiLedWS;
GestionClignotementLedWS * g_t_ClignotementLedWS;


// définition objet moncapteur
SCD4x g_t_CapteurSCD41(SCD4x_SENSOR_SCD41);
// configuration afficheur OLED
Adafruit_SSD1306 g_t_EcranLCD(128,32, &Wire,-1 );//-1 pour signifier qu'aucune  des broches
// n'est utilisée pour initialiser l'affichage

TimerEvent_t g_t_TimerTempoMesure;
TimerEvent_t g_t_TimerGestionGenerale;


//ConvertAnalogValue TensionBatterie(0, 0, 0.0, 10.0, 0, 1023);


mode_operation_t g_e_EtatEnEcours = mode_extinction;
VariableTracee<mode_alarme_t> g_t_ModeAlarme(silence, "g_t_ModeAlarme", DEBUG);
static uint32_t g_u32_TempsEcoule = 0;
static constexpr uint32_t g_u32_TempsMaxON = 900000;


void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg);

///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  uint16_t l_u16_TensionBatterieInt = 1000;
  uint8_t l_u8_TempsONRestant = 0;
  uint8_t l_u8_NiveauBatterie = 0;

  if(g_t_TimerTempoMesure.IsTop() == true)
  {
    l_u8_NiveauBatterie = Tester_Batterie();

    acquerir_afficher();

    Icone_Etat_Piles(l_u8_NiveauBatterie);

    Symbole_Mode_En_Cours(g_e_EtatEnEcours);

    if(g_e_EtatEnEcours == mode_normal)
    {
      l_u8_TempsONRestant = (uint8_t)((g_u32_TempsMaxON - g_u32_TempsEcoule)/(uint32_t)60000)+1;

      Afficher_Temps_ON(l_u8_TempsONRestant);
    }



    g_t_EcranLCD.display();
  }

  Machine_Etat_Generale();

}

void setup()
{
  MsTimer2 :: set (DEC_TIMESTAMP, Inc_Timer);
  MsTimer2:: start();

  Serial.begin(115200);
  Serial.println("CapteurCO2");

  g_t_TimerGestionGenerale.Init(GestionTimningGeneral, 50, Periodic_Timer, &g_t_TimerGestionGenerale);
  g_t_TimerGestionGenerale.Start();

  g_t_GestionMultiLedWS = new GestionLedWS_t(NUM_PIXELS, CMD_LEDWS);

  g_t_GestionMultiLedWS->Nouvelle_Valeur(0, HTMLColorCode::Purple, true);

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

  g_t_TimerTempoMesure.Init(NULL, 5000);
  g_t_TimerTempoMesure.Start();

  g_t_GestionMultiLedWS->Nouvelle_Valeur(0, HTMLColorCode::Black, true);
  g_t_GestionMultiLedWS->Nouvelle_Valeur(1, HTMLColorCode::Black, true);

  g_t_ClignotementLedWS = new GestionClignotementLedWS(0, g_t_GestionMultiLedWS, 50);
  g_t_ClignotementLedWS->ReglerLuminosite(64);

  //Pour débug..
#if DEBUG == 1
  g_e_EtatEnEcours = mode_normal_debut;
#endif
}

void GestionTimningGeneral(uint32_t p_u32_arg, void* p_v_arg)
{
  static uint16_t l_u16_TempsAppuieBP = 0;
  bool l_b_EtatBP = 0;
  TimerEvent_t * l_pt_TimerGestionBP = (TimerEvent_t *)p_v_arg;
  uint16_t l_u16_DeltaTemps = l_pt_TimerGestionBP->GetValue();

  l_b_EtatBP = digitalRead(ENTREE_BP);

  if(l_b_EtatBP == 1)
  {
    if(l_u16_TempsAppuieBP < 0xffff)
    {
      l_u16_TempsAppuieBP += l_u16_DeltaTemps;
      Serial.println(l_u16_TempsAppuieBP);
    }

    if(g_u32_TempsEcoule < 2000)
    {
      if(l_u16_TempsAppuieBP > 1000)
      {
        //ACTIVER MODE_ON
        g_e_EtatEnEcours = mode_normal_debut;
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

    if((l_u16_TempsAppuieBP > 100) && (l_u16_TempsAppuieBP < 1000))
    {
      // Couper Alarme si ON
      g_t_ModeAlarme = alarme_off;

      // Remettre 15 min de temps ON si mode normal
      if(g_e_EtatEnEcours == mode_normal)
      {
        g_e_EtatEnEcours = mode_normal_debut;
        g_u32_TempsEcoule = 0;
      }
    }

    l_u16_TempsAppuieBP = 0;
  }

  if(g_e_EtatEnEcours != mode_extinction)
  {
    if(l_u16_TempsAppuieBP > 2000)
    {
      g_e_EtatEnEcours = mode_continu;
    }

    if(l_u16_TempsAppuieBP > 4000)
    {
      //ACTIVER MODE OFF
      g_e_EtatEnEcours = mode_extinction;
    }
  }

  if(g_e_EtatEnEcours == mode_normal)
  {
    if(g_u32_TempsEcoule > g_u32_TempsMaxON)
    {
      // Si Mode normal, extinction car allumé depuis 15min.
        g_e_EtatEnEcours = mode_extinction;
    }
    else if(g_u32_TempsEcoule > (g_u32_TempsMaxON-60000))
    {
      // Si Mode normal, alarme pour prévenir extinction prochain.
      if(g_e_EtatEnEcours == mode_normal)
      {
        g_t_ModeAlarme = alarme_fin_TempsON;
      }
    }
  }
  g_u32_TempsEcoule += l_u16_DeltaTemps;

// Gestion alarmes
  if(g_t_ModeAlarme.LireValeur() == alarme_off)
  {
    g_t_ModeAlarme = silence;
    digitalWrite(CMD_BUZZER, 0);
  }
  else if(g_t_ModeAlarme != silence)
  {
    static uint8_t Temps100ms = 0;

    Temps100ms ^= 1; // passe à zero toutes les 100ms

    switch(g_t_ModeAlarme)
    {
    case alarme_fin_TempsON:
    case alarme_attention:
      static uint8_t Temps1s=10;

      if(Temps100ms == 0)
      {
        Temps1s -= 1;

        if(Temps1s == 0)
        {
          Temps1s = 10;
          digitalWrite(CMD_BUZZER, 1);
        }
      }
      else
      {
        digitalWrite(CMD_BUZZER, 0);
      }

      break;
    case alarme_alerte:
    case alarme_batterie_faible:
      static uint8_t Temps500ms=5;
      static bool l_b_ValeurBuzzer = 0;

      if(Temps100ms == 0)
      {
        Temps500ms -= 1;

        if(Temps500ms == 0)
        {
          Temps500ms = 5;

          l_b_ValeurBuzzer ^= 1;
          digitalWrite(CMD_BUZZER, l_b_ValeurBuzzer);
        }
      }

      break;
    default:
      break;
    }
  }




}

void Machine_Etat_Generale(void)
{
  static mode_operation_t g_e_Etat_Precedent = mode_extinction;


  if(((g_e_EtatEnEcours != g_e_Etat_Precedent))
      || (g_e_EtatEnEcours == mode_normal_debut))
  {
    if((g_e_Etat_Precedent == mode_extinction) && (g_e_EtatEnEcours != mode_extinction))
    {
      Mode_ON();
    }
    else if((g_e_Etat_Precedent != mode_extinction) && (g_e_EtatEnEcours == mode_extinction))
    {
      Mode_OFF();
    }

    if(g_e_EtatEnEcours == mode_normal_debut)
    {
      Mode_Normal_Debut();

      if(g_e_Etat_Precedent != mode_normal)
      {
        Mode_Normal();
      }
      g_e_Etat_Precedent = g_e_EtatEnEcours;
      g_e_EtatEnEcours = mode_normal;
    }
    else if(g_e_EtatEnEcours == mode_continu)
    {
      Mode_Continu();
    }

    g_e_Etat_Precedent = g_e_EtatEnEcours;

  }

}
