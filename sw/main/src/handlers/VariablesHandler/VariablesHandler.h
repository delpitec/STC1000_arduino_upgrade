#ifndef VARIABLES_HANDLER_H
#define VARIABLES_HANDLER_H
#include "Arduino.h"

const int MODO_MANUAL            = 1;
const int MODO_AUTOMATICO        = 2;
const int MODO_CALIBRACAO        = 3;
const int MODO_CONFIG_FECHAMENTO = 4;
const int MODO_CONFIG_ABERTURA   = 5;
const int MODO_INDEFINIDO        = 6; // -> Este deve sempre ser o último!

const int CMD_ABRIR_JANELA       = 1;
const int CMD_FECHAR_JANELA      = -1;
const int CMD_FAZER_NADA         = 0;

class VariablesHandler
{
    public:
        VariablesHandler();
        float  GetTemperaturaAtual() const;
        int    GetTemperaturaDeAbertura() const;
        int    GetTemperaturaDeFechamento() const;
        int    GetCalibracaoDeTemperatura() const;
        int    GetComandoJanela() const;
        int    GetModo() const;
        String GetSSID() const;
        String GetSenha() const;
        void   SetTemperaturaAtual(const float valor);
        void   SetTemperaturaDeAbertura(const int valor);
        void   SetTemperaturaDeFechamento(const int valor);
        void   SetCalibracaoDeTemperatura(const int valor);
        void   SetComandoJanela(const int cmd);
        void   SetModo(const int NovoModo);
        void   SetSSID(const String NovoSSID);
        void   SetSenha(const String NovaSenha);
    private:
        int    m_Modo;
        float  m_TemperaturaAtual;
        int    m_TemperaturaDeAbertura;
        int    m_TemperaturaDeFechamento;
        int    m_CalibracaoDeTemperatura;
        bool   m_ComandoJanela;
        String m_SSID;
        String m_Senha;
};

#endif
