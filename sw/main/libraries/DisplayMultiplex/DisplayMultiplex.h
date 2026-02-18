#ifndef DISPLAYMULTIPLEX_H
#define DISPLAYMULTIPLEX_H

#include <Arduino.h>
#include <math.h>



class DisplayMultiplex {
public:

    static const int A = 10;
    static const int B = 11;
    static const int F = 12;
    static const int M = 13;
    static const int E = 14;
    static const int U = 15;
    static const int tracos = 16;


    void Begin();  // intervalo em microssegundos
    DisplayMultiplex();
    void showNumber(float valor);
    void displayLetra(int l1, int l2);
    static void IRAM_ATTR Timer0_ISR();

    // Funções de segmentos (cada uma define como acionar os pinos)
    static void Segmento_1(bool ligar);
    static void Segmento_2(bool ligar);
    static void Segmento_3(bool ligar);
    static void Segmento_4(bool ligar);
    static void Segmento_5(bool ligar);
    static void Segmento_6(bool ligar);
    static void Segmento_7(bool ligar);
    static void Segmento_8(bool ligar);
    static void Segmento_9(bool ligar);
    static void Segmento_10(bool ligar);
    static void Segmento_11(bool ligar);
    static void Segmento_12(bool ligar);
    static void Segmento_13(bool ligar);
    static void Segmento_14(bool ligar);
    static void Segmento_15(bool ligar);
    static void Segmento_16(bool ligar);
    static void Segmento_17(bool ligar);
    static void Segmento_18(bool ligar);
    static void Segmento_19(bool ligar);
    static void Segmento_20(bool ligar);
    static void Segmento_21(bool ligar);
    static void Segmento_22(bool ligar);
    static void Segmento_23(bool ligar);
    static void Segmento_24(bool ligar);
    static void Segmento_25(bool ligar);
    static void Segmento_26(bool ligar);
    static void Segmento_27(bool ligar);
    static void Segmento_28(bool ligar);
    static void Segmento_29(bool ligar);
    static void Segmento_30(bool ligar);
    static void Segmento_31(bool ligar);
    static void Segmetos_Limpar();


private:

    static bool Segmentos[31];

    static hw_timer_t* _timer;

    // Exemplo de pinos (substitua conforme seu hardware)
    static int pino_a;
    static int pino_b;
    static int pino_c;
    static int pino_d;
    static int pino_e;
    static int pino_f;
    static int pino_g;

};

#endif
