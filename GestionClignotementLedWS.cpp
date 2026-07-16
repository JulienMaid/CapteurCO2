/*
 * GestionClignotementLedWS.cpp
 *
 *  Created on: 17 juin 2026
 *      Author: julien
 */

#include "GestionClignotementLedWS.h"

GestionClignotementLedWS::GestionClignotementLedWS(uint8_t p_u8_IndexLed,
    GestionLedWS_t * p_pt_ControleurLed,
          uint8_t p_u8_SequenceLength) : m_u8_SequenceLength(p_u8_SequenceLength), m_u8_SequenceIndex(0),
          m_u8_IndexLed(p_u8_IndexLed)
{
  m_ptu8_LedSequence = new uint8_t[m_u8_SequenceLength];
  ClearSequence();

  m_pt_GestionLedWS = p_pt_ControleurLed;

  m_t_TimerLed.Init(StaticGetSequence, 100, true, this);
  m_t_TimerLed.Start();
}

GestionClignotementLedWS::~GestionClignotementLedWS()
{
  m_t_TimerLed.Stop();
  delete[] m_ptu8_LedSequence;
}

void GestionClignotementLedWS::ClearSequence(void)
{
  memset(m_ptu8_LedSequence, 0, m_u8_SequenceLength);
}

uint32_t GestionClignotementLedWS::GetSequence(void)
{
  uint32_t l_u32_returnedValue = HTMLColorCode::Black;

  switch(m_ptu8_LedSequence[m_u8_SequenceIndex])
  {
  case 1:
    l_u32_returnedValue = HTMLColorCode::Green;
    break;
  case 2:
    l_u32_returnedValue = HTMLColorCode::Orange;
    break;
  case 3:
    l_u32_returnedValue = HTMLColorCode::Red;
    break;
  default:
    l_u32_returnedValue = HTMLColorCode::Black;
    break;
  }

  m_u8_SequenceIndex++;
  if (m_u8_SequenceIndex >= m_u8_SequenceLength)
  {
    m_u8_SequenceIndex = 0;
  }

  return l_u32_returnedValue;
}

void GestionClignotementLedWS::ReglerLuminosite(uint8_t p_u8_Luminosite)
{
  m_pt_GestionLedWS->ReglerLuminosite(p_u8_Luminosite);
}

void GestionClignotementLedWS::StaticGetSequence(uint32_t arg1, void *ClassGestionLed)
{
  GestionClignotementLedWS *l_t_GestionLED = (GestionClignotementLedWS*) ClassGestionLed;

  l_t_GestionLED->m_pt_GestionLedWS->Nouvelle_Valeur(l_t_GestionLED->m_u8_IndexLed, l_t_GestionLED->GetSequence(), true);
}

void GestionClignotementLedWS::Demarrer(void)
{
  m_t_TimerLed.Start();
}

void GestionClignotementLedWS::Arreter(void)
{
  m_t_TimerLed.Stop();
}

void GestionClignotementLedWS::SetSequence(uint8_t p_u8_NumeroSequence)
{
  uint8_t l_tu8_LedSequence[m_u8_SequenceLength] = {0};

  m_u8_SequenceIndex = 0;

  switch(p_u8_NumeroSequence)
  {
  case 1:
    l_tu8_LedSequence[0] = 1;
    break;
  case 2:
    l_tu8_LedSequence[0] = 2;
    break;
  case 3:
    l_tu8_LedSequence[0] = 3;
    break;
  case 4:
    l_tu8_LedSequence[0] = 3;
    l_tu8_LedSequence[1] = 3;
    l_tu8_LedSequence[5] = 3;
    l_tu8_LedSequence[6] = 3;
    l_tu8_LedSequence[10] = 3;
    l_tu8_LedSequence[11] = 3;
    break;
  case 5:
    for(auto i=0; i<m_u8_SequenceLength; i=i+10)
    {
        l_tu8_LedSequence[i+0] = 3;
        l_tu8_LedSequence[i+1] = 3;
        l_tu8_LedSequence[i+2] = 3;
        l_tu8_LedSequence[i+3] = 3;
        l_tu8_LedSequence[i+4] = 3;
        l_tu8_LedSequence[i+5] = 0;
        l_tu8_LedSequence[i+6] = 0;
        l_tu8_LedSequence[i+7] = 0;
        l_tu8_LedSequence[i+8] = 0;
        l_tu8_LedSequence[i+9] = 0;
    }
    break;
  default:
    break;
  }

  memcpy(m_ptu8_LedSequence, l_tu8_LedSequence, m_u8_SequenceLength);

}

void GestionClignotementLedWS::ReglerIndexLed(uint8_t p_u8_IndexLed)
{
  if(m_u8_IndexLed != p_u8_IndexLed)
  {
    m_pt_GestionLedWS->Nouvelle_Valeur(m_u8_IndexLed, HTMLColorCode::Black, true);

    m_u8_IndexLed = p_u8_IndexLed;
  }
}
