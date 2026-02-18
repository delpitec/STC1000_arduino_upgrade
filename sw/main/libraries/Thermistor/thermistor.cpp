/**
 * Copyright (c) 2014 panStamp <contact@panstamp.com>
 * 
 * This file is part of the panStamp project.
 * 
 * panStamp  is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * any later version.
 * 
 * panStamp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with panStamp; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 
 * USA
 * 
 * Author: Daniel Berenguer
 * Creation date: 11/05/2014
 *
 * This library simplifies the use of NTC thermistors by using Steinhart
 * simplified formula.
 * http://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
 *
 * Based on procedure explained by Adafruit:
 * https://learn.adafruit.com/thermistor/using-a-thermistor
 *
 * This library works only with the following circuit topology
 *
 * Vcc---NTC---ADC---SERIES_RESISTOR---GND
 */

#include "thermistor.h"
#include "HardwareSerial.h"
// Temperature for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25

// Number of ADC samples
#define NUMSAMPLES         5 

// ADC resolution
#if defined(PANSTAMP_NRG) || defined(ESP_PLATFORM)
#define ADC_RESOLUTION 0xFFF
#else
#define ADC_RESOLUTION 1023
#endif
#define VERBOSE_SENSOR_ENABLED 1
/**
 * THERMISTOR
 * 
 * Class constructor
 *
 * @param adcPin Analog pin where the thermistor is connected
 * @param nomRes Nominal resistance at 25 degrees Celsius
 * @param bCoef beta coefficient of the thermistor
 * @param serialRes Value of the serial resistor
 */
THERMISTOR::THERMISTOR(uint8_t adcPin, uint16_t nomRes, uint16_t bCoef, uint16_t serialRes) 
{
  analogPin = adcPin;
  nominalResistance = nomRes;
  bCoefficient = bCoef;
  serialResistance = serialRes;
}

/**
 * read
 *
 * Read temperature from thermistor
 *
 * @return temperature in 0.1 ºC
 */
int THERMISTOR::read(void)
{
  uint8_t i;
  uint16_t sample;
  float average = 0;

  #ifndef ESP_PLATFORM
  analogReference(DEFAULT);
  #endif

  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++)
  {
    sample = analogRead(analogPin);
    average += sample;
    delay(10);
  }
  average /= NUMSAMPLES;

  #ifdef VERBOSE_SENSOR_ENABLED
  Serial.print("Average analog reading ");
  Serial.println(average);
  #endif

  Serial.print("TESTE: ");
  Serial.println(average * 3.3 / 4095.0);

  // convert the value to resistance
  average = ADC_RESOLUTION / average - 1;
  average = serialResistance * average;

  #ifdef VERBOSE_SENSOR_ENABLED
  Serial.print("Thermistor resistance "); 
  Serial.println(average);
  #endif

  float steinhart;
  steinhart =  medirResistor(analogPin, nominalResistance, 1.10) / nominalResistance;     // (R/Ro)
  #ifdef PANSTAMP_NRG
  steinhart = logf(steinhart);                 // ln(R/Ro)
  #else
  steinhart = log(steinhart);                  // ln(R/Ro)
  #endif
  steinhart /= bCoefficient;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  #ifdef VERBOSE_SENSOR_ENABLED
  Serial.print("Temperature ");
  Serial.print(steinhart);
  Serial.println(" *C");
  #endif
  return (int)(steinhart * 10);
  // float THERMISTOR::incrementoFisicoNTC(int contador, float tempAtualC)
}

/**
 * read_int
 *
 * Read temperature from thermistor
 *
 * @return temperature in 1 ºC
 */
int THERMISTOR::read_int(void)
{
  return this->read() / 10;
}

// Mudar para set calibration
// tempAtualC remover e usar dentro direto o read()
float THERMISTOR::incrementoFisicoNTC(int contador, float tempAtualC) {
    const float R0 = 10000.0;
    const float B  = 3950.0;
    const float T0 = 25.0 + 273.15; // ????????? Usar os membros do método ex; nominalResistance
    
    
    // calibration = contador;


    // temperatura atual em Kelvin
    float T = tempAtualC + 273.15;

    // resistência atual do NTC
    float R = R0 * exp(B * (1.0/T - 1.0/T0));

    // deltaR calculado em °C (correto)
    float Tc = tempAtualC;
    float deltaR = 0.56 * Tc * Tc - 14.4 * Tc + 248.0;

    // diminui a resistência -> aumenta temperatura
    R -= deltaR * contador;

    // conversão de volta à temperatura
    float invT = 1.0/T0 + (1.0/B) * log(R / R0);
    float newT = (1.0 / invT) - 273.15;

    return newT;
}

// void SetCalibration()
// float ReadCalibrated()
//{
//    float invT = 1.0/T0 + (1.0/B) * log(R / R0);
//    float newT = (1.0 / invT) - 273.15;
//    return newT
//}


float THERMISTOR::medirResistor(int adcPin, float R_fixed, float fatorCorrecao, int numAmostras) {
    long soma = 0;
    
    // Ler várias vezes para reduzir ruído
    for(int i = 0; i < numAmostras; i++){
        soma += analogRead(adcPin);
    }
    
    float adcMedia = soma / (float)numAmostras;
    
    // Converter ADC para tensão
    float Vadc = adcMedia * 3.3 / 4095.0;
    
    // Aplicar fator de correção
    Vadc *= fatorCorrecao;
    
    // Evitar divisão por zero
    if (Vadc >= 3.3) Vadc = 3.29;
    if (Vadc <= 0.0) Vadc = 0.01;
    // Calcular resistência do resistor ligado ao GND
    float R_var = R_fixed * (Vadc / (3.3 - Vadc));
    return R_var;
}

int THERMISTOR::read_inverted(void)
{
  // Primeiro obtém a leitura normal
  int normal_reading = this->read();
  
  // Define os limites esperados (ajuste conforme seu NTC)
  const int min_expected = -100;   // -10.0°C em décimos
  const int max_expected = 800;    // 80.0°C em décimos
  
  // Inverte a leitura: (max + min) - leitura_atual
  int inverted = (max_expected + min_expected) - normal_reading;
  
  #ifdef VERBOSE_SENSOR_ENABLED
    Serial.print("Normal: ");
    Serial.print(normal_reading / 10.0);
    Serial.print("°C | Inverted: ");
    Serial.print(inverted / 10.0);
    Serial.println("°C");
  #endif
  
  return inverted;
}
