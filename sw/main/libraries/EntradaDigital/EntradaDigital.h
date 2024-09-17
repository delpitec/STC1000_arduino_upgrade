#ifndef ENTRADADIGITAL_H_
#define ENTRADADIGITAL_H_

#include "Arduino.h"
#include <Bounce2.h>

class EntradaDigital {
  public:
  
    EntradaDigital(unsigned int pino, unsigned int modo = 0, unsigned int pullUpInterno = 0, unsigned int debounce = 40);
    void AtualizaLeitura();
    
    bool EstaAtivo();
    bool TrasicaoAtivo();

  private:
    unsigned int _Pino; // Pino do Arduino ao qual o relé está conectado
	  unsigned int _Modo; // 0: Normal, 1: Entrada invertida
	  unsigned int _PullUpInterno; // 0: Sem PullUp interno, 1: Com Pull Up Interno
    Bounce _Debounce;

};


#endif