#include <thermistor.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "SaidaDigital.h"
#include "EntradaDigital.h"
#include "CustomTimer.h"
#include "DisplayMultiplex.h"

// Definição dos pinos do display
const int segmentPins1[7] = {A0, A1, A2, A3, A4, A5, 13};
const int segmentPins2[7] = {A0, A1, A2, A3, A4, A5, 13};
const int display1ControlPin = 3;
const int display2ControlPin = 2;
int displayNumber = 0;

// Instância da classe DisplayMultiplex
DisplayMultiplex Display(segmentPins1, segmentPins2, display1ControlPin, display2ControlPin, displayNumber);

// Botões
const int button4 = 9;
const int button5 = 10;
const int A = 11;
const int B = 12;
const int M = 15;
const int U = 16;
const int E = 13;
const int F = 14;

int TemperaturaDeAbertura = 25;
int TemperaturaDeFechamento = 15;


/* ------ Global Setup BEGIN -------*/

SaidaDigital Rele(8);                       // [Pino 8]
EntradaDigital Botao_CIMA(4,1,1);           // [Pino 04 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital Botao_BAIXO(12,1,1);         // [Pino 12 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital Botao_S(11,1,1);             // [Pino 11 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital SensorJanelaAberta(9,1,1);   // [Pino 09 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital SensorJanelaFechada(10,1,1); // [Pino 10 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
const int eepromTempMaior = 0; // Endereço para tempMaior no EEPROM
const int eepromTempMenor = 2; // Endereço para tempMenor no EEPROM

THERMISTOR thermistor(A7,             // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      10000);         // Value of the series resistor

CustomTimer tmr(3000); // Timer para exibir os caracteres

/* ------ Global Setup END -------*/

void setup() 
{
  Display.Begin();      // Inicializa o display
  Display.setupTimer(); // Configura o Timer para o display

  // Configura o botão 0 e 1 como entrada com pull-up interno
  pinMode(button4, INPUT); 
  pinMode(button5, INPUT);

  carregarTemperaturasDaEEPROM();
}

int funcao = 1;

void loop() 
{
  if(Botao_S.EstaAtivoAguardando())
  {
    tmr.Init(3000);
    if(funcao > 4)
    {
      salvarTemperaturasNaEEPROM();
      funcao = 1;
    }
    else
    { 
      funcao++;
    }
  }
    
  if (funcao == 1) 
  {
    if(!tmr.Finished())
    {
      Display.displayLetra(M, A);
    }
    else
    {
      janelaManual();
    }
  } 
  else if (funcao == 2) 
  {
    if(!tmr.Finished())
    {
      Display.displayLetra(A, U);
    }
    else
    {
       temperaturaAutomatica();
    }
  } 
  else if (funcao == 3) 
  {
    if(!tmr.Finished())
    {
      Display.displayLetra(F, E);
    }
    else
    {
      ajustarTemperaturaDeFechamento();
    }
  } 
  else if (funcao == 4) 
  {
    if(!tmr.Finished())
    {
      Display.displayLetra(A, B);
    }
    else
    {
        ajustarTemperaturaDeAbertura();
    }
  }
  else
  {
    ;
  }
  
}


void janelaManual() 
{
  int tracos = 999;
  Display.showNumber(tracos); // Exibe tracos

  if(Botao_CIMA.EstaAtivoAguardando())
  {
    if(SensorJanelaFechada.EstaAtivo() && !SensorJanelaAberta.EstaAtivo())
    {
      Rele.Ligar();
      delay(1000);
      Rele.Desligar();
    }
  }
  else if(Botao_BAIXO.EstaAtivoAguardando())
  {
    if(!SensorJanelaFechada.EstaAtivo() && SensorJanelaAberta.EstaAtivo())
    {
      Rele.Ligar();
      delay(1000);
      Rele.Desligar();
    }
  }
}

void temperaturaAutomatica()
{ 
  int temperatura = thermistor.read_int();
  
  Display.showNumber(temperatura); // Exibe a temperatura no display

  //sensor de quando a janela deve parar de abrir e parar de fechar
  if(temperatura <= TemperaturaDeFechamento && digitalRead(button4) == LOW)
  {
    while(temperatura <= TemperaturaDeFechamento && digitalRead(button4) == LOW)
    {
       if (digitalRead(button5) == HIGH)
       {
          Rele.Ligar();
          delay(1000);
          Rele.Desligar();
       }
       else
       {
          Rele.Desligar(); 
       }
    }
  }
  else if(temperatura >= TemperaturaDeAbertura && digitalRead(button5) == LOW)
  {
    while(temperatura >= TemperaturaDeAbertura && digitalRead(button5) == LOW)
    {
      if (digitalRead(button4) == HIGH)
      {
        Rele.Ligar();
        delay(1000);
        Rele.Desligar();
      }
      else
      {
        Rele.Desligar();
      }
    }
  }
  else
  {
    Rele.Desligar();
  }
}


void ajustarTemperaturaDeAbertura() 
{
  if(Botao_CIMA.EstaAtivoAguardando())
  {
    TemperaturaDeAbertura++;
  }
  if(Botao_BAIXO.EstaAtivoAguardando())
  {
    TemperaturaDeAbertura--;
  }
  if(TemperaturaDeAbertura <= TemperaturaDeFechamento)
  {
    TemperaturaDeAbertura = TemperaturaDeFechamento + 1;
  }
  // Exibe a temperatura de abertura no display
  Display.showNumber(TemperaturaDeAbertura); 
}

void ajustarTemperaturaDeFechamento() 
{
  if(Botao_CIMA.EstaAtivoAguardando())
  {
    TemperaturaDeFechamento++;
  }
  if(Botao_BAIXO.EstaAtivoAguardando())
  {
    TemperaturaDeFechamento--;
  }
  if(TemperaturaDeFechamento >= TemperaturaDeAbertura)
  {
    TemperaturaDeFechamento = TemperaturaDeAbertura - 1;
  }
  // Exibe a temperatura de fechamento no display
  Display.showNumber(TemperaturaDeFechamento); 
}

void salvarTemperaturasNaEEPROM()
{
  int tentativas = 0;
  bool sucesso = false;

  // Tentativa de salvar tempMaior
  while (tentativas < 3 && !sucesso)
  {
    // Escrever tempMaior na EEPROM
    EEPROM.put(eepromTempMaior, TemperaturaDeAbertura);

    // Ler tempMaior da EEPROM para verificar se foi salvo corretamente
    int valorLido;
    EEPROM.get(eepromTempMaior, valorLido);

    if (valorLido == TemperaturaDeAbertura)
    {
      sucesso = true; // Salvo com sucesso
    }
    else
    {
      tentativas++;
    }
  }
  
  // Resetar variáveis para a próxima verificação
  tentativas = 0;
  sucesso = false;

  // Tentativa de salvar tempMenor
  while(tentativas < 3 && !sucesso)
  {
    // Escrever tempMenor na EEPROM
    EEPROM.put(eepromTempMenor, TemperaturaDeFechamento);

    // Ler tempMenor da EEPROM para verificar se foi salvo corretamente
    int valorLido;
    EEPROM.get(eepromTempMenor, valorLido);

    if (valorLido == TemperaturaDeFechamento)
    {
      sucesso = true; // Salvo com sucesso
    }
    else
    {
      tentativas++;
    }
  }
}

void carregarTemperaturasDaEEPROM()
{
  EEPROM.get(eepromTempMaior, TemperaturaDeAbertura);
  EEPROM.get(eepromTempMenor, TemperaturaDeFechamento);

  // Verificar se os valores carregados são válidos
  if (TemperaturaDeAbertura <= TemperaturaDeFechamento || TemperaturaDeAbertura > 99 || TemperaturaDeFechamento < 0) 
  {
    TemperaturaDeAbertura = 25;  // Valor padrão
    TemperaturaDeFechamento = 15;  // Valor padrão
  }
}
