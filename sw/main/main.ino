#include <thermistor.h>
#include <TimerOne.h>
#include <EEPROM.h>
#include "SaidaDigital.h"
#include "EntradaDigital.h"
#include "CustomTimer.h"

extern volatile unsigned long timer0_millis;

// Mapeamento dos segmentos do display
const int segmentPins1[7] = {A0, A1, A2, A3, A4, A5, 13};
const int segmentPins2[7] = {A0, A1, A2, A3, A4, A5, 13};

const int display1ControlPin = 3;
const int display2ControlPin = 2;

//botões
const int button4 = 9;
const int button5 = 10;

int TemperaturaDeAbertura = 25;
int TemperaturaDeFechamento = 15;

// Padrões para os números de 0 a 9
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

// Numero que sera exibido nos displays 7seg
int displayNumero = 0;



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

CustomTimer tmr(3000);

/* ------ Global Setup END -------*/

void setup() 
{
  for (int i = 0; i < 7; i++) 
  {
    pinMode(segmentPins1[i], OUTPUT);
    pinMode(segmentPins2[i], OUTPUT);
  }
  // Configurar pinos de controle do primeiro display como saída
  pinMode(display1ControlPin, OUTPUT);
  // Configurar pinos de controle do segundo display como saída
  pinMode(display2ControlPin, OUTPUT);

  // Configura o botão 0 e 1 como entrada com pull-up interno
  pinMode(button4, INPUT); 
  pinMode(button5, INPUT);

  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerIsr);
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
      ForcarDisplayTexto("MA");
    }
    else
    {
      ForcarDisplayTexto("--");
      janelaManual();
    }
  } 
  else if (funcao == 2) 
  {
    if(!tmr.Finished())
    {
      ForcarDisplayTexto("AU");
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
      ForcarDisplayTexto("FE");
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
      ForcarDisplayTexto("AB");
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
  
  displayNumero = temperatura;

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
  displayNumero = TemperaturaDeAbertura;
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
  
  displayNumero = TemperaturaDeFechamento;
}


void displayDigit(const int segmentPins[], int digitos)
{
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(segmentPins[i], binarios[digitos][i]);
  }
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

void ForcarDisplayTexto(String texto)
{
  if (texto == "AB") {
    displayNumero = 11 * 100 + 12;  // Codifica como 11-12
  } else if (texto == "FE") {
    displayNumero = 14 * 100 + 13;  // Codifica como 14-13
  } else if (texto == "MA") {
    displayNumero = 15 * 100 + 11;  // Codifica como 15-11
  } else if (texto == "AU") {
    displayNumero = 11 * 100 + 16;  // Codifica como 11-16
  } else if (texto == "--") {
    displayNumero = 10 * 100 + 10;  // Codifica como 10-10 
  } else 
  {
    displayNumero = 10 * 100 + 10;  // Inválido ... Codifica como 10-10
  }  
}

void timerIsr()
{
  static unsigned int trocadisplay = false;
  
  // Mostra numeros
  if(displayNumero < 100)
  {
    if(trocadisplay)
    {
      int dezenas = displayNumero / 10;
      digitalWrite(display1ControlPin, HIGH);
      digitalWrite(display2ControlPin, LOW);
      displayDigit(segmentPins1, dezenas);
    }
    else
    {
      int unidades = displayNumero % 10;
      digitalWrite(display1ControlPin, LOW);
      digitalWrite(display2ControlPin, HIGH);
      displayDigit(segmentPins2, unidades);
    }
  }
  // Mostra as letras
  else
  {
    if(trocadisplay)
    {
      int dezenas = displayNumero / 100;
      digitalWrite(display1ControlPin, HIGH);
      digitalWrite(display2ControlPin, LOW);
      displayDigit(segmentPins1, dezenas);
    }
    else
    {
      int unidades = displayNumero % 100;
      digitalWrite(display1ControlPin, LOW);
      digitalWrite(display2ControlPin, HIGH);
      displayDigit(segmentPins2, unidades);
    }
 }

  trocadisplay = !trocadisplay;
}
