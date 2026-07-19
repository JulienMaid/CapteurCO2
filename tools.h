#pragma once

#include <stdint.h>

#define DEBUG 1

typedef enum qualite_air_t
{
  Acceptable = 0,
  Mediocre,
  Mauvaise,
  Tres_Mauvaise,
  Danger
} qualite_air_t;

typedef enum mode_operation_t
{
  mode_extinction = 0,
  mode_normal_debut,
  mode_normal,
  mode_continu,
  mode_stop_alamre,
} mode_operation_t;

typedef enum mode_alarme_t
{
  alarme_off = 0,
  alarme_fin_TempsON,
  alarme_attention,
  alarme_alerte,
} mode_alarme_t;

qualite_air_t Determiner_Qualite_Air(const float & p_f_tauxCO2);

void Init_EntreesSorties(void);

void Machine_Etat_Generale(void);

void acquerir_afficher();

void acquerir();

void Mode_Normal_Debut(void);

void Mode_Normal(void);

void Mode_Continu(void);

void Mode_ON(void);

void Mode_OFF(void);

void Mode_Stop_Alarme(void);

void Icone_Etat_Piles(uint8_t p_u8_Valeur);

void Symbole_Mode_En_Cours(mode_operation_t p_e_ModeEnCours);

void Afficher_Temps_ON(uint8_t p_u8_MinutesON);

uint8_t Tester_Batterie(void);
