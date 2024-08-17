#include <Arduino.h>
#include "SaidaDigital.h"

// Construtor da classe Relay
SaidaDigital::SaidaDigital(unsigned int pino, unsigned int modo)
{
  _Pino = pino;
  _Modo = modo;
  pinMode(_Pino, OUTPUT);
  Desligar();
}

// Método para ativar o relé
void SaidaDigital::Ligar()
{
	if(!_Modo)
	{
		digitalWrite(_Pino, HIGH);
	}
	else
	{
		digitalWrite(_Pino, LOW);
	}
}

// Método para desativar o relé
void SaidaDigital::Desligar()
{
	if(!_Modo)
	{
		digitalWrite(_Pino, LOW);
	}
	else
	{
		digitalWrite(_Pino, HIGH);
	}
}

void SaidaDigital::Inverter()
{
  int const EstadoAtual = digitalRead(_Pino); 
  if (EstadoAtual == HIGH)
  {
    Desligar();
  } 
  else 
  {
    Ligar();
  }
}