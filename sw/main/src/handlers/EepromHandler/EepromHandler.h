#ifndef EEPROM_HANDLER_H
#define EEPROM_HANDLER_H
#include <Arduino.h>
#include <EEPROM.h>

class EepromHandler
{
    public:
        EepromHandler(
            const int EnderecoTemperaturaAbertura,
            const int EnderecoTemperaturaFechamento,
            const int EnderecoSSID,
            const int EnderecoSenha,
            const int EnderecoCalibracaoDeTemperatura
        );

        int    GetTemperaturaDeAbertura();
        void   SetTemperaturaDeAbertura(const int newTemperaturaDeAbertura, const unsigned int tentativas = 1);
        int    GetTemperaturaDeFechamento();
        void   SetTemperaturaDeFechamento(const int newTemperaturaDeFechamento, const unsigned int tentativas = 1);
        int    GetCalibracaoDeTemperatura();
        void   SetCalibracaoDeTemperatura(const int newCalibracaoDeTemperatura, const unsigned int tentativas = 1);
        String GetSSID();
        void   SetSSID(const String NovoSSID);
        void   SetSenha(const String NovaSenha);
        String GetSenha();
    private:
        const int m_EnderecoTemperaturaAbertura;
        const int m_EnderecoTemperaturaFechamento;
        const int m_EnderecoSSID;
        const int m_EnderecoSenha;
        const int m_EnderecoCalibracaoDeTemperatura;
};

#endif