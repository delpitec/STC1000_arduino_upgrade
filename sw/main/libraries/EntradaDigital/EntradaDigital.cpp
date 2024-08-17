#include <Arduino.h>
#include "EntradaDigital.h"

EntradaDigital::EntradaDigital(unsigned int pino, unsigned int modo, unsigned int pullUpInterno)
{
	_Pino = pino;
	_Modo = modo;
	_PullUpInterno = pullUpInterno;
  
	if(_PullUpInterno)
		pinMode(pino, INPUT_PULLUP);
	else
		pinMode(pino, INPUT);
}

int EntradaDigital::EstaAtivo()
{
	if(!_Modo)
	{
		return digitalRead(_Pino);
	}
	else
	{
		return !digitalRead(_Pino);
	}
}

/* 
 * @brief Verifica se uma entrada está ativa e caso esteja retem a 
 *        saída da funcao enquanto ela não for for desativada.
 * @return 
 *        'true' se a entrada passou por um estado ativo e agora está desativada.
 *        'false' se não foi ativada.
 */
int EntradaDigital::EstaAtivoAguardando()
{
	if(EstaAtivo())
	{	
		while(EstaAtivo())
			; // Aguarda Desativar
		return true;
	}
	else
	{
		return false;
	}
}
