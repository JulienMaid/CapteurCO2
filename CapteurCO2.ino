#if 0

#include <Arduino.h>
//The setup function is called once at startup of the sketch
void setup()
{
  // Add your initialization code here
}

// The loop function is called in an endless loop
void loop()
{
  delay(1000);
  //Add your repeated code here
}

#endif

// licence Créative Common CC-BY-NC
//Les obligations liées à la licence sont de :
//• créditer les créateurs de la paternité des œuvres originales,
//• indiquer les sources et indiquer si des modifications ont été effectuées aux œuvres (obligation d’attribution) ;
//• ne pas tirer profit (gain direct ou plus-value commerciale) de l’œuvre ou des œuvres dérivées.
//version 3.0 est du 26/11/2024
// Auteurs Joan ERRA d'après prototype réalisé par Alain Franco
// descriptif sur dossier technique du site "radiolocalisationssouterraines.fr"
// ATTENTION à bien configurer IDE Arduino pour carte Arduino pr mini 3,3V 8MHz
// Mode Lowpower (2 mesures par minute)
// utilise Mstimer2 pour gérér les bips d'alarme



#include <EEPROM.h>
#define VER  3.0// Version  du programme
#define ver "3.0"
// grandeur mémorisée dans l'EEPROM
struct versions {float numero; int jour; int mois; int annee;};
versions version_encours ={VER,27,11,2024,};// version du 27/11/2024

#include <Wire.h> // pour la communication I2C
#include "SparkFun_SCD4x_Arduino_Library.h" // pour le captur SCD41
#include <Adafruit_GFX.h> // pour les graphiques
#include <Adafruit_SSD1306.h> // pour le circuit de commande de l'afficheur
#include <MsTimer2.h>

//Cette version du  progremme concerne les affectations suivantes
#define ROUGE 11 // led rouge de la led RVB
#define VERT 10  // led verte de la led RVB
#define BLEU 9   // led bleue de la led RVB
#define LED 13   // petite led sur la carte arduino pro mini
#define BUZ 6    // buzzer
#define ON 5     // commande du relais de maintien de la tension d'alimentation

#define PTEST 7// Sortie utilisée pour la mise au point
#define bp 2  // entree bouton poussoir
#define dipswitch 3// dipswitch pour desactiver alarmes sonores "
#define kVbat A0 // entrée fraction de la tension batterie
// kVbat envoyé au CAN avec k=0,39 (R6=47k, R7=30k)

#define seuil_piles_haut 822 // seuil d'alerte à 6,8V il reste 15 à 20mn de fonctionnement mesuré
#define demi_charge 962 // correspond à Vbat=7,96V
#define pleine_charge 980 // correspond à Vbat=8,1V
#define seuil_piles_bas     700   //seuil de 5,8V correspondant au seuil de décharge autorisé pour la batterie


//Variables de controle
char pret_pour_arret=0; //à la mise sous tension le relachement du poussoir
// ne doit pas arrêter le fonctionnement
char dejafait=0; // pour les messages qui ne doivent être effacés qu'une fois

char acquit_alarm1; // acquittement de l'alarme 1, pour arreter le son

char mode_normal=1;// à 1 mode normal: arret au bout de 15mn
// à 0 mode continu, pas d'arrte, pour étalonnafe éventuel
char niveau_alarme=0 ; // à 0 pas d'alarme, à 1 alarme1, à 2 alarm2
char cpt=0; // compteur pour l'alarme1
const unsigned long duree_fonctionnement= 900000; // 15mn en millisecondes

// variables intermédiaires de calcul
int Ncan; // Nombre résultat de la conversion analogique-numérique (can)
int Nblack; // Ntaille de la zone noire de l'icone jauge batterie (0 à 16 , à 16 tout noir)

// variables taux de CO2
unsigned int taux_co2; // sortie du capteur  en ppm
float taux_pourcent; // taux de CO2 en % en type float
char taux_string[4];// taux de CO2 en type string pour l'afficheur

// définition objet moncapteur
SCD4x moncapteur(0x62);// 0x62 adresse I2C du capteur
// configuration afficheur OLED
Adafruit_SSD1306 display(128,32, &Wire,-1 );//-1 pour signifier qu'aucune  des broches
// n'est utilisée pour initialiser l'affichage


///////F: fonction SETUP//////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);
  Serial.println(F("SCD41"));
  MsTimer2 :: set (100, alarme_cligno); //execution de la routine "alarme_cligno" toutes les 100ms
  // paramètre 50 au lieu de 100 car horloge à 8MHz au lieu de 16Mhz

  Wire.begin();// initialisation de la liaison I2C

  //  versions version_lue;// version-lue déclaré comme objet de type versions
  //
  //// enregistrer la version du firmware dans l'EEPROM à la première execution du programme
  ////pour limiter le nombre d'écriture dans l'EEPROM
  //  EEPROM.get( 0, version_lue );
  //  if(version_lue.numero !=VER)
  //  {
  //    Serial.println("écriture dans l'EEPROM de la version en cours");
  //    EEPROM.put(0,version_encours);
  //  }
  //
  //  EEPROM.get( 0, version_lue );
  //  Serial.print( "version lue depuis EEPROM: " ); Serial.print("    ");
  //  Serial.print( version_lue.numero );Serial.print("  ");
  //  Serial.print( version_lue.jour );Serial.print("/");
  //  Serial.print( version_lue.mois );Serial.print("/");
  //  Serial.println( version_lue.annee );

  // Initialisation capteur SCD41
  moncapteur.begin();
  // moncapteur.enableDebugging();  // message debug sur Serial
  if (moncapteur.begin() == false)
  {
    Serial.println(F("SCD41 non détecté"));
    for (;;);
  }
  else Serial.println(F("SCD41  détecté"));

  // Initialisation afficheur Oled 128 x 32
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 erreur d'allocation "));
    for (;;);
  }
  else Serial.println(F(" display detecté"));//
  display.clearDisplay();
  display.display();

  // configuration du sens des lignes d'entrées/sorties
  pinMode(ROUGE, OUTPUT);
  pinMode(VERT, OUTPUT);
  pinMode(BLEU, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(BUZ, OUTPUT);
  pinMode(ON, OUTPUT); // commande du relais
  pinMode(dipswitch,INPUT_PULLUP);// non utilisé dans cette version

  // initialisation d'interruption quand on appui sur le bouton poussoir
  attachInterrupt(digitalPinToInterrupt(bp), arreter, RISING);

  pret_pour_arret=0;
  //delay(1);
  tester_batterie_vide();// tester si la batterie est déchargéee
  fermer_contact();// prendre le relais du poussoir qui pourra être relaché

  message_accueil();//validation des sorties Interface Homme Machine

  acquerir();// faire la première mesure pour ne pas l'afficher car est à 0

  mode_normal=1; // par défaut le mode normal est activé
  mode_lowpower();//lancer le mode nolowpower
  pret_pour_arret=1;
  if(digitalRead(bp)==1) mode_normal=0;// si le poussoir est encore appuyé après le message d'accueil
  // alors passage en mode continu
  afficher_mode();
}

///////F: fonction PRINCIPALE//////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // tester la batterie
  tester_batterie();

  //si Mode Normal,éteindre quand la durée de fonctionnement est atteinte
  if (millis()>=(duree_fonctionnement - 120000) && mode_normal==1 && dejafait==0)
  {
    alerter_extinction();
    dejafait=1;
  }
  if (millis()>=duree_fonctionnement && mode_normal==1)
  {
    for (int i=0;i<2;i++)
    {
      digitalWrite(BUZ,1);
      delay(100);
      digitalWrite(BUZ,0);
      delay(100);
    }
    ouvrir_contact();
  }
  // faire mesure et affichage du résultat
  MsTimer2::start();
  acquerir_afficher(); // Durée 8 secondes passe à 5 si taux>2%
  gerer_ledRVB_niveauAlarm();//Durée 0 passe à 3 si 0,5%<taux<2% et à 2s si taux>2%
}

