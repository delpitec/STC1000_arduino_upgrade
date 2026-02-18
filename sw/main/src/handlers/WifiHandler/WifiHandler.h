#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H
#include <Arduino.h>
#include <WebServer.h>
#include "../WebDataHandler/WebDataHandler.h"

class WiFiHandler
{
    public: /* Public methods */
        WiFiHandler(WebDataHandler& WebDataHdlr);
        void beginClient(const String WifiSSID, const String WifiSenha);
        void handleClient();
    private: /* Private methods */
        void setupRoutes();
        void handleRoot();
        void handleSetTemp();
        void handleAbrirJanela();
        void handleFecharJanela();
        void handleSetMode();
        void handleUpdateTemp();
        void handleUpdateSSID();
        void handleUpdatePassword();
        void handleSetFakeMode();
        void handleUpdateAjuste();
        void handleGetData();
    private: /* Private variables */
        WebDataHandler&   m_WebDataHdlr;
        WebServer         m_server;
};

#endif