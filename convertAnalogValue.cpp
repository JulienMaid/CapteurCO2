#include "convertAnalogValue.h"

#include <Arduino.h>

// @brief fonction équivalente à map mais passant de uint32_t en type double
double mapDouble(long x, long in_min, long in_max, double out_min, double out_max);

double mapDouble(long x, long in_min, long in_max, double out_min, double out_max)
{
    const double run = in_max - in_min;
    if(run == 0){
        return 0; // AVR returns -1, SAM returns 0
    }
    const double rise = out_max - out_min;
    const double delta = x - in_min;
    return (delta * rise) / run + out_min;
}


ConvertAnalogValue::ConvertAnalogValue(uint32_t p_u32_AnalogMedian, uint32_t p_u32_DeadZone, double p_dble_MinNegValue,
    double p_dble_MaxPosValue, uint32_t p_u32_AnalogMin, uint32_t p_u32_AnalogMax) :
    m_u32_AnalogMin(p_u32_AnalogMin), m_u32_AnalogMax(p_u32_AnalogMax), m_u32_AnalogMedian(
        p_u32_AnalogMedian), m_u32_DeadZone(p_u32_DeadZone), m_dble_MaxPosValue(p_dble_MaxPosValue),
		m_dble_MinNegValue(p_dble_MinNegValue)
{
  // TODO Auto-generated constructor stub

}

ConvertAnalogValue::~ConvertAnalogValue()
{
  // TODO Auto-generated destructor stub
}

double ConvertAnalogValue::GetConvertedValue(uint32_t p_u32_AnalogValue)
{
	double l_dble_Consigne;

  if (p_u32_AnalogValue >= (m_u32_AnalogMedian + m_u32_DeadZone))
  {
    l_dble_Consigne = mapDouble(p_u32_AnalogValue, m_u32_AnalogMedian, m_u32_AnalogMax, 0, m_dble_MaxPosValue);

  }
  else if (p_u32_AnalogValue <= (m_u32_AnalogMedian - m_u32_DeadZone))
  {
    l_dble_Consigne = mapDouble(p_u32_AnalogValue, m_u32_AnalogMedian, m_u32_AnalogMin, 0, m_dble_MinNegValue);
  }
  else
  {
    l_dble_Consigne = 0;
  }

  return l_dble_Consigne;
}
