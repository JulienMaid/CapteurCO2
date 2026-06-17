#include <EEPROM.h>

#include <Wire.h> // pour la communication I2C
#include "SparkFun_SCD4x_Arduino_Library.h" // pour le captur SCD41
#include <Adafruit_GFX.h> // pour les graphiques
#include <Adafruit_SSD1306.h> // pour le circuit de commande de l'afficheur
#include <MsTimer2.h>
#include <Adafruit_NeoPixel.h>
#include "timer_sw.h"
#include "trace_debug.h"

#include "DefinitionES.h"

#include "gestion_Led_WS.h"

#include "convertAnalogValue.h"
#include "GestionClignotementLedWS.h"



#define PIN_WS2812B  4   // ESP32 pin that connects to WS2812B
#define NUM_PIXELS     1  // The number of LEDs (pixels) on WS2812B

Adafruit_NeoPixel LEDWS2812(NUM_PIXELS, PIN_WS2812B, NEO_RGB + NEO_KHZ800);

GestionLedWS_t * MultiLED;


// variables taux de CO2
unsigned int taux_co2; // sortie du capteur  en ppm
float taux_pourcent; // taux de CO2 en % en type float
char taux_string[4];// taux de CO2 en type string pour l'afficheur

// définition objet moncapteur
SCD4x moncapteur(0x62);// 0x62 adresse I2C du capteur
// configuration afficheur OLED
Adafruit_SSD1306 display(128,32, &Wire,-1 );//-1 pour signifier qu'aucune  des broches
// n'est utilisée pour initialiser l'affichage

TimerEvent_t TimerTempoMesure;

ConvertAnalogValue TensionBatterie(0, 0, 0.0, 10.0, 0, 1023);

//GestionClignotementLed LedInterne(13);

GestionClignotementLedWS * LedWS;

///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  MsTimer2 :: set (DEC_TIMESTAMP, Inc_Timer); //execution de la routine "alarme_cligno" toutes les 100ms
  MsTimer2:: start();

  Init_Trace_Debug();

//  pinMode(13, OUTPUT);
//  digitalWrite(13, 1);


//  LedInterne.SetSequence3();

  SEND_VTRACE(INFO, "SCD41");

  Wire.begin();// initialisation de la liaison I2C

  // Initialisation capteur SCD41
  moncapteur.begin();
  // moncapteur.enableDebugging();  // message debug sur Serial
  if (moncapteur.begin() == false)
  {
    SEND_VTRACE(INFO, "SCD41 non détecté");
    for (;;);
  }
  else
  {
    SEND_VTRACE(INFO, "SCD41  détecté");
  }

  acquerir();

  // Initialisation afficheur Oled 128 x 32
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 erreur d'allocation "));
    for (;;);
  }
  else
  {
    Serial.println(F(" display detecté"));//
  }
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("CapteurCO2");

  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Initialisation...");

  display.display();

  TimerTempoMesure.Init(NULL, 5000);
  TimerTempoMesure.Start();


  MultiLED = new GestionLedWS_t(NUM_PIXELS, PIN_WS2812B);

  LedWS = new GestionClignotementLedWS(0, MultiLED);

  LedWS->SetSequence1();


//  LEDWS2812.begin();
//  LEDWS2812.clear();

  while(1);

}

///////F: fonction PRINCIPALE//////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  if(TimerTempoMesure.IsTop() == true)
  {
    MultiLED->Nouvelle_Valeur(0, HTMLColorCode::Red, true);

//    LEDWS2812.setPixelColor(0, HTMLColorCode::Red);
//    LEDWS2812.show();


    digitalWrite(13, 1);

    acquerir_afficher();
    delay(100);
    digitalWrite(13, 0);


    MultiLED->Nouvelle_Valeur(0, HTMLColorCode::Green, true);

//    LEDWS2812.setPixelColor(0, HTMLColorCode::Green);
//    LEDWS2812.show();


  }

}


///////F:fonction faire la première mesure pour ne pas l'afficher car est à 0   /////////////////////////////////////
void acquerir()
{
  moncapteur.getCO2();
  moncapteur.getTemperature();
  moncapteur.getHumidity();
}

///////F:fonction faire la mesure, l'afficher  //////////////////////////////////////////////
void acquerir_afficher()
{

  // acquerir taux de CO2
  taux_co2=(int)moncapteur.getCO2();
  taux_pourcent=(float)taux_co2 /10000;
  if (taux_pourcent<10000)dtostrf(taux_pourcent,4,2,taux_string);
  else dtostrf(taux_pourcent,4,1,taux_string);

  Serial.println("");
  Serial.print(F("taux de CO2 en ppm : "));
  Serial.print(taux_co2);
  Serial.print(F(" PPM "));
  Serial.print("   en % : " );
  Serial.print(taux_pourcent,1);
  Serial.print(F(" % "));
  Serial.println("");
  Serial.print("taux d'humidité en %H : ");
  Serial.print(moncapteur.getHumidity(), 1);
  Serial.print("%H");
  Serial.println("");

  Serial.print(F("temoerature : "));
  Serial.print(moncapteur.getTemperature());
  Serial.print(F(" oC "));
  Serial.println("");

  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(taux_string);
  display.print("%");
  display.display();


}

