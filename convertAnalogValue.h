#pragma once

#include "stdint.h"

class ConvertAnalogValue
{
public:
  //! Constructeur de la classe
  //! @param p_u32_AnalogMedian valeur de la zone médiane
  //! @param p_u32_DeadZone valeur de la largeur de la zone morte
  //! @param p_dble_MaxPosValue valeur convertie maximale
  //! @param p_dble_MinNegValue valeur convertie minimale
  //! @param p_u32_AnalogMin valeur analogique minimun
  //! @param p_u32_AnalogMax valeur analogique maximum
  ConvertAnalogValue(uint32_t p_u32_AnalogMedian = 512, uint32_t p_u32_DeadZone = 30,
		  double p_dble_MinNegValue = -600, double p_dble_MaxPosValue = 1000, uint32_t p_u32_AnalogMin = 100,
		  uint32_t p_u32_AnalogMax = 1024);
  virtual ~ConvertAnalogValue();

  //! Retourne la consigne pwm en fonction la consigne de barre
  //! @param p_u32_AnalogValue valeur analogique de consigne de la barre
  //! @return valeur de consigne pwn associée
  virtual double GetConvertedValue(uint32_t p_u32_AnalogValue);

  double getDbleMaxPosValue() const
  {
    return m_dble_MaxPosValue;
  }

  double getDbleMinNegValue() const
  {
    return m_dble_MinNegValue;
  }

  uint32_t getMU32AnalogMax() const
  {
    return m_u32_AnalogMax;
  }

  void setMU32AnalogMedian(uint32_t p_u32_AnalogMedian)
  {
    m_u32_AnalogMedian = p_u32_AnalogMedian;
  }

  uint32_t getMU32AnalogMedian() const
  {
    return m_u32_AnalogMedian;
  }

  uint32_t getMU32AnalogMin() const
  {
    return m_u32_AnalogMin;
  }

  uint32_t getMU32DeadZone() const
  {
    return m_u32_DeadZone;
  }

private:

  uint32_t m_u32_AnalogMedian;

  uint32_t m_u32_AnalogMin;

  uint32_t m_u32_AnalogMax;

  uint32_t m_u32_DeadZone;

  double m_dble_MaxPosValue;

  double m_dble_MinNegValue;
};

