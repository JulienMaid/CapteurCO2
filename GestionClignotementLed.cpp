/*
 * GestionLed.cpp
 *
 *  Created on: 16 mars 2024
 *      Author: julien
 */

#include <string.h>
#include <Arduino.h>
#include "GestionClignotementLed.h"

GestionClignotementLed::GestionClignotementLed(uint32_t p_u32_PortLed, bool p_b_Reverse,
		bool p_b_AutonomousOperation, uint8_t p_u8_SequenceLength) :
    m_u32_PortLed(p_u32_PortLed), m_u8_SequenceLength(p_u8_SequenceLength), m_u8_SequenceIndex(0),
	m_b_Reverse(p_b_Reverse)
{
  m_ptu8_LedSequence = new uint8_t[m_u8_SequenceLength];
  ClearSequence();

  pinMode(m_u32_PortLed, OUTPUT);

  m_t_TimerLed.Init(StaticGetSequence, 100, true, this);

  if (p_b_AutonomousOperation == true)
  {
    m_t_TimerLed.Start();
  }
}

GestionClignotementLed::~GestionClignotementLed()
{
  m_t_TimerLed.Stop();
  delete[] m_ptu8_LedSequence;
}

void GestionClignotementLed::ClearSequence(void)
{
  memset(m_ptu8_LedSequence, 0, m_u8_SequenceLength);
}

uint8_t GestionClignotementLed::GetSequence(void)
{
  uint8_t l_u8_returnedValue;

  if (m_b_Reverse == true)
  {
    l_u8_returnedValue = !m_ptu8_LedSequence[m_u8_SequenceIndex];
  }
  else
  {
    l_u8_returnedValue = m_ptu8_LedSequence[m_u8_SequenceIndex];
  }

  m_u8_SequenceIndex++;
  if (m_u8_SequenceIndex >= m_u8_SequenceLength)
  {
    m_u8_SequenceIndex = 0;
  }

  return l_u8_returnedValue;
}

uint32_t GestionClignotementLed::GetPortLed(void)
{
  return m_u32_PortLed;
}

void GestionClignotementLed::SetSequence(uint8_t p_u8_NumeroSequence)
{
    const uint8_t l_tu8_LedSequenceDef[m_u8_SequenceLength] = { 0 };
    const uint8_t l_tu8_LedSequence1[m_u8_SequenceLength] = { 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0,
        0, 0, 0, 0 };
    const uint8_t l_tu8_LedSequence2[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0 };
    const uint8_t l_tu8_LedSequence3[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0 };
    const uint8_t l_tu8_LedSequence4[m_u8_SequenceLength] = { 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0,
        0, 0, 0, 0 };

    uint8_t * l_ptu8_LedSequenceX = nullptr;

  m_u8_SequenceIndex = 0;

  switch(p_u8_NumeroSequence)
  {
  case 1:
    l_ptu8_LedSequenceX = l_tu8_LedSequence1;
    break;
  case 2:
    l_ptu8_LedSequenceX = l_tu8_LedSequence2;
    break;
  case 3:
    l_ptu8_LedSequenceX = l_tu8_LedSequence3;
    break;
  case 4:
    l_ptu8_LedSequenceX = l_tu8_LedSequence4;
    break;
  default:
    l_ptu8_LedSequenceX = l_tu8_LedSequenceDef;
    break;
  }

  memcpy(m_ptu8_LedSequence, l_ptu8_LedSequenceX, m_u8_SequenceLength);

}

void GestionClignotementLed::StaticGetSequence(uint32_t arg1, void *ClassGestionLed)
{
  GestionClignotementLed *l_t_GestionLED = (GestionClignotementLed*) ClassGestionLed;

  digitalWrite(l_t_GestionLED->GetPortLed(), l_t_GestionLED->GetSequence());
}

void GestionClignotementLed::Demarrer(void)
{
  m_t_TimerLed.Start();
}

void GestionClignotementLed::Arreter(void)
{
  m_t_TimerLed.Stop();
}
