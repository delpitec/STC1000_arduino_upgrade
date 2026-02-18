#include "EepromHandler.h"

EepromHandler::EepromHandler(
    const int EnderecoTemperaturaAbertura,
    const int EnderecoTemperaturaFechamento,
    const int EnderecoSSID,
    const int EnderecoSenha,
    const int EnderecoCalibracaoDeTemperatura) :
    m_EnderecoTemperaturaAbertura(EnderecoTemperaturaAbertura),
    m_EnderecoTemperaturaFechamento(EnderecoTemperaturaFechamento),
    m_EnderecoSSID(EnderecoSSID),
    m_EnderecoSenha(EnderecoSenha),
    m_EnderecoCalibracaoDeTemperatura(EnderecoCalibracaoDeTemperatura)
{
    EEPROM.begin(512);
}

int EepromHandler::GetTemperaturaDeAbertura()
{
    int ret = 0U;
    EEPROM.get(m_EnderecoTemperaturaAbertura, ret);
    return ret;
}

void EepromHandler::SetTemperaturaDeAbertura(const int newTemperaturaDeAbertura, const unsigned int tentativas)
{
    for(unsigned int i = 0; i < tentativas; i++)
    {
        EEPROM.put(m_EnderecoTemperaturaAbertura, newTemperaturaDeAbertura);
        if (EEPROM.commit())
        {
            int valorLido;
            EEPROM.get(m_EnderecoTemperaturaAbertura, valorLido);

            if (valorLido == newTemperaturaDeAbertura)
            {
                break;
            }
        }
    }
}

int EepromHandler::GetTemperaturaDeFechamento()
{
    int ret = 0U;
    EEPROM.get(m_EnderecoTemperaturaFechamento, ret);
    return ret;
}

void EepromHandler::SetTemperaturaDeFechamento(const int newTemperaturaDeFechamento, const unsigned int tentativas)
{
    for(unsigned int i = 0; i < tentativas; i++)
    {
        EEPROM.put(m_EnderecoTemperaturaFechamento, newTemperaturaDeFechamento);

        if (EEPROM.commit())
        {
            int valorLido;
            EEPROM.get(m_EnderecoTemperaturaFechamento, valorLido);

            if (valorLido == newTemperaturaDeFechamento)
            {
                break;
            }
        }
        delay(10);
    }
}

int EepromHandler::GetCalibracaoDeTemperatura()
{
    int ret = 0U;
    EEPROM.get(m_EnderecoCalibracaoDeTemperatura, ret);
    return ret;
}

void EepromHandler::SetCalibracaoDeTemperatura(const int newCalibracaoDeTemperatura, const unsigned int tentativas)
{
    for(unsigned int i = 0; i < tentativas; i++)
    {
        EEPROM.put(m_EnderecoCalibracaoDeTemperatura, newCalibracaoDeTemperatura);

        if (EEPROM.commit())
        {
            int valorLido;
            EEPROM.get(m_EnderecoCalibracaoDeTemperatura, valorLido);

            if (valorLido == newCalibracaoDeTemperatura)
            {
                break;
            }
        }
        delay(10);
    }
}


String EepromHandler::GetSSID()
{
    char ssid[32];
    bool isEmpty = true;

    for(int i = 0; i < 31; i++)
    {
        ssid[i] = EEPROM.read(m_EnderecoSSID + i);
        if(ssid[i] != 0 && ssid[i] != 255)
        {
            isEmpty = false;
        }
        if(ssid[i] == 0)
        {
            break;
        }
    }
    ssid[31] = '\0';
    return isEmpty ? "" : String(ssid);
}

void EepromHandler::SetSSID(const String NovoSSID)
{
    for(int i = m_EnderecoSSID; i < m_EnderecoSSID + 32; i++)
    {   // Erase
        EEPROM.write(i, 0);
    }
    for(int i = 0; i < NovoSSID.length(); i++)
    {   // Write
        EEPROM.write(m_EnderecoSSID + i, NovoSSID[i]);
    }
    EEPROM.commit();
}

void EepromHandler::SetSenha(const String NovaSenha)
{
    for(int i = m_EnderecoSenha; i < m_EnderecoSenha + 64; i++)
    {   // Erase
        EEPROM.write(i, 0);
    }
    for(int i = 0; i < NovaSenha.length(); i++)
    {   // Write
        EEPROM.write(m_EnderecoSenha + i, NovaSenha[i]);
    }
    EEPROM.commit();
}

String EepromHandler::GetSenha()
{
    char password[64];
    bool isEmpty = true;

    for(int i = 0; i < 63; i++)
    {
        password[i] = EEPROM.read(m_EnderecoSenha + i);
        if(password[i] != 0 && password[i] != 255)
        {
            isEmpty = false;
        }
        if(password[i] == 0)
        {
            break;
        }
    }
    password[63] = '\0';

    return isEmpty ? "" : String(password);
}