///////F:fonction message d'accueil permet verification allumage leds,afficheur et buzzer///
void message_accueil()
{
  digitalWrite(BUZ,1);delay(100);digitalWrite(BUZ,0);
  digitalWrite(ROUGE,1);
  digitalWrite(VERT,1);
  digitalWrite(BLEU,1);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(" DETECTEUR");
  display.setCursor(0,18);
  display.print("  de CO2 ");
  display.setTextSize(1);
  display.print(ver);
  display.display();
  pret_pour_arret=1;
  delay(1500);

  digitalWrite(ROUGE,0);
  digitalWrite(VERT,1);
  digitalWrite(BLEU,0);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("ENLEVER LE");
  display.setCursor(0,18);
  display.print(" BOUCHON ");
  display.display();
  delay(1500);

  digitalWrite(ROUGE,1);
  digitalWrite(VERT,0);
  digitalWrite(BLEU,0);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("2 MESURES");
  display.setCursor(0,18);
  display.print("PAR MINUTE");
  display.display();
  delay(1000);
  if (digitalRead(dipswitch)==0)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(" ALARME 1 ");
    display.setCursor(0,18);
    display.print("PAS DE SON");
    display.display();
    delay(1500);
  }
  else delay(500);
}

///////F:fonction affiche mode normal ou continu ////////////////////////////////////////////////////////////////
void afficher_mode()
{
  detachInterrupt(digitalPinToInterrupt(bp));

  digitalWrite(ROUGE,0);
  digitalWrite(VERT,0);
  digitalWrite(BLEU,1);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("   MODE");
  display.setCursor(0,18);
  if(mode_normal==1)
  {
    display.print("  NORMAL");
    display.display();
    if (tester_2appuis()==1) mode_etalonnage();

    digitalWrite(ROUGE,1);
    digitalWrite(VERT,0);
    digitalWrite(BLEU,1);
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("ARRET AUTO");
    display.setCursor(0,18);
    display.print("DANS 15mn");
    display.display();
    if (tester_2appuis()==1) config_usine();
  }
  else
  {
    display.print("  CONTINU");
    display.display();
    delay(3000);
  }
  digitalWrite(ROUGE,0);
  digitalWrite(VERT,0);
  digitalWrite(BLEU,0);
  attachInterrupt(digitalPinToInterrupt(bp), arreter, RISING);
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
  Serial.print(F("taux de CO2 en ppm : "));Serial.print(taux_co2);Serial.print(F(" PPM "));
  Serial.print("   en % : " );Serial.print(taux_pourcent,1);Serial.print(F(" % "));
  Serial.println("");
  Serial.print("taux d'humidité en %H : ");Serial.print(moncapteur.getHumidity(), 1);Serial.print("%H");
  Serial.println("");

  if(taux_co2 <20000)
  {
    /* affichage température non effectuee sur cette version
    //affichage température
     display.clearDisplay();
     icone_etat_piles();
    symbole_mode_encours();
    display.cp437(true);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.print(moncapteur.getTemperature(), 1);
    display.setTextSize(2);
    display.setCursor(80, 14);
    display.write(0xF8);
    display.println("C");
    display.display();
    delay(2000);
     */
    //affichage humidité
    display.clearDisplay();
    icone_etat_piles();
    symbole_mode_encours();
    display.cp437(true);
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    if(moncapteur.getHumidity()>99) display.print("100");
    else display.print(moncapteur.getHumidity(), 1);
    display.setTextSize(2);
    display.setCursor(80, 16);
    display.write(0x25);
    display.println("h");
    display.display();
    delay(2000);
  }
  if(taux_co2 <40000)
  {
    //affichage taux de CO2 en ppm
    display.clearDisplay();
    icone_etat_piles();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 8);
    display.print(taux_co2);
    if(taux_co2 <10000) display.setCursor(74, 16);
    if(taux_co2 <1000) display.setCursor(58, 16);
    if(taux_co2 >10000) display.setCursor(92, 16);
    display.setTextSize(2);
    display.println("ppm");
    display.display();
    delay(3000);
  }

  // affichage taux en %
  display.clearDisplay();
  icone_etat_piles();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(0, 8);
  if(taux_co2<40000)
  {
    for(int i=0;i<4;i++)display.print(taux_string[i]);
    display.write(0x25);
    display.setTextSize(2);
    display.setCursor( 92, 16);
    display.println("co2");
    display.display();
  }
  else
  {
    display.setTextSize(2);
    display.setCursor( 0, 16);
    display.write("Taux");
    display.setCursor( 50, 10);
    display.setTextSize(3);
    display.write(0x3E);// affichage du symbole >
    display.write("4");
    display.write(0x25);//affichage de %
    display.display();
  }
  delay(3000);
}

///////F:fonction qui fait clignoter le buzzer et la led RVB suivant le niveau_alarme 1 ou 2//////////////////////////
void alarme_cligno()
{
  if(niveau_alarme==0)
  {
    cpt=0;
    digitalWrite(BUZ,0);
  }
  if(niveau_alarme==2 )
  {
    digitalWrite(BUZ,!digitalRead(BUZ));
    digitalWrite(ROUGE,!digitalRead(ROUGE));
    digitalWrite(LED,!digitalRead(LED));
    digitalWrite(VERT,0);
  }
  if(niveau_alarme==1)
  {
    digitalWrite(VERT,0);
    if(cpt==10)
    {
      if (acquit_alarm1==0)digitalWrite(BUZ,!digitalRead(BUZ) & digitalRead(dipswitch));
      else digitalWrite(BUZ,0);// éteindre alarme sonore si acquittée
      digitalWrite(ROUGE,!digitalRead(ROUGE));
      delay(500);
      cpt=0;
    }
    cpt++;
  }
}


///////F:fonction qui commande la led RVB, défini le niveau d'alarma et affiche "EVACUEZ" si taux>2%////////////
void gerer_ledRVB_niveauAlarm()
{

  if ( taux_co2 <= 1800)
  {
    niveau_alarme=0;
    digitalWrite(ROUGE, 0); // voyant au vert
    digitalWrite(VERT, 1);
    digitalWrite(BLEU, 0);
    digitalWrite(BUZ, 0);
  }
  if ((taux_co2 > 1800) && (taux_co2 <= 5000))
  {
    niveau_alarme=0;
    digitalWrite(ROUGE, 1);  // voyant à l'orange
    digitalWrite(VERT, 1);
    digitalWrite(BLEU, 0);
    digitalWrite(BUZ, 0);
  }

  // si taux compris entre 5000ppm et 20000ppm durée exposition au pro rata
  if ((taux_co2 >=5000) && (taux_co2 <20000))
  {
    niveau_alarme=0;
    digitalWrite(ROUGE, 1);  // voyant au rouge
    digitalWrite(VERT, 0);
    digitalWrite(BLEU, 0);
    digitalWrite(BUZ, 0);
    //afficher_duree_expo();
  }
  // si taux compris entre 20000ppm et 30000ppm
  if (taux_co2 >=20000 && taux_co2 <30000)niveau_alarme=1;
  // si taux supérieur à 30000ppm
  if (taux_co2 >=30000)
  {
    niveau_alarme=2;
    digitalWrite(VERT, 0);
    digitalWrite(BLEU, 0);
  }
  Serial.print(niveau_alarme);Serial.print("     ");

  if (taux_co2>=20000)
  {
    // afficher "DANGER"
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print(" DANGER");
    display.display();
    if (taux_co2>=30000)delay(1500); else delay(2000);
    display.clearDisplay();
  }

  if (taux_co2 >=30000 )
  {
    // afficher "EVACUEZ"
    display.clearDisplay();
    display.setTextSize(3);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.print("EVACUEZ");
    display.display();
    delay(1500);
    display.clearDisplay();
  }
}

///////F:fonction fermer le contact du relais /////////////////////////////////////////////////////////////////////
void fermer_contact()
{
  // on alimente la bobine du relais
  digitalWrite(ON,1);
  delay(100);// Delai pour laisser le temps au relais de coller
}

///////F:fonction ouvrir le contact du relais  monostable//////////////////////////////////////////////////////////
void ouvrir_contact()
{
  // ouvrir le contact , si le poussoir est relaché
  //revient à débrancher l'alimentation

  // on coupe l'alimentation de la bobine du relais monostable ////////////////////////////////////////////////////
  digitalWrite(ON,0);
  for(int i=0;i<50;i++)delayMicroseconds(1000);// durée impulsion 30ms
  digitalWrite(LED,0);
}

