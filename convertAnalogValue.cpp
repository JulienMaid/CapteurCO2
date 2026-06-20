#include "convertAnalogValue.h"

#include <Arduino.h>

// @brief fonction équivalente à map mais passant de uint32_t en type float
float mapfloat(long x, long in_min, long in_max, float out_min, float out_max);

float mapfloat(long x, long in_min, long in_max, float out_min, float out_max)
{
    const float run = in_max - in_min;
    if(run == 0){
        return 0; // AVR returns -1, SAM returns 0
    }
    const float rise = out_max - out_min;
    const float delta = x - in_min;
    return (delta * rise) / run + out_min;
}


ConvertAnalogValue::ConvertAnalogValue(uint32_t p_u32_AnalogMedian, uint32_t p_u32_DeadZone, float p_dble_MinNegValue,
    float p_dble_MaxPosValue, uint32_t p_u32_AnalogMin, uint32_t p_u32_AnalogMax) :
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

float ConvertAnalogValue::GetConvertedValue(uint32_t p_u32_AnalogValue)
{
	float l_dble_Consigne;

  if (p_u32_AnalogValue >= (m_u32_AnalogMedian + m_u32_DeadZone))
  {
    l_dble_Consigne = mapfloat(p_u32_AnalogValue, m_u32_AnalogMedian, m_u32_AnalogMax, 0, m_dble_MaxPosValue);

  }
  else if (p_u32_AnalogValue <= (m_u32_AnalogMedian - m_u32_DeadZone))
  {
    l_dble_Consigne = mapfloat(p_u32_AnalogValue, m_u32_AnalogMedian, m_u32_AnalogMin, 0, m_dble_MinNegValue);
  }
  else
  {
    l_dble_Consigne = 0;
  }

  return l_dble_Consigne;
}
