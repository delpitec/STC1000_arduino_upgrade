#include "DisplayMultiplex.h"

hw_timer_t* DisplayMultiplex::_timer = NULL;

int DisplayMultiplex::pino_a = 32;
int DisplayMultiplex::pino_b = 33;
int DisplayMultiplex::pino_c = 25;
int DisplayMultiplex::pino_d = 26;
int DisplayMultiplex::pino_e = 27;
int DisplayMultiplex::pino_f = 14;
int DisplayMultiplex::pino_g = 4;

bool DisplayMultiplex::Segmentos[31] = {false};

void DisplayMultiplex::Begin() {
   _timer = timerBegin(1500);
   timerAttachInterrupt(_timer, &Timer0_ISR);
   timerAlarm(_timer, 1, true, 0);
}

const bool gabarito[3][17][7] = {
  // Display 1
  {
    {true,  true,  true,  true,  true,  true,  false},  // 0
    {true, true,  false,  false, false, false, false},   // 1
    {true,  false,  true, true,  false,  true, true},    // 2
    {true,  true,  true,  false,  false, true, true},    // 3
    {true, true,  false,  false, true, false,  true},    // 4
    {false,  true, true,  false,  true, true,  true},    // 5
    {false,  true, true,  true,  true,  true,  true},    // 6
    {true,  true,  false,  false, false, true, false},   // 7
    {true,  true,  true,  true,  true,  true,  true},    // 8
    {true,  true,  true,  false,  true, true,  true},     // 9
    {true,  true,  false,  true,  true,  true,  true},    // A
    {false,  true,  true,  true,  true,  false,  true},   // B
    {false,  false,  false,  true,  true,  true,  true},  // F
    {true,  true,  false,  true,  true,  true,  false},   // M
    {false,  false,  true,  true,  true,  true,  true},   // E
    {true,  true,  true,  true,  true,  false,  false},   // U
    {false,  false,  false,  false,  false,  false,  true}   // -


  },

  // Display 2 (repete o mesmo padrão de segmentos)
  {
    {true,  true,  true,  true,  true,  true,  false},
    {false, true,  true,  false, false, false, false},   // 1
    {true,  true,  false, true,  true,  false, true},    // 2
    {true,  true,  true,  true,  false, false, true},    // 3
    {false, true,  true,  false, false, true,  true},    // 4
    {true,  false, true,  true,  false, true,  true},    // 5
    {true,  false, true,  true,  true,  true,  true},    // 6
    {true,  true,  true,  false, false, false, false},   // 7
    {true,  true,  true,  true,  true,  true,  true},    // 8
    {true,  true,  true,  true,  false, true,  true},     // 9
    {true,  true,  true,  false,  true,  true,  true},    // A
    {false,  false,  true,  true,  true,  true,  true},   // B
    {true,  false,  false,  false,  true,  true,  true},  // F
    {true,  true,  true,  false,  true,  true,  false},   // M
    {true,  false,  false,  true,  true,  true,  true},   // E
    {false,  true,  true,  true,  true,  true,  false},   // U
    {false,  false,  false,  false,  false,  false,  true}   // -

  },

  // Display 3 (repete o mesmo padrão de segmentos)
  {
    {true,  true,  true,  true,  true,  true,  false},
    {false, true,  true,  false, false, false, false},   // 1
    {true,  true,  false, true,  true,  false, true},    // 2
    {true,  true,  true,  true,  false, false, true},    // 3
    {false, true,  true,  false, false, true,  true},    // 4
    {true,  false, true,  true,  false, true,  true},    // 5
    {true,  false, true,  true,  true,  true,  true},    // 6
    {true,  true,  true,  false, false, false, false},   // 7
    {true,  true,  true,  true,  true,  true,  true},    // 8
    {true,  true,  true,  true,  false, true,  true},     // 9
    {true,  true,  true,  false,  true,  true,  true},    // A
    {false,  false,  true,  true,  true,  true,  true},   // B
    {true,  false,  false,  false,  true,  true,  true},  // F
    {true,  true,  true,  false,  true,  true,  false}, //  M
    {true,  false,  false,  true,  true,  true,  true},   // E
    {false,  true,  true,  true,  true,  true,  false},   // U
    {false,  false,  false,  false,  false,  false,  true}   // -
  }

};

DisplayMultiplex::DisplayMultiplex() {
    // Configuração inicial dos pinos (pode mudar conforme seu display)
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, OUTPUT);
}



