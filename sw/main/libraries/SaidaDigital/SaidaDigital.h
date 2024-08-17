#ifndef SAIDADIGITAL_H_
#define SAIDADIGITAL_H_

#include "Arduino.h"

class SaidaDigital {
  public:
  
    SaidaDigital(unsigned int pino, unsigned int modo = 0);
    void Ligar();
    void Desligar();
    void Inverter();

  private:
    unsigned int _Pino; // Pino do Arduino ao qual o relé está conectado
	unsigned int _Modo; // 0: Normal, 1: Saída invertida
};


#endif