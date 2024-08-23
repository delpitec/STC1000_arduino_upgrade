#ifndef DISPLAYMULTIPLEX_H_
#define DISPLAYMULTIPLEX_H_

#include "Arduino.h"
#include <TimerOne.h>

class DisplayMultiplex 
{
public:
    // Construtor
    DisplayMultiplex(const int segmentPins1[], const int segmentPins2[], int display1ControlPin, int display2ControlPin, int displayNumber);
    
    // Inicializa o display
    void Begin();

    // Mostra um número de 0 a 99 no display
    void showNumber(int number);

    // Configura o TimerOne
    void setupTimer();

    // Mostra letras no display
    void displayLetra(int n1, int n2);

private:
    // Função de interrupção para atualização de displays
    static void timerIsr();

    // Função para mostrar um dígito em um display
    void displayDigit(const int segmentPins[], int digitos);

    // Mapeamento dos segmentos para números e caracteres
    const int binarios[17][7] =
    {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}, // 9
        {0, 0, 0, 0, 0, 0, 1}, // -
        {1, 1, 1, 0, 1, 1, 1}, // A 
        {0, 0, 1, 1, 1, 1, 1}, // B 
        {1, 0, 0, 1, 1, 1, 1}, // E 
        {1, 0, 0, 0, 1, 1, 1}, // F 
        {1, 1, 1, 0, 1, 1, 0}, // M
        {0, 1, 1, 1, 1, 1, 0}  // U
    };

    // Pinos de controle dos displays e segmentos
    const int* _segmentPins1;
    const int* _segmentPins2;
    int _display1ControlPin;
    int _display2ControlPin;

    // Número a ser exibido no display
    int _displayNumber;

    // Instância estática para acesso na interrupção
    static DisplayMultiplex* _instance;
};

#endif