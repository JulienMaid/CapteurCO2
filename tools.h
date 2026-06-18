#pragma once



typedef enum qualite_air_t
{
  Acceptable = 0,
  Mediocre,
  Mauvaise,
  Tres_Mauvaise,
  Danger
} qualite_air_t;


qualite_air_t Determiner_Qualite_Air(const float & p_f_tauxCO2);
