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
    Timer1.initialize(500000); // Inicializa o Timer1 para interrupções a cada 500 ms
    Timer1.attachInterrupt(timerIsr); // Anexa a função de interrupção
}

void DisplayMultiplex::displayLetra(int n1, int n2)
{
    digitalWrite(_display1ControlPin, HIGH);
    digitalWrite(_display2ControlPin, LOW);
    displayDigit(segmentPins1, n1);
    delay(5);

    digitalWrite(_display1ControlPin, LOW);
    digitalWrite(_display2ControlPin, HIGH);
    displayDigit(segmentPins2, n2);
    delay(5);
}

// Função de interrupção para atualização de displays
void DisplayMultiplex::timerIsr()
{
    static bool _trocaDisplay = false;
    
    if(_instance->_displayNumber == tracos)
    {
        if(_trocaDisplay)
        {
            digitalWrite(_instance->_display1ControlPin, HIGH);
            digitalWrite(_instance->_display2ControlPin, LOW);
            _instance->displayDigit(_instance->segmentPins1, 10);
        }
        else
        {
            digitalWrite(_instance->_display1ControlPin, LOW);
            digitalWrite(_instance->_display2ControlPin, HIGH);
            _instance->displayDigit(_instance->segmentPins2, 10);
        }
    }
    else
    {
        if(_trocaDisplay)
        {
            int dezenas = _instance->_displayNumber / 10;
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