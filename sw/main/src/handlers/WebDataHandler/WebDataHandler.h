#ifndef WEB_DATA_HANDLER_H
#define WEB_DATA_HANDLER_H

#include <Arduino.h>
#include "../VariablesHandler/VariablesHandler.h"

class WebDataHandler
{
    public:
        WebDataHandler(VariablesHandler& handler);
        void AbrirJanela() const;
        void FecharJanela() const;
        bool TrocarModo(const unsigned int NovoModo) const;
        bool TrocarTemperaturaDeAbertura(const int NovaTemp);
        bool TrocarTemperaturaDeFechamento(const int NovaTemp) const;
        void TrocarSSID(const String NovoSSid) const;
        bool TrocarSenha(const String NovaSenha) const;
        bool EstaEmModoAutomatico() const;
    private:
        VariablesHandler& m_VariablesHdler;
};

#endif
