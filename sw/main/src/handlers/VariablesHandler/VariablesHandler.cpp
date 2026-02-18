#include "VariablesHandler.h"

VariablesHandler::VariablesHandler()
{
    ;
}

float VariablesHandler::GetTemperaturaAtual() const
{
    return m_TemperaturaAtual;
}
int VariablesHandler::GetTemperaturaDeAbertura() const
{
    return m_TemperaturaDeAbertura;
}

int VariablesHandler::GetTemperaturaDeFechamento() const
{
    return m_TemperaturaDeFechamento;
}

int VariablesHandler::GetCalibracaoDeTemperatura() const
{
    return m_CalibracaoDeTemperatura;
}

int VariablesHandler::GetComandoJanela() const
{
    return m_ComandoJanela;
}

int VariablesHandler::GetModo() const
{
    return m_Modo;
}


String VariablesHandler::GetSSID() const
{
    return m_SSID;
}

String VariablesHandler::GetSenha() const
{
    return m_Senha;
}

void VariablesHandler::SetTemperaturaAtual(const float valor)
{
    m_TemperaturaAtual = valor;
}
void VariablesHandler::SetTemperaturaDeAbertura(const int valor)
{
    m_TemperaturaDeAbertura = valor;
}

void VariablesHandler::SetTemperaturaDeFechamento(const int valor)
{
    m_TemperaturaDeFechamento = valor;
}

void VariablesHandler::SetCalibracaoDeTemperatura(const int valor)
{
    m_CalibracaoDeTemperatura = valor;
}

void VariablesHandler::SetComandoJanela(const int cmd)
{
    if(cmd == CMD_ABRIR_JANELA || cmd == CMD_FECHAR_JANELA || cmd == CMD_FAZER_NADA)
    {
        m_ComandoJanela = cmd;
    }
}

void VariablesHandler::SetModo(const int NovoModo)
{
    m_Modo = NovoModo;
}

void VariablesHandler::SetSSID(const String NovoSSID)
{
    m_SSID = NovoSSID;
}

void VariablesHandler::SetSenha(const String NovaSenha)
{
    m_Senha = NovaSenha;
}

