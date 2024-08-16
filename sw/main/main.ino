#include <TimerOne.h>

extern volatile unsigned long timer0_millis;

// Mapeamento dos segmentos do display
const int segmentPins1[7] = {A0, A1, A2, A3, A4, A5, 13};
const int segmentPins2[7] = {A0, A1, A2, A3, A4, A5, 13};


const int display1ControlPin = 3;
const int display2ControlPin = 2;

// Pinos dos LEDs
const int led1 = 8; // LED para janela aberta

// Pino de leitura analógica
const int sensor = A7;

//botões
const int button1 = 4;
const int button2 = 12;
const int button3 = 11;
const int button4 = 9;
const int button5 = 10;

int tempMaior = 25;
int tempMenor = 15;

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

int displayNumber = 0;

const int tracos;

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

  // Configurar pinos dos LEDs como saída
  pinMode(led1, OUTPUT);
  
  // Configurar pino do sensor de temperatura como entrada
  pinMode(sensor, INPUT);

  // Configura o botão 0 e 1 como entrada com pull-up interno
  pinMode(button1, INPUT);
  pinMode(button2, INPUT); 
  pinMode(button3, INPUT);
  pinMode(button4, INPUT); 
  pinMode(button5, INPUT);

  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerIsr);
}

int funcao = 1;

void loop() 
{
    if(digitalRead(button3) == LOW)
    {
      while(digitalRead(button3) == LOW)
      {
        ;
      }
      DesligarTodosLeds();
      if(funcao > 4)
      {
        noInterrupts();
        timer0_millis = 0;
        interrupts();
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
             ajustarTempMenor();
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
              ajustarTempMaior();
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

void DesligarTodosLeds()
{
    digitalWrite(led1, LOW);
}

void janelaManual() 
{
  displayNumber = tracos;
  
  // Verifica o estado do botão 1 e 2
  if(digitalRead(button1) == LOW)
  {
    while(digitalRead(button1) == LOW)
    {
      ;
    }
    if(digitalRead(button4) == LOW && digitalRead(button5) == HIGH)
    {
      digitalWrite(led1, HIGH);
      delay(1000);
      digitalWrite(led1, LOW);
    }
    if(digitalRead(button5) == LOW && digitalRead(button4) == HIGH)
    {
      digitalWrite(led1, HIGH);
      delay(1000);
      digitalWrite(led1, LOW);
    }
  }
}
void temperaturaAutomatica()
{ 
  int temperatura = lerTemperatura(sensor);

  displayNumber = temperatura;

  //sensor de quando a janela deve parar de abrir e parar de fechar
  if(temperatura <= tempMenor && digitalRead(button4) == LOW)
  {
    while(temperatura <= tempMenor && digitalRead(button4) == LOW)
    {
       if (digitalRead(button5) == HIGH)
       {
          digitalWrite(led1, HIGH);
          delay(1000);
          digitalWrite(led1, LOW);
       }
       else
       {
          digitalWrite(led1, LOW); 
       }
    }
  }
  else if(temperatura >= tempMaior && digitalRead(button5) == LOW)
  {
    while(temperatura >= tempMaior && digitalRead(button5) == LOW)
    {
      if (digitalRead(button4) == HIGH)
      {
        digitalWrite(led1, HIGH);
        delay(1000);
        digitalWrite(led1, LOW);
      }
      else
      {
        digitalWrite(led1, LOW);
      }
    }
  }
  else
  {
    digitalWrite(led1, LOW);
  }
}


void ajustarTempMaior() 
{
  if(digitalRead(button1) == LOW)
  {
    while(digitalRead(button1) == LOW)
    {
      ;
    }
    tempMaior++;
  }
  if(digitalRead(button2) == LOW)
  {
    while(digitalRead(button2) == LOW)
    {
      ;
    }
    tempMaior--;
  }
  if(tempMaior <= tempMenor)
  {
    tempMaior = tempMenor + 1;
  }
  displayNumber = tempMaior;
}

void ajustarTempMenor() 
{
  if(digitalRead(button1) == LOW)
    {
      while(digitalRead(button1) == LOW)
      {
        ;
      }
      tempMenor++;
    }
  if(digitalRead(button2) == LOW)
  {
    while(digitalRead(button2) == LOW)
    {
      ;
    }
    tempMenor--;
  }
  if(tempMenor >= tempMaior)
  {
    tempMenor = tempMaior -1;
  }
  
  displayNumber = tempMenor;
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

void timerIsr()
{
  static bool trocadisplay = false;
  
  if (displayNumber == tracos && millis() >= 3000) 
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
        int dezenas = displayNumber / 10;
        digitalWrite(display1ControlPin, HIGH);
        digitalWrite(display2ControlPin, LOW);
        displayDigit(segmentPins1, dezenas);
      }
      else
      {
        int unidades = displayNumber % 10;
        digitalWrite(display1ControlPin, LOW);
        digitalWrite(display2ControlPin, HIGH);
        displayDigit(segmentPins2, unidades);
      }
    }
  }
  trocadisplay = !trocadisplay;
}