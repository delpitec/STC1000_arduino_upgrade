#include "DisplayMultiplex.h"
#include <TimerOne.h>

// Inicialização das variáveis estáticas
DisplayMultiplex* DisplayMultiplex::_instance = nullptr;

DisplayMultiplex::DisplayMultiplex(int display1ControlPin, int display2ControlPin, int a, int b, int c, int d, int e, int f, int g)
{
    // Inicializa os pinos dos segmentos para ambos os displays
    const int tempSegmentPins1[7] = {a, b, c, d, e, f, g};
    const int tempSegmentPins2[7] = {a, b, c, d, e, f, g};
    memcpy(segmentPins1, tempSegmentPins1, sizeof(segmentPins1));
    memcpy(segmentPins2, tempSegmentPins2, sizeof(segmentPins2));

    _display1ControlPin = display1ControlPin;
    _display2ControlPin = display2ControlPin;
    _displayNumber = 0;
    _instance = this;  // Armazena a instância atual
}

// Inicializa o display
void DisplayMultiplex::Begin()
{
    for (int i = 0; i < 7; i++)
    {
        pinMode(segmentPins1[i], OUTPUT);
        pinMode(segmentPins2[i], OUTPUT);
    }
    pinMode(_display1ControlPin, OUTPUT);
    pinMode(_display2ControlPin, OUTPUT);
}

// Mostra um número de 0 a 99 no display
void DisplayMultiplex::showNumber(int number) 
{
    _displayNumber = number;
}

// Configura o TimerOne
void DisplayMultiplex::setupTimer() 
{
    Timer1.initialize(10000); // simulação é 500000 / arduino : 10000
    Timer1.attachInterrupt(timerIsr); // Anexa a função de interrupção
}

void DisplayMultiplex::displayLetra(int L1, int L2)
{
  if (L1 == A && L2 == B){
    showNumber(11 * 100 + 12);  // Codifica como 11-12
  } else if (L1 == F && L2 == E){
    showNumber(14 * 100 + 13);  // Codifica como 14-13
  } else if (L1 == M && L2 == A){
    showNumber(15 * 100 + 11);  // Codifica como 15-11
  } else if (L1 == A && L2 == U) {
    showNumber(11 * 100 + 16);  // Codifica como 11-16
  } else if (L1 == tracos && L2 == tracos) {
    showNumber(10 * 100 + 10);  // Codifica como 10-10 
  } else 
  {
    showNumber(10 * 100 + 10);  // Inválido ... Codifica como 10-10
  }
}

// Função de interrupção para atualização de displays
void DisplayMultiplex::timerIsr()
{
    static bool _trocaDisplay = false;
    
	// Mostra numeros
    if(_instance->_displayNumber < 100)
    {
        if(_trocaDisplay)
        {	int dezenas = _instance->_displayNumber / 10;
            digitalWrite(_instance->_display1ControlPin, HIGH);
            digitalWrite(_instance->_display2ControlPin, LOW);
            _instance->displayDigit(_instance->segmentPins1, dezenas);
        }
        else
        {
			int unidades = _instance->_displayNumber % 10;
            digitalWrite(_instance->_display1ControlPin, LOW);
            digitalWrite(_instance->_display2ControlPin, HIGH);
            _instance->displayDigit(_instance->segmentPins2, unidades);
        }
    }
    // Mostra as letras
	else
    {
        if(_trocaDisplay)
        {
            int dezenas = _instance->_displayNumber / 100;
            digitalWrite(_instance->_display1ControlPin, HIGH);
            digitalWrite(_instance->_display2ControlPin, LOW);
            _instance->displayDigit(_instance->segmentPins1, dezenas);
        }
        else
        {
            int unidades = _instance->_displayNumber % 100;
            digitalWrite(_instance->_display1ControlPin, LOW);
            digitalWrite(_instance->_display2ControlPin, HIGH);
            _instance->displayDigit(_instance->segmentPins2, unidades);
        }
    }
    _trocaDisplay = !_trocaDisplay;
}

// Função para mostrar um dígito em um display
void DisplayMultiplex::displayDigit(const int segmentPins[], int digitos)
{
    for (int i = 0; i < 7; i++)
    {
        digitalWrite(segmentPins[i], binarios[digitos][i]);
    }
}