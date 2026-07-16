

#pragma once

#include <stdint.h>
#include <Arduino.h>

template<typename T>
class VariableTracee
{
public:
  //! Contructeur de la classe
  //! @param valeur du coeff du membre "entrée" tel que Outn = p_d_InputCoeff * Inn + (1 - p_d_InputCoeff) * Outn-1
  VariableTracee(T p_t_ValeurInitiale, String p_t_LibelleValariable, bool p_b_ActiverTracage = false);

  virtual ~VariableTracee();

  void EcrireValeur(T p_T_NouvelleValeur);

  T LireValeur(void);

  void ActiverTracage(bool p_b_Activer);

private:
  T m_t_ValeurInterne;

  String m_t_LibelleValariable;
  bool m_b_ActiverTracage;

};

template<typename T>
inline VariableTracee<T>::VariableTracee(T p_t_ValeurInitiale, String p_t_LibelleValariable, bool p_b_ActiverTracage) :
  m_t_ValeurInterne(p_t_ValeurInitiale), m_t_LibelleValariable(p_t_LibelleValariable), m_b_ActiverTracage(p_b_ActiverTracage)
{
}

template<typename T>
inline VariableTracee<T>::~VariableTracee()
{
}

template<typename T>
inline void VariableTracee<T>::EcrireValeur(T p_T_NouvelleValeur)
{
  if(m_b_ActiverTracage == true)
  {
    if(m_t_ValeurInterne != p_T_NouvelleValeur)
    {
      Serial.print(m_t_LibelleValariable);
      Serial.print("<--");
      Serial.println(p_T_NouvelleValeur);
    }
  }

  m_t_ValeurInterne = p_T_NouvelleValeur;
}

template<typename T>
inline T VariableTracee<T>::LireValeur(void)
{
  return m_t_ValeurInterne;
}

template<typename T>
inline void VariableTracee<T>::ActiverTracage(bool p_b_Activer)
{
  m_b_ActiverTracage = p_b_Activer;
}