///////F: mettre hors tension la sonde suite à impulsion sour le poussoir//////////////////////////////////////////
void arreter()
{
  if (pret_pour_arret==1)
  {
    for(int i=0;i<2000;i++) delayMicroseconds(1000); // attendre 2s
    if (digitalRead(bp)==1)// si au bout de 2s le BP est toujours appuyé
    {
      digitalWrite(BUZ,1);
      for(int i=0;i<100;i++)delayMicroseconds(1000);// Bip de 100ms
      digitalWrite(BUZ,0);
      ouvrir_contact();
      //attendre que le condo de filtrage d'alim se décharge
      digitalWrite(LED,1);
      while(1);
    }
    else
    {
      if (niveau_alarme==1)acquit_alarm1=!acquit_alarm1;
      else acquit_alarm1=0;
    }
  }

  // on ne fait rien, car pas encore prêt pour être arréter par le poussoir
  //ce qui est le cas au relachement du poussoir à la mise en route  du détecteur.
}


///////F:fonction tester batterie ///////////////////////////////////////////////////////////////////
void tester_batterie()
{
  tester_batterie_vide();
  // si niveau de batterie bas afficher message
  if (analogRead(kVbat)<=seuil_piles_haut)
  {
    display.clearDisplay();  // affichage OLED
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(" RECHARGEZ");
    display.setCursor(0,18);
    display.print(" BATTERIE");
    display.display();
    // delai de 3s, pour lire l'affichage
    for(int i=0;i<3000;i++)delayMicroseconds(1000);
  }
}


/// F: si batterie vide afficher les message batterie vides puis mettre hors tension///////////////////////////////
void tester_batterie_vide()
{
  if (analogRead(kVbat)<seuil_piles_bas)
  {
    display.clearDisplay();  // affichage OLED
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print("  BATTERIE");
    display.setCursor(0,18);
    display.print("    VIDE");
    display.display();


    for(int i=0; i<50;i++)
    {
      digitalWrite(BUZ,1); delay(200);
      digitalWrite(BUZ,0); delay(500);
    }
    ouvrir_contact();
  }
}
///////F:fonction qui sélectionne le mode low power à 2 mesures par minute//////////////////////////////////////////
void mode_lowpower()
{

  // Configurer le mode Lowpower
  //stopper le mode lecture par défaut (1 mesure toutes les 5se)
  if (moncapteur.stopPeriodicMeasurement() == true)
  {Serial.println(F("Arret des mesures périodiques toutes les 5s "));}
  // démarrer le mode low power: 1 mesure toutes les 30s
  if (moncapteur.startLowPowerPeriodicMeasurement() == true)
  {Serial.println(F("lancement des mesures périodiques toutes les 30s (mode Low Power)"));}
}


///////F:fonction qui affiche message d'alerte extinction et fait biper le buzzer
void alerter_extinction()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print("   ARRET");
  display.setCursor(0,18);
  display.print(" DANS 2 mn");
  display.display();
  for (int i=0;i<2;i++)
  {
    digitalWrite(BUZ,1);
    delay(500);
    digitalWrite(BUZ,0);
    delay(500);
  }
  delay(10000);// le message este affiché 10s
}

///////F:fonction qui génère l'icone état piles////////////////////////////////////////////////////////////////
void icone_etat_piles()
{
  display.drawRoundRect(108,0, 20, 10,3, WHITE);
  display.fillRect(110,2, 16, 6, WHITE);
  Ncan=analogRead(kVbat);
  // SI Vbat<6,8V (822) rectangle rempli de noir
  if(Ncan<=seuil_piles_haut)Nblack=16;
  // à Vbat=7,96V (962)jauge à moitié (Demi-charge)
  if (Ncan>seuil_piles_haut and Ncan<demi_charge) Nblack=map(Ncan,seuil_piles_haut,demi_charge,16,8);
  // à Vbat=8,1V  jauge pleine (de blanc)
  if (Ncan>demi_charge) Nblack=map(Ncan,demi_charge,pleine_charge,8,0);
  display.fillRect(110,2,Nblack,6,BLACK);
}

