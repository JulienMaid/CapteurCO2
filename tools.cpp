/*
 * tools.cpp
 *
 *  Created on: 18 juin 2026
 *      Author: julien
 */

#include "tools.h"


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
