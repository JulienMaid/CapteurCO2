/*
 * GestionClignotementLedWS.h
 *
 *  Created on: 17 juin 2026
 *      Author: julien
 */

#ifndef GESTIONCLIGNOTEMENTLEDWS_H_
#define GESTIONCLIGNOTEMENTLEDWS_H_

#include "timer_sw.h"
#include "gestion_Led_WS.h"

class GestionClignotementLedWS
{
public:
  GestionClignotementLedWS(uint8_t p_u8_IndexLed,
      GestionLedWS_t * p_pt_ControleurLed,
      uint8_t p_u8_SequenceLength = 20);
  virtual ~GestionClignotementLedWS();
  //! Retourne la valeur courante de la séquence d'allumage
  virtual uint32_t GetSequence(void);

  virtual void ReglerLuminosite(uint8_t p_u8_Luminosite);

  virtual void Demarrer(void);

  virtual void Arreter(void);

  //! Active la séquence d'extinction (chaque valeur de la séquence est 0)
  virtual void ClearSequence(void);

  //! Active la séquence n°x
  virtual void SetSequence(uint8_t p_u8_NumeroSequence);

  //! methode statique utilisée par le timer logiciel dans le fonctionnement autonome
  static void StaticGetSequence(uint32_t arg1, void *ClassGestionLed);

protected:
  uint8_t m_u8_IndexLed;
  uint8_t m_u8_SequenceLength;
  uint8_t m_u8_SequenceIndex;

  uint8_t *m_ptu8_LedSequence;

  GestionLedWS_t *m_pt_GestionLedWS;

  TimerEvent_t m_t_TimerLed;
};

#endif /* GESTIONCLIGNOTEMENTLEDWS_H_ */