///////F:fonction qui génère le symbole du mode en cours////////////////////////////////////////////////////////////////
void symbole_mode_encours()
{
  display.drawRoundRect(114,20, 14, 12,6, WHITE);
  display.setCursor(118, 22);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  if(mode_normal==1)display.println("N");
  else display.println("C");
}

///////F:fonction qui effectue l'étalonnage à 420ppm////////////////////////////////////////////////////////////////
void mode_etalonnage()
{
  unsigned char data[12]; //contiendra 2 octets + 1 octet CRC pour chaque grandeur mesurée
  // et 2 octet  + 1 octet CRC pour l'état du capteur
  unsigned int  reference=420; // 420ppm valeur de reference d'étalonnage choisie

  unsigned char echec, nombre_minutes;


  pret_pour_arret=1;
  attachInterrupt(digitalPinToInterrupt(bp), arreter, RISING);

  // stoP scd measurement in periodic mode
  Wire.beginTransmission(0x62);
  Wire.write(0x3F);//0x3f86  commande d'arret de mesure périodique
  Wire.write(0x86);
  Wire.endTransmission();
  delay(600);

  // Initialiser SCD41 en mode mesures périodiques mis à jour toutes les 5s
  Wire.beginTransmission(0x62);// 0x62 Adresse I2C du SCD41
  Wire.write(0x21);//0X21B1 commande de mesure périodique
  Wire.write(0xb1);
  Wire.endTransmission();

  //afficher message d'attente et compte à rebours"

  for(nombre_minutes=20;nombre_minutes>3;nombre_minutes--)
  {
    for(int i=0;i<10;i++)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);display.setCursor(0,0);
      display.print(" ETALONNAGE EN COURS");
      display.setCursor(0,18);
      display.print(" ATTENDRE  ");
      display.setTextSize(2);
      display.print(nombre_minutes);
      display.print("mn");
      display.display();
      delay(3000);
      digitalWrite(BLEU,!digitalRead(BLEU));
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);display.setCursor(0,0);
      display.print("PLACER LE DETECTEUR");
      display.setCursor(0,18);
      display.print("DEHORS ET A L'ABRI");
      display.display();
      delay(3000);
      digitalWrite(BLEU,!digitalRead(BLEU));
    }

  }
  digitalWrite(BLEU,1);
  // arreter mode mesure
  Wire.beginTransmission(0x62);
  Wire.write(0x3f);
  Wire.write(0x86);
  Wire.endTransmission();

  //delai pour le capteur
  delay(20);
  digitalWrite(LED,1);
  // préparer données pour étalonnage
  data[0] = (reference & 0xff00) >> 8;// contient l'octet de poids fort
  data[1] = reference & 0x00ff;       // contient l'octet de poids faible
  data[2] = CalcCrc(data);// calculer CRC

  // envoyer la commande d'étalonnage ( perform_forced_recalibration)
  Wire.beginTransmission(0x62);
  Wire.write(0x36);// envoie 0X362F d'étalonnage forcé et attente du résultat
  Wire.write(0x2F);
  // ajouter les données pour l'étalonnage
  // 2 bytes valeur référence d'étalonnage, CRC
  Wire.write(data[0]);
  Wire.write(data[1]);
  Wire.write(data[2]);
  Wire.endTransmission();

  // attendre 500ms la fin de l'étalonnage
  delay(500);

  // Démarrer le mode mesure périodique (toutes les 5s)
  Wire.beginTransmission(0x62);
  Wire.write(0x21);
  Wire.write(0xb1);
  echec=Wire.endTransmission();

  // attendre 3mn la stabilisation des mesures (conseilfabricant)
  for(nombre_minutes=3;nombre_minutes>0;nombre_minutes--)
  {
    for(int i=0;i<30;i++)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);display.setCursor(0,0);
      display.print(" ETALONNAGE EN COURS");
      display.setCursor(0,18);
      display.print(" ATTENDRE  ");
      display.setTextSize(2);
      display.print(nombre_minutes);
      display.print("mn");
      display.display();
      delay(2000);
      digitalWrite(BLEU,!digitalRead(BLEU));
    }
  }



  // Affichage message final
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  if (echec== 0 )display.print("  SUCCES  ");
  else display.print("   ECHEC ");
  display.setCursor(0,18);
  display.print(" ETEINDRE ");
  display.display();

  // faire buzzer pour alerter utilisateur que c'est fini
  for(int i=0;i<10;i++)
  {
    digitalWrite(BUZ,1); delay(1000);
    digitalWrite(BUZ,0); delay(300);
  }

  for(int i=0;i<30;i++)//
  {
    digitalWrite(BUZ,1); delay(70);
    digitalWrite(BUZ,0); delay(270);
  }
  while(1);
}

//////F:calcul du CRC conformément datasheet section 5.17
unsigned char CalcCrc(unsigned char data[2])
{
  unsigned char crc = 0xFF;
  for(int i = 0; i < 2; i++)
  {
    crc ^= data[i];
    for(char bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ 0x31u;
      else crc = (crc << 1);
    }
  }
  return crc;
}

//////F: fonction qui restaure les paramètres en sortie d'usine /////////////////////////////////
void config_usine()
{
  unsigned char echec;
  pret_pour_arret=1;
  attachInterrupt(digitalPinToInterrupt(bp), arreter, RISING);
  // arreter mode mesure
  Wire.beginTransmission(0x62);
  Wire.write(0x3f);
  Wire.write(0x86);
  Wire.endTransmission();
  //delai pour le capteur
  delay(20);

  for(int compte_a_rebours=30; compte_a_rebours>0;compte_a_rebours--)
  {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.print(" RESTAURATION ");
    display.setCursor(0,12);
    display.print("CONFIG USINE DANS ");
    display.print(compte_a_rebours);
    display.print("s");
    display.setCursor(0,25);
    display.print("ETEINDRE POUR ANNULER");
    delay(1000);
    display.display();
    digitalWrite(ROUGE,!digitalRead(ROUGE));
    digitalWrite(BLEU,!digitalRead(BLEU));

  }
  // restaurer paramètres en sortie d'usine
  Wire.beginTransmission(0x62);
  Wire.write(0x36);
  Wire.write(0x32);
  echec=Wire.endTransmission();
  //delai pour le capteur
  delay(20);

  // Affichage message final
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  if (echec== 0 )display.print("  SUCCES  ");
  else display.print("   ECHEC ");
  display.setCursor(0,18);
  display.print(" ETEINDRE ");
  display.display();

  while(1);
}


///////F:fonction qui affiche la durée d'exposition maximum conseillée/////////////////////////

// si taux compris entre 5000ppm et 20000ppm durée exposition au pro rata
void  afficher_duree_expo()
{
  unsigned int dureetotale_minutes,duree_heures,duree_minutes;
  niveau_alarme=0;
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("EXPOSITION");
  display.setCursor(0, 18);
  display.print(" MAX: ");
  dureetotale_minutes= map(taux_co2,5000,20000,480,15);
  duree_heures = dureetotale_minutes / 60;
  duree_minutes = dureetotale_minutes - (duree_heures * 60);
  if (duree_heures>1)
  {
    if (duree_minutes>30){ duree_heures++; duree_minutes=0;}
    if (duree_minutes<30) duree_minutes=0;
  }
  display.setCursor(70, 18);
  display.print(duree_heures);display.print("h");
  if (duree_minutes>0){display.print(duree_minutes);display.print("mn");}
  display.display();
  delay(3000);
}



////////F: tester si 2  appuis successifs brefs sur le bouton poussoir///////////////////
int tester_2appuis()
{
  int compteur_de_temps;
  compteur_de_temps=0;
  //attendre appui  poussoir dans un délai de 2s
  while(compteur_de_temps<2000 and digitalRead(bp)==0)
  {delay(1);compteur_de_temps++;}
  if (digitalRead(bp)==0) {return 0;}

  //attendre le relachement du poussoir dans un délai de 2s
  compteur_de_temps=0;
  while(compteur_de_temps<2000 and digitalRead(bp)==1)
  {delay(1);compteur_de_temps++;}
  if (digitalRead(bp)==1) {return 0;}
  compteur_de_temps=0;

  //attendre l'appui du poussoir dans un délai de 2s
  while(compteur_de_temps<2000 and digitalRead(bp)==0)
  {delay(1);compteur_de_temps++;}
  if (digitalRead(bp)==0) {return 0;}
  return 1;
}
