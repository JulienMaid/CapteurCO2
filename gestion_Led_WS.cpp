/*
 * gestion_Led_WS.cpp
 *
 *  Created on: 11 juin 2026
 *      Author: julien
 */

#include "gestion_Led_WS.h"


GestionLedWS_t::GestionLedWS_t(uint16_t p_u8_NbreLed, int16_t p_i16_NumPatte) : m_u16_NbreLed(p_u8_NbreLed)
{
  if(m_pt_NeoPixel != nullptr)
  {
    delete m_pt_NeoPixel;
  }

//  m_pt_NeoPixel = new Adafruit_NeoPixel(p_u8_NbreLed, p_i16_NumPatte, NEO_RGB + NEO_KHZ800);
  m_pt_NeoPixel = new Adafruit_NeoPixel(m_u16_NbreLed, p_i16_NumPatte, NEO_GRB + NEO_KHZ800);

  m_pt_NeoPixel->begin();
  m_pt_NeoPixel->clear();

}

bool GestionLedWS_t::Nouvelle_Valeur(uint16_t p_u16_numLed, uint32_t p_u32_Valeur, bool p_b_Actualiser)
{
  if(p_u16_numLed >= m_u16_NbreLed)
  {
    return false;
  }

  m_pt_NeoPixel->setPixelColor(p_u16_numLed, p_u32_Valeur);

  if(1)//(p_b_Actualiser == true)
  {
    m_pt_NeoPixel->show();
  }



  return true;

}

void GestionLedWS_t::Actualiser_Bandeau()
{
  m_pt_NeoPixel->show();
}

void GestionLedWS_t::ReglerLuminosite(uint8_t p_u8_Luminosite)
{
  m_pt_NeoPixel->setBrightness(p_u8_Luminosite);
}

