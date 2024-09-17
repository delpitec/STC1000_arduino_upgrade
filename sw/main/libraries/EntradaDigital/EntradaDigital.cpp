#include <Arduino.h>
#include "EntradaDigital.h"

EntradaDigital::EntradaDigital(unsigned int pino, unsigned int modo, unsigned int pullUpInterno, unsigned int debounce)
{
	_Pino = pino;
	_Modo = modo;
	_PullUpInterno = pullUpInterno;
    _Debounce = Bounce();

	if(_PullUpInterno)
		pinMode(_Pino, INPUT_PULLUP);
	else
		pinMode(_Pino, INPUT);

	_Debounce.attach(_Pino);
    _Debounce.interval(debounce); 
}

void EntradaDigital::AtualizaLeitura()
{
	_Debounce.update();
}

bool EntradaDigital::EstaAtivo()
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

bool EntradaDigital::TrasicaoAtivo()
{
		if(!_Modo)
	{
		return _Debounce.fell();
	}
	else
	{
		return _Debounce.rose();
	}
}

