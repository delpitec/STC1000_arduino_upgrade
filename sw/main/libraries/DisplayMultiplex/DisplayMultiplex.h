#ifndef DISPLAYMULTIPLEX_H_
#define DISPLAYMULTIPLEX_H_

#include "Arduino.h"
#include <TimerOne.h>

// Letras estáticas (A, B, M, U, E, F)
const int A = 11;
const int B = 12;
const int M = 15;
const int U = 16;
const int E = 13;
const int F = 14;
const int tracos = 110;

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


class DisplayMultiplex 
{
public:
    // Construtor
    DisplayMultiplex(int display1ControlPin, int display2ControlPin, int a, int b, int c, int d, int e, int f, int g);

    // Inicializa o display
    void Begin();

    // Mostra um número de 0 a 99 no display
    void showNumber(int number);

    // Configura o TimerOne
    void setupTimer();

    // Mostra letras no display
    void displayLetra(int L1, int L2);

private:
    // Função de interrupção para atualização de displays
    static void timerIsr();

    // Função para mostrar um dígito em um display
    void displayDigit(const int segmentPins[], int digitos);

    // Array pinos
    int segmentPins1[7];
    int segmentPins2[7];

    // Pinos de controle dos displays e segmentos
    int _display1ControlPin;
    int _display2ControlPin;

    // Número a ser exibido no display
    int _displayNumber;

    // Instância estática para acesso na interrupção
    static DisplayMultiplex* _instance;
};

#endif