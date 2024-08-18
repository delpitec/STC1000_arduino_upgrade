#include <TimerOne.h>
#include <EEPROM.h>
#include "SaidaDigital.h"
#include "EntradaDigital.h"

extern volatile unsigned long timer0_millis;

// Mapeamento dos segmentos do display
const int segmentPins1[7] = {A0, A1, A2, A3, A4, A5, 13};
const int segmentPins2[7] = {A0, A1, A2, A3, A4, A5, 13};


const int display1ControlPin = 3;
const int display2ControlPin = 2;

// Pino de leitura analógica
const int sensor = A7;

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

int displayNumero = 0;

const int tracos;

/* ------ Global Setup BEGIN -------*/

SaidaDigital Rele(8);                       // [Pino 8]
EntradaDigital Botao_CIMA(4,1,1);           // [Pino 04 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital Botao_BAIXO(12,1,1);         // [Pino 12 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital Botao_S(11,1,1);             // [Pino 11 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital SensorJanelaAberta(9,1,1);   // [Pino 09 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital SensorJanelaFechada(10,1,1); // [Pino 10 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
const int eepromTempMaior = 0; // Endereço para tempMaior no EEPROM
const int eepromTempMenor = 2; // Endereço para tempMenor no EEPROM

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

  // Configurar pino do sensor de temperatura como entrada
  pinMode(sensor, INPUT);

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
    Rele.Desligar();
    if(funcao > 4)
    {
      noInterrupts();
      timer0_millis = 0;
      interrupts();
      salvarTemperaturasNaEEPROM();
      funcao = 1;
    }
    else
    {
      noInterrupts();
      timer0_millis = 0;
      interrupts();
      funcao++;
    }
  }
    
  if (funcao == 1) 
  {
    if(millis() < 3000)
    {
      digitalWrite(display1ControlPin, HIGH);
      digitalWrite(display2ControlPin, LOW);
      displayDigit(segmentPins1, 15);
      delay(5);
  
      digitalWrite(display1ControlPin, LOW);
      digitalWrite(display2ControlPin, HIGH);
      displayDigit(segmentPins2, 11);
      delay(5);
    }
    else
    {
      janelaManual();
    }
  } 
  else if (funcao == 2) 
  {
    if(millis() < 3000)
    {
        digitalWrite(display1ControlPin, HIGH);
        digitalWrite(display2ControlPin, LOW);
        displayDigit(segmentPins1, 11);
        delay(5);
    
        digitalWrite(display1ControlPin, LOW);
        digitalWrite(display2ControlPin, HIGH);
        displayDigit(segmentPins2, 16);
        delay(5);
    }
    else
    {
       temperaturaAutomatica();
    }
  } 
  else if (funcao == 3) 
  {
    if(millis() < 3000)
    {
        digitalWrite(display1ControlPin, HIGH);
        digitalWrite(display2ControlPin, LOW);
        displayDigit(segmentPins1, 14);
        delay(5);
    
        digitalWrite(display1ControlPin, LOW);
        digitalWrite(display2ControlPin, HIGH);
        displayDigit(segmentPins2, 13);
        delay(5);
    }
    else
    {
       ajustarTemperaturaDeFechamento();
    }
  } 
  else if (funcao == 4) 
  {
    if(millis() < 3000)
    {
        digitalWrite(display1ControlPin, HIGH);
        digitalWrite(display2ControlPin, LOW);
        displayDigit(segmentPins1, 11);
        delay(5);
    
        digitalWrite(display1ControlPin, LOW);
        digitalWrite(display2ControlPin, HIGH);
        displayDigit(segmentPins2, 12);
        delay(5);
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
  
  if(millis() >= 3456000000)// 40 dias
  {
      noInterrupts();
      timer0_millis = 0;
      interrupts();
  }
}


void janelaManual() 
{
  displayNumero = tracos;
  
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
  int temperatura = lerTemperatura(sensor);

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

int lerTemperatura(int pin)
{
  int sensorValor = analogRead(sensor);  // Ler o valor analógico do sensor (0-1023)
  int temp = map(sensorValor, 0, 1023, 0, 99);  // Converter para 0-99 graus
  return temp;
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

void timerIsr()
{
  static bool trocadisplay = false;
  
  if (displayNumero == tracos && millis() >= 3000) 
  {
    if (trocadisplay) 
    {
      digitalWrite(display1ControlPin, HIGH);
      digitalWrite(display2ControlPin, LOW);
      displayDigit(segmentPins1, 10);
    } 
    else 
    {
      digitalWrite(display1ControlPin, LOW);
      digitalWrite(display2ControlPin, HIGH);
      displayDigit(segmentPins2, 10); 
    }
  } 
  else 
  {
    if(millis() >= 3000)
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
  }
  trocadisplay = !trocadisplay;
}