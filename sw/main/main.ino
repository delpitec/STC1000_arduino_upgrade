#include <thermistor.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "SaidaDigital.h"
#include "EntradaDigital.h"
#include "CustomTimer.h"
#include "DisplayMultiplex.h"

// Instância da classe DisplayMultiplex
DisplayMultiplex Display(3, 2, A0, A1, A2, A3, A4, A5, 13);

int TemperaturaDeAbertura = 25;
int TemperaturaDeFechamento = 15;
bool janelaFechada = false;
bool janelaAberta = false;


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
CustomTimer tmrRele(0);  // Timer de 10 segundos para verificar o sensor após ligar o relé

/* ------ Global Setup END -------*/

void setup() 
{
  Display.Begin();      // Inicializa o display
  Display.setupTimer(); // Configura o Timer para o display

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
      Display.displayLetra(DisplayMultiplex::M, DisplayMultiplex::A);
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
      Display.displayLetra(DisplayMultiplex::A, DisplayMultiplex::U);
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
      Display.displayLetra(DisplayMultiplex::F, DisplayMultiplex::E);
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
      Display.displayLetra(DisplayMultiplex::A, DisplayMultiplex::B);
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
  Display.showNumber(DisplayMultiplex::tracos); // Exibe tracos

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
  if(temperatura <= TemperaturaDeFechamento)
  {
    if(!SensorJanelaFechada.EstaAtivo() && SensorJanelaAberta.EstaAtivo() && janelaFechada == false)
    {
      Rele.Ligar();
      delay(1000);       
      Rele.Desligar();
      tmrRele.Init(10000); 
      janelaFechada = true;
      janelaAberta = false;
    }
    if(janelaFechada == true && tmrRele.Finished())
    {
      if (!SensorJanelaFechada.EstaAtivo())
      {
        Rele.Ligar();  
        delay(1000);       
        Rele.Desligar();
        tmrRele.Init(10000); 
      }
      else
      {
        janelaFechada = false;
      }
    }
  }
  else if(temperatura >= TemperaturaDeAbertura)
  {
    if(SensorJanelaFechada.EstaAtivo() && !SensorJanelaAberta.EstaAtivo() && janelaAberta == false)
    {
      Rele.Ligar();
      delay(1000);
      Rele.Desligar();
      tmrRele.Init(10000); 
      janelaAberta = true;
      janelaFechada = false;
    }
    if(janelaAberta == true && tmrRele.Finished())
    {
      if (!SensorJanelaFechada.EstaAtivo())
      {
        Rele.Ligar();  
        delay(1000);       
        Rele.Desligar();
        tmrRele.Init(10000); 
      }
      else
      {
        janelaAberta = false;
      }
    }
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
  if(TemperaturaDeAbertura >= 100)
  {
    TemperaturaDeAbertura = 99;
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
  if(TemperaturaDeFechamento <= 0)
  {
    TemperaturaDeFechamento = 1;
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
