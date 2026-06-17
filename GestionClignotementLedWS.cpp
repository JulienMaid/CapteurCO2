/*
 * GestionClignotementLedWS.cpp
 *
 *  Created on: 17 juin 2026
 *      Author: julien
 */

#include "GestionClignotementLedWS.h"

GestionClignotementLedWS::GestionClignotementLedWS(uint8_t p_u8_IndexLed,
    GestionLedWS_t * p_pt_ControleurLed,
          uint8_t p_u8_SequenceLength) : m_u8_SequenceLength(p_u8_SequenceLength), m_u8_SequenceIndex(0)
{
  m_ptu32_LedSequence = new HTMLColorCode[m_u8_SequenceLength];
  ClearSequence();

  m_pt_GestionLedWS = p_pt_ControleurLed;

  m_t_TimerLed.Init(StaticGetSequence, 100, true, this);
  m_t_TimerLed.Start();
}

GestionClignotementLedWS::~GestionClignotementLedWS()
{
  m_t_TimerLed.Stop();
  delete[] m_ptu32_LedSequence;
}

void GestionClignotementLedWS::ClearSequence(void)
{
  memset(m_ptu32_LedSequence, HTMLColorCode::Black, m_u8_SequenceLength);
}

void GestionClignotementLedWS::SetSequence1(void)
{
  HTMLColorCode l_tu8_LedSequence[m_u8_SequenceLength] =
  { HTMLColorCode::Red, HTMLColorCode::Red, HTMLColorCode::Blue, HTMLColorCode::Red, HTMLColorCode::Red,
      0, 0, 0, 0, 0,
      HTMLColorCode::Purple, HTMLColorCode::Purple, HTMLColorCode::Purple, HTMLColorCode::Red, HTMLColorCode::Red,
      0, 0, 0, 0, 0 };

  memcpy(m_ptu32_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength*sizeof(*m_ptu32_LedSequence));
}

void GestionClignotementLedWS::SetSequence2(void)
{
  HTMLColorCode l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu32_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength*sizeof(*m_ptu32_LedSequence));

}

void GestionClignotementLedWS::SetSequence3(void)
{
  HTMLColorCode l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu32_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength*sizeof(*m_ptu32_LedSequence));
}

void GestionClignotementLedWS::SetSequence4(void)
{
  HTMLColorCode l_tu8_LedSequence[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0,
      0, 0, 0, 0 };

  memcpy(m_ptu32_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength*sizeof(*m_ptu32_LedSequence));
}

HTMLColorCode GestionClignotementLedWS::GetSequence(void)
{
  HTMLColorCode l_u8_returnedValue;

  l_u8_returnedValue = m_ptu32_LedSequence[m_u8_SequenceIndex];

  m_u8_SequenceIndex++;
  if (m_u8_SequenceIndex >= m_u8_SequenceLength)
  {
    m_u8_SequenceIndex = 0;
  }

  return l_u8_returnedValue;
}

void GestionClignotementLedWS::StaticGetSequence(uint32_t arg1, void *ClassGestionLed)
{
  GestionClignotementLedWS *l_t_GestionLED = (GestionClignotementLedWS*) ClassGestionLed;

  l_t_GestionLED->m_pt_GestionLedWS->Nouvelle_Valeur(0, l_t_GestionLED->GetSequence(), true);
//  digitalWrite(l_t_GestionLED->GetPortLed(), l_t_GestionLED->GetSequence());
}
