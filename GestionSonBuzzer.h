/*
 * GestionSonBuzzer.h
 *
 *  Created on: 18 juin 2026
 *      Author: julien
 */

#ifndef GESTIONSONBUZZER_H_
#define GESTIONSONBUZZER_H_

#include "GestionClignotementLed.h"

class GestionSonBuzzer: public GestionClignotementLed
{
public:
  GestionSonBuzzer(uint32_t p_u32_PortLed = 0, bool p_b_Reverse = false, bool p_b_AutonomousOperation = true,
      uint8_t p_u8_SequenceLength = 20, uint32_t p_u32_PeriodeSequence = 100);
  virtual ~GestionSonBuzzer();

  //! Active la séquence n°x
  virtual void SetSequence(uint8_t p_u8_NumeroSequence) override;


};

#endif /* GESTIONSONBUZZER_H_ */
