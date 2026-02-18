#include <WiFi.h>
#include <EEPROM.h>
#include "WifiHandler.h"

float m_temperaturaAtual = 0.0;

WiFiHandler::WiFiHandler(WebDataHandler& WebDataHdlr) :
    m_WebDataHdlr(WebDataHdlr),
    m_server(80)
{
    ;
}

void WiFiHandler::beginClient(const String WifiSSID, const String WifiSenha){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WifiSSID.c_str(), WifiSenha.c_str());
    IPAddress ip = WiFi.softAPIP();

    Serial.print("AP iniciado com IP: ");
    Serial.println(ip);

    setupRoutes();
    m_server.begin();
    Serial.println("WebServer iniciado");
}



void WiFiHandler::setupRoutes()
{
    m_server.on("/", HTTP_GET, [this](){this->handleRoot();});
    m_server.on("/set-temp", HTTP_GET, [this](){this->handleSetTemp();});
    m_server.on("/abrir", [this](){this->handleAbrirJanela();});
    m_server.on("/fechar", [this](){this->handleFecharJanela();});
    m_server.on("/set-mode", HTTP_GET, [this](){this->handleSetMode();});
    m_server.on("/update-temp", HTTP_POST, [this](){this->handleUpdateTemp();});
    m_server.on("/update-ssid", HTTP_POST, [this](){this->handleUpdateSSID();});
    m_server.on("/update-password", HTTP_POST, [this](){this->handleUpdatePassword();});
    m_server.on("/set-fake-mode", HTTP_GET, [this](){this->handleSetFakeMode();});
    m_server.on("/update-ajuste", HTTP_POST, [this](){this->handleUpdateAjuste();});
    m_server.on("/get-data", HTTP_GET, [this](){this->handleGetData();});
}

void WiFiHandler::handleClient()
{
    m_server.handleClient();
}

void WiFiHandler::handleRoot() {
    // TODO: mover lógica atual do handleRoot pra cá
    m_server.send(200, "text/plain", "handleRoot");
}


void WiFiHandler::handleSetTemp()
{
    if(m_server.hasArg("temp"))
    {
        m_temperaturaAtual = m_server.arg("temp").toInt();
        m_server.send(200, "text/plain", "OK");
        Serial.print("Temperatura simulada alterada para: ");
        Serial.println(m_temperaturaAtual);
        /*
        if(modoAutomatico)
        {
            if(temperaturaAtual >= TemperaturaDeAbertura && !janelaAberta)
            {
                fecharJanelawebCmd = true;
            }
            else if(temperaturaAtual <= TemperaturaDeFechamento && janelaAberta)        ?????????????????
            {
                fecharJanelawebCmd = true;
            }
        }
        */
    }
    else
    {
        m_server.send(400, "text/plain", "Erro");
    }
}
void WiFiHandler::handleAbrirJanela()
{
    m_WebDataHdlr.AbrirJanela();
    m_server.send(200, "text/plain", "OK");
}

void WiFiHandler::handleFecharJanela()
{
    m_WebDataHdlr.FecharJanela();
    m_server.send(200, "text/plain", "OK");
}

void WiFiHandler::handleSetMode()
{
    if(m_server.hasArg("mode"))
    {
        const int ModoNovo = m_server.arg("mode").toInt();
        if(!m_WebDataHdlr.TrocarModo(ModoNovo))
        {
            m_server.send(200, "text/plain", "OK");
        }
        else
        {
            m_server.send(400, "text/plain", "Erro");
        }
    }
}

void WiFiHandler::handleUpdateTemp()
{
    if(m_server.hasArg("abertura") && m_server.hasArg("fechamento"))
    {
        const int NovaTemperaturaDeAbertura   = m_server.arg("abertura").toInt();
        const int NovaTemperaturaDeFechamento = m_server.arg("fechamento").toInt();

        if(NovaTemperaturaDeAbertura < NovaTemperaturaDeFechamento)
        {
            m_server.send(400, "text/plain", "Temperatura de abertura deve ser maior que de fechamento");
        }
        else if(!m_WebDataHdlr.TrocarTemperaturaDeAbertura(NovaTemperaturaDeAbertura))
        {
            m_server.send(400, "text/plain", "Temperatura de abertura fora do range +99C...-99C");
        }
        else if(!m_WebDataHdlr.TrocarTemperaturaDeFechamento(NovaTemperaturaDeFechamento))
        {
            m_server.send(400, "text/plain", "Temperatura de fechamento fora do range +99C...-99C");
        }
        else
        {
            m_server.send(200, "text/plain", "OK");
        }
    }
    else
    {
        m_server.send(400, "text/plain", "Parametros faltando");
    }
}

void WiFiHandler::handleUpdateSSID()
{
    if(m_server.hasArg("newssid"))
    {
        const String NovoSSID = m_server.arg("newssid");
        if(NovoSSID.length() > 0)
        {
            // Verificar melhor maneira -> WiFi.softAP(currentSSID.c_str(), currentPassword.c_str());
            m_WebDataHdlr.TrocarSSID(NovoSSID);
            m_server.send(200, "text/plain", "OK");
        }
        else
        {
            m_server.send(400, "text/plain", "SSID não pode ser vazio");
        }
    }
    else
    {
        m_server.send(400, "text/plain", "Parâmetro faltando");
    }
}


void WiFiHandler::handleUpdatePassword() {
    if(m_server.hasArg("newpassword"))
    {
        const String NovaSenha = m_server.arg("newpassword");

        if(m_WebDataHdlr.TrocarSenha(NovaSenha))
        {
            //WiFi.softAP(m_VariablesHdlr.GetSSID().c_str(), NovaSenha.c_str());
            m_server.send(200, "text/plain", "OK");
            Serial.println("Senha alterada com sucesso");
        }
        else
        {
            m_server.send(400, "text/plain", "Senha deve ter pelo menos 8 caracteres");
        }
    }
    else
    {
        m_server.send(400, "text/plain", "Parâmetro faltando");
    }
}

void WiFiHandler::handleSetFakeMode() {
    // TODO
    m_server.send(200, "text/plain", "handleSetFakeMode");
}

void WiFiHandler::handleUpdateAjuste() {
    // TODO
    m_server.send(200, "text/plain", "handleUpdateAjuste");
}

void WiFiHandler::handleGetData()
{
    String json = "{";
    json += "\"m_temperaturaAtual\":" + String(m_temperaturaAtual, 1) + ",";
    // json += "\"temperaturaFake\":" + String(temperaturaFake, 1) + ",";          ???????????????
    // json += "\"c\":" + String(c) + ",";
    // json += "\"janelaAberta\":" + String(janelaAberta ? "true" : "false") + ",";
    json += "\"modoAutomatico\":" + String(m_WebDataHdlr.EstaEmModoAutomatico() ? "true" : "false") + ",";
    //json += "\"modoFakeAtivo\":" + String(modoFakeAtivo ? "true" : "false");
    json += "}";
  
  m_server.send(200, "application/json", json);
}