void IRAM_ATTR DisplayMultiplex::Timer0_ISR() {
  
   static int contador = 0;
   contador++;
   if (contador > 31) contador = 0;

    // Máscara de endereços que será usada para definir se deve ligar ou não o segmento
    // 9 10 11 12 13 14 15 -> disp 1 | 16 17 18 19 20 21 22 -> disp 2 | 23 24 25 26 27 28 29 -> disp 3 | 6 7 30 -> miscelaneas

    


    switch (contador){
        case 0: DisplayMultiplex::Segmento_1(DisplayMultiplex::Segmentos[contador]); break;
        case 1:  DisplayMultiplex::Segmento_2(DisplayMultiplex::Segmentos[contador]); break;
        case 2:  DisplayMultiplex::Segmento_3(DisplayMultiplex::Segmentos[contador]); break;
        case 3:  DisplayMultiplex::Segmento_4(DisplayMultiplex::Segmentos[contador]); break;
        case 4:  DisplayMultiplex::Segmento_5(DisplayMultiplex::Segmentos[contador]); break;
        case 5:  DisplayMultiplex::Segmento_6(DisplayMultiplex::Segmentos[contador]); break;
        case 6:  DisplayMultiplex::Segmento_7(DisplayMultiplex::Segmentos[contador]); break;
        case 7:  DisplayMultiplex::Segmento_8(DisplayMultiplex::Segmentos[contador]); break;
        case 8:  DisplayMultiplex::Segmento_9(DisplayMultiplex::Segmentos[contador]); break;
        case 9:  DisplayMultiplex::Segmento_10(DisplayMultiplex::Segmentos[contador]); break;
        case 10: DisplayMultiplex::Segmento_11(DisplayMultiplex::Segmentos[contador]); break;
        case 11: DisplayMultiplex::Segmento_12(DisplayMultiplex::Segmentos[contador]); break;
        case 12: DisplayMultiplex::Segmento_13(DisplayMultiplex::Segmentos[contador]); break;
        case 13: DisplayMultiplex::Segmento_14(DisplayMultiplex::Segmentos[contador]); break;
        case 14: DisplayMultiplex::Segmento_15(DisplayMultiplex::Segmentos[contador]); break;
        case 15: DisplayMultiplex::Segmento_16(DisplayMultiplex::Segmentos[contador]); break;
        case 16: DisplayMultiplex::Segmento_17(DisplayMultiplex::Segmentos[contador]); break;
        case 17: DisplayMultiplex::Segmento_18(DisplayMultiplex::Segmentos[contador]); break;
        case 18: DisplayMultiplex::Segmento_19(DisplayMultiplex::Segmentos[contador]); break;
        case 19: DisplayMultiplex::Segmento_20(DisplayMultiplex::Segmentos[contador]); break;
        case 20: DisplayMultiplex::Segmento_21(DisplayMultiplex::Segmentos[contador]); break;
        case 21: DisplayMultiplex::Segmento_22(DisplayMultiplex::Segmentos[contador]); break;
        case 22: DisplayMultiplex::Segmento_23(DisplayMultiplex::Segmentos[contador]); break;
        case 23: DisplayMultiplex::Segmento_24(DisplayMultiplex::Segmentos[contador]); break;
        case 24: DisplayMultiplex::Segmento_25(DisplayMultiplex::Segmentos[contador]); break;
        case 25: DisplayMultiplex::Segmento_26(DisplayMultiplex::Segmentos[contador]); break;
        case 26: DisplayMultiplex::Segmento_27(DisplayMultiplex::Segmentos[contador]); break;
        case 27: DisplayMultiplex::Segmento_28(DisplayMultiplex::Segmentos[contador]); break;
        case 28: DisplayMultiplex::Segmento_29(DisplayMultiplex::Segmentos[contador]); break;
        case 29: DisplayMultiplex::Segmento_30(DisplayMultiplex::Segmentos[contador]); break;
        case 30: DisplayMultiplex::Segmento_31(DisplayMultiplex::Segmentos[contador]); break;
    }
}

// Atualiza a mascara Segmentos 
void DisplayMultiplex::showNumber(float val)
{
    if(val < 0){
        val = val * -1;
        Segmentos[30] = true;
    }
    int dezena = (int)val / 10;
    int unidade = (int)val % 10;
    int decimais = val;
    int decimal = (val - decimais) * 10;

    for(int i = 8; i < 15; i++)
    {
        DisplayMultiplex::Segmentos[i] = gabarito[0][dezena][i-8]; // display 1
    }

    for(int i = 15; i < 22; i++) // -> rever valores
    {
        DisplayMultiplex::Segmentos[i] = gabarito[1][unidade][i-15]; // display 2
    }

    for(int i = 22; i < 29; i++) // -> rever valores
    {
        DisplayMultiplex::Segmentos[i] = gabarito[2][decimal][i-22]; // display 3
    }

    Segmentos[29] = true;
    Segmentos[6] = true;
    Segmentos[5] = true;

}


void DisplayMultiplex::displayLetra(int l1, int l2)
{
    if(l1 == A && l2 == U)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][A][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][U][i-15]; // display 2
        }
    } 
    if(l1 == A && l2 == B)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][A][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][B][i-15]; // display 2
        }
    }
    if(l1 == F && l2 == E)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][F][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][E][i-15]; // display 2
        }
    }
    if(l1 == A && l2 == F)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][A][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][F][i-15]; // display 2
        }
    }
    if(l1 == M && l2 == A)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][M][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][A][i-15]; // display 2
        }
    }
    if(l1 == tracos && l2 == tracos)
    {
        for(int i = 8; i < 15; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[0][tracos][i-8]; // display 1
        }
        for(int i = 15; i < 22; i++)
        {
            DisplayMultiplex::Segmentos[i] = gabarito[1][tracos][i-15]; // display 2
        }
    }


    for(int i = 22; i < 29; i++)
    {
        DisplayMultiplex::Segmentos[i] = false;
    }

    Segmentos[30] = false;
    Segmentos[29] = false;
    Segmentos[6] = false;
    Segmentos[5] = false;

}

void DisplayMultiplex::Segmetos_Limpar() {
    digitalWrite(pino_a, LOW);
    digitalWrite(pino_b, LOW);
    digitalWrite(pino_c, LOW);
    digitalWrite(pino_d, LOW);
    digitalWrite(pino_e, LOW);
    digitalWrite(pino_f, LOW);
    digitalWrite(pino_g, LOW);
}

void DisplayMultiplex::Segmento_1(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, OUTPUT);

    if(ligar)
    {
        digitalWrite(pino_f, HIGH);
        digitalWrite(pino_g, LOW);
    }
    else
    {
        digitalWrite(pino_f, LOW);
        digitalWrite(pino_g, LOW);
    }

}

void DisplayMultiplex::Segmento_2(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, OUTPUT);

    if(ligar)
    {
        digitalWrite(pino_g, HIGH);
        digitalWrite(pino_f, LOW);
    }
    else
    {
        digitalWrite(pino_g, LOW);
        digitalWrite(pino_f, LOW);
    }
}

void DisplayMultiplex::Segmento_3(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_f, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_f, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_4(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, OUTPUT);

    if(ligar)
    {
        digitalWrite(pino_g, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_g, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_5(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_f, HIGH);
        digitalWrite(pino_c, LOW);
    }
    else
    {
        digitalWrite(pino_f, LOW);
        digitalWrite(pino_c, LOW);
    }
}

void DisplayMultiplex::Segmento_6(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_d, HIGH);
        digitalWrite(pino_c, LOW);
    }
    else
    {
        digitalWrite(pino_d, LOW);
        digitalWrite(pino_c, LOW);
    }
}

void DisplayMultiplex::Segmento_7(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_f, HIGH);
        digitalWrite(pino_d, LOW);
    }
    else
    {
        digitalWrite(pino_f, LOW);
        digitalWrite(pino_d, LOW);
    }
}

void DisplayMultiplex::Segmento_8(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, OUTPUT);

    if(ligar)
    {
        digitalWrite(pino_e, HIGH);
        digitalWrite(pino_g, LOW);
    }
    else
    {
        digitalWrite(pino_e, LOW);
        digitalWrite(pino_g, LOW);
    }
}

void DisplayMultiplex::Segmento_9(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_c, HIGH);
        digitalWrite(pino_a, LOW);
    }
    else
    {
        digitalWrite(pino_c, LOW);
        digitalWrite(pino_a, LOW);
    }
}

void DisplayMultiplex::Segmento_10(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_a, HIGH);
        digitalWrite(pino_d, LOW);
    }
    else
    {
        digitalWrite(pino_a, LOW);
        digitalWrite(pino_d, LOW);
    }
}

void DisplayMultiplex::Segmento_11(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_a, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_a, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_12(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_d, HIGH);
        digitalWrite(pino_a, LOW);
    }
    else
    {
        digitalWrite(pino_d, LOW);
        digitalWrite(pino_a, LOW);
    }
}

void DisplayMultiplex::Segmento_13(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_a, HIGH);
        digitalWrite(pino_b, LOW);
    }
    else
    {
        digitalWrite(pino_a, LOW);
        digitalWrite(pino_b, LOW);
    }
}

