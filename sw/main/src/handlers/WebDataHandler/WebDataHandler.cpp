#include "WebDataHandler.h"

WebDataHandler::WebDataHandler(VariablesHandler& handler) :
    m_VariablesHdler(handler)
{
    ;
}

void WebDataHandler::AbrirJanela() const
{
    if(!m_VariablesHdler.GetModo() != MODO_AUTOMATICO)
    {
        m_VariablesHdler.SetComandoJanela(CMD_ABRIR_JANELA);
    }
}

void WebDataHandler::FecharJanela() const
{
    if(!m_VariablesHdler.GetModo() != MODO_AUTOMATICO)
    {
        m_VariablesHdler.SetComandoJanela(CMD_FECHAR_JANELA);
    }
}

bool WebDataHandler::TrocarModo(const unsigned int NovoModo) const
{
    bool Sucesso = false;
    const bool NovoModoValido = (NovoModo == MODO_MANUAL) || (NovoModo == MODO_AUTOMATICO);
    if(NovoModoValido)
    {
        m_VariablesHdler.SetModo(NovoModo);
        Sucesso = true;
    }
    return Sucesso;
}

bool WebDataHandler::TrocarTemperaturaDeAbertura(const int NovaTemp)
{
    bool Sucesso = false;
    if((NovaTemp < 99) && (NovaTemp > -99))
    {
        m_VariablesHdler.SetTemperaturaDeAbertura(NovaTemp);
        Sucesso = true;
    }
    return Sucesso;
}

bool WebDataHandler::TrocarTemperaturaDeFechamento(const int NovaTemp) const
{
    bool Sucesso = false;
    if((NovaTemp < 99) && (NovaTemp > -99))
    {
        m_VariablesHdler.SetTemperaturaDeFechamento(NovaTemp);
        Sucesso = true;
    }
    return Sucesso;
}

void WebDataHandler::TrocarSSID(const String NovoSSid) const
{
    m_VariablesHdler.SetSSID(NovoSSid);
}

bool WebDataHandler::TrocarSenha(const String NovaSenha) const
{
    bool ret = false;
    if (NovaSenha.length() < 8)
    {
        m_VariablesHdler.SetSenha(NovaSenha);
        ret = true;
    }
    return ret;
}

bool WebDataHandler::EstaEmModoAutomatico() const
{
    return m_VariablesHdler.GetModo() == MODO_AUTOMATICO;
}
