/*
 * GestionSonBuzzer.cpp
 *
 *  Created on: 18 juin 2026
 *      Author: julien
 */

#include "GestionSonBuzzer.h"
#include <string.h>
#include <stdint.h>

GestionSonBuzzer::GestionSonBuzzer(uint32_t p_u32_PortLed, bool p_b_Reverse,
    bool p_b_AutonomousOperation, uint8_t p_u8_SequenceLength, uint32_t p_u32_PeriodeSequence) :
    GestionClignotementLed(p_u32_PortLed, p_b_Reverse, p_b_AutonomousOperation, p_u8_SequenceLength, p_u32_PeriodeSequence)
{
  // TODO Auto-generated constructor stub

}

GestionSonBuzzer::~GestionSonBuzzer()
{
  // TODO Auto-generated destructor stub
}

void GestionSonBuzzer::SetSequence(uint8_t p_u8_NumeroSequence)
{
  const uint8_t l_tu8_BuzzerSequenceDef[m_u8_SequenceLength] = { 0 };
  const uint8_t l_tu8_BuzzerSequence1[m_u8_SequenceLength] = { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0,
      0, 0, 0, 0 };
  const uint8_t l_tu8_BuzzerSequence2[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };
  const uint8_t l_tu8_BuzzerSequence3[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };
  const uint8_t l_tu8_BuzzerSequence4[m_u8_SequenceLength] = { 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
      1, 1, 0, 0 };

  uint8_t * l_ptu8_LedSequenceX = nullptr;

  m_u8_SequenceIndex = 0;

  switch(p_u8_NumeroSequence)
  {
  case 1:
  l_ptu8_LedSequenceX = l_tu8_BuzzerSequence1;
  break;
  case 2:
  l_ptu8_LedSequenceX = l_tu8_BuzzerSequence2;
  break;
  case 3:
  l_ptu8_LedSequenceX = l_tu8_BuzzerSequence3;
  break;
  case 4:
  l_ptu8_LedSequenceX = l_tu8_BuzzerSequence4;
  break;
  default:
  l_ptu8_LedSequenceX = l_tu8_BuzzerSequenceDef;
  break;
  }

  memcpy(m_ptu8_LedSequence, l_ptu8_LedSequenceX, m_u8_SequenceLength);

}