void DisplayMultiplex::Segmento_14(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_b, HIGH);
        digitalWrite(pino_a, LOW);
    }
    else
    {
        digitalWrite(pino_b, LOW);
        digitalWrite(pino_a, LOW);
    }
}

void DisplayMultiplex::Segmento_15(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_a, HIGH);
        digitalWrite(pino_c, LOW);
    }
    else
    {
        digitalWrite(pino_a, LOW);
        digitalWrite(pino_c, LOW);
    }
}

void DisplayMultiplex::Segmento_16(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_c, HIGH);
        digitalWrite(pino_b, LOW);
    }
    else
    {
        digitalWrite(pino_c, LOW);
        digitalWrite(pino_b, LOW);
    }
}

void DisplayMultiplex::Segmento_17(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_d, HIGH);
        digitalWrite(pino_b, LOW);
    }
    else
    {
        digitalWrite(pino_d, LOW);
        digitalWrite(pino_b, LOW);
    }
}

void DisplayMultiplex::Segmento_18(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_b, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_b, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_19(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_f, HIGH);
        digitalWrite(pino_b, LOW);
    }
    else
    {
        digitalWrite(pino_f, LOW);
        digitalWrite(pino_b, LOW);
    }
}

void DisplayMultiplex::Segmento_20(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_e, HIGH);
        digitalWrite(pino_b, LOW);
    }
    else
    {
        digitalWrite(pino_e, LOW);
        digitalWrite(pino_b, LOW);
    }
}

void DisplayMultiplex::Segmento_21(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_b, HIGH);
        digitalWrite(pino_c, LOW);
    }
    else
    {
        digitalWrite(pino_b, LOW);
        digitalWrite(pino_c, LOW);
    }
}

void DisplayMultiplex::Segmento_22(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, OUTPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_b, HIGH);
        digitalWrite(pino_d, LOW);
    }
    else
    {
        digitalWrite(pino_b, LOW);
        digitalWrite(pino_d, LOW);
    }
}

void DisplayMultiplex::Segmento_23(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_d, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_d, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_24(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_e, HIGH);
        digitalWrite(pino_c, LOW);
    }
    else
    {
        digitalWrite(pino_e, LOW);
        digitalWrite(pino_c, LOW);
    }
}

void DisplayMultiplex::Segmento_25(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_e, HIGH);
        digitalWrite(pino_d, LOW);
    }
    else
    {
        digitalWrite(pino_e, LOW);
        digitalWrite(pino_d, LOW);
    }
}

void DisplayMultiplex::Segmento_26(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, OUTPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_a, HIGH);
        digitalWrite(pino_f, LOW);
    }
    else
    {
        digitalWrite(pino_a, LOW);
        digitalWrite(pino_f, LOW);
    }
}

void DisplayMultiplex::Segmento_27(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_c, HIGH);
        digitalWrite(pino_f, LOW);
    }
    else
    {
        digitalWrite(pino_c, LOW);
        digitalWrite(pino_f, LOW);
    }
}

void DisplayMultiplex::Segmento_28(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_c, HIGH);
        digitalWrite(pino_d, LOW);
    }
    else
    {
        digitalWrite(pino_c, LOW);
        digitalWrite(pino_d, LOW);
    }
}

void DisplayMultiplex::Segmento_29(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, OUTPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, INPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_c, HIGH);
        digitalWrite(pino_e, LOW);
    }
    else
    {
        digitalWrite(pino_c, LOW);
        digitalWrite(pino_e, LOW);
    }
}

void DisplayMultiplex::Segmento_30(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, INPUT);
    pinMode(pino_e, OUTPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_e, HIGH);
        digitalWrite(pino_f, LOW);
    }
    else
    {
        digitalWrite(pino_e, LOW);
        digitalWrite(pino_f, LOW);
    }
}

void DisplayMultiplex::Segmento_31(bool ligar)
{
    Segmetos_Limpar();
    pinMode(pino_a, INPUT);
    pinMode(pino_b, INPUT);
    pinMode(pino_c, INPUT);
    pinMode(pino_d, OUTPUT);
    pinMode(pino_e, INPUT);
    pinMode(pino_f, OUTPUT);
    pinMode(pino_g, INPUT);

    if(ligar)
    {
        digitalWrite(pino_d, HIGH);
        digitalWrite(pino_f, LOW);
    }
    else
    {
        digitalWrite(pino_d, LOW);
        digitalWrite(pino_f, LOW);
    }
}