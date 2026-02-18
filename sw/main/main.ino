#include <thermistor.h>
#include "src/handlers/EepromHandler/EepromHandler.h"
#include "src/handlers/VariablesHandler/VariablesHandler.h"
#include "src/handlers/WifiHandler/WifiHandler.h"

#include "SaidaDigital.h"
#include "EntradaDigital.h"
#include "CustomTimer.h"
#include "DisplayMultiplex.h"


void handleSetTemp();
void handleUpdateTemp();
void handleRoot();
void handleAbrirJanela();
void handleFecharJanela();
void fecharJanela();
void abrirJanela();
void handleSetMode();
void handleUpdateSSID();
void saveSSIDToEEPROM(const String& newSSID);
void handleFecharJanelaViaWeb();
void handleSalvarTemperaturasNaEEPROMViaWeb();
void handleUpdatePassword();
void automaticoFake();
void handleSetFakeMode();
void handleUpdateAjuste();
void handleGetData();

const int A = 10;
const int B = 11;
const int F = 12;
const int M = 13;
const int E = 14;
const int U = 15;
const int tracos = 16;

THERMISTOR thermistor(34,             // Analog pin
                      10000,          // Nominal resistance at 25 ºC
                      3950,           // thermistor's beta coefficient
                      5200);

DisplayMultiplex Display;
SaidaDigital     Rele(22);              // [Pino 22]
SaidaDigital     Led(2);                // [Pino 2]
EntradaDigital   Botao_CIMA(18, 1, 1);  // [Pino 18 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital   Botao_BAIXO(35, 1, 0); // [Pino 35 , Lógica Invertida (Ativo em 0V) , Sem Pull Up interno]
EntradaDigital   Botao_S(21, 1, 1);     // [Pino 21 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]
EntradaDigital   SensorJanelaAberta(13,1,1); // [Pino 13 , Lógica Invertida (Ativo em 0V) , Com Pull Up interno]

int CalibracaoDeTemperatura = 0;
bool janelaAberta = false;
bool janelaFechada = false;
bool fecharJanelawebCmd = false;
int c = 0;
float temperaturaFake;


const unsigned int TempoLetras_ms = 3000;
CustomTimer tmr(TempoLetras_ms);
CustomTimer tmrRele(1000);
CustomTimer tmrEEPROM;

EepromHandler    EepromHdlr(0, 4, 100, 150, 50); // Interface para dados persistentes
VariablesHandler VariablesHdlr;                  // Variáveis do sistema. Ponto único de acesso
WebDataHandler   WebDataHdlr(VariablesHdlr);     // Regras entre Main <-> Comunicação Wifi
WiFiHandler      WifiHdlr(WebDataHdlr);          // Regras exlucivas do Wifi
void setup()
{
  Serial.begin(115200);
  Display.Begin();

  InicializacaoVariaveisDoSistema();
  WifiHdlr.beginClient(VariablesHdlr.GetSSID(), VariablesHdlr.GetSenha());
}



void loop() {

  //InitPersistVariables();

  while(1)
  {

    WifiHdlr.handleClient();
    //WiFi.softAP(m_VariablesHdlr.GetSSID().c_str(), NovaSenha.c_str());  Atualizar dados de conexão wifi caso identificada mudança de Senha e/ou SSID

    AtualizaLeituraBotoes();
    AtualizaLeituraSensores();

    handleFecharJanelaViaWeb();
    handleSalvarTemperaturasNaEEPROMViaWeb();

    if(Botao_S.TrasicaoAtivo())
    {
      VariablesHdlr.SetModo(VariablesHdlr.GetModo() + 1); // modo++
      tmr.Init(TempoLetras_ms);
      if(VariablesHdlr.GetModo() >= MODO_INDEFINIDO)
      {
        //AtualizaDadosNaMemoriaPersistente();
        //salvarTemperaturasNaEEPROM();
        VariablesHdlr.SetModo(MODO_MANUAL);
      }
    }
    if (VariablesHdlr.GetModo() == MODO_MANUAL)
    {
      if(!tmr.Finished())
      {
        Display.displayLetra(M, A);
      }
      else
      {
        janelaManual();
      }
    }
    else if (VariablesHdlr.GetModo() == MODO_AUTOMATICO)
    {
      if(!tmr.Finished())
      {
        Display.displayLetra(A, U);
      }
      else
      {
        temperaturaAutomatica();
      }
    }
    else if (VariablesHdlr.GetModo() == MODO_CALIBRACAO)
    {
      if(!tmr.Finished())
      {
        Display.displayLetra(A, F);
      }
      else
      {
        // CalibrarTemperatura();
        automaticoFake();
      }
    }
    else if (VariablesHdlr.GetModo() == MODO_CONFIG_FECHAMENTO)
    {
      if(!tmr.Finished())
      {
        Display.displayLetra(F, E);
      }
      else
      {
        ajustarTemperaturaDeFechamento();
      }
    }
    else if (VariablesHdlr.GetModo() == MODO_CONFIG_ABERTURA)
    {
      if(!tmr.Finished())
      {
        Display.displayLetra(A, B);
      }
      else
      {
          ajustarTemperaturaDeAbertura();
      }
    }
  }
}
/*
  @brief  Inicializa as variáveis de acordo com os dados persistentes (salvos em memória não volátil - EEPROM)
          Caso algum dado persistente não esteja salvo na memória não volátil, irá inicializar as variáveis 
          do programa com valores padrão de fábrica.
*/
void InicializacaoVariaveisDoSistema()
{
    const int PersistTemperaturaDeAbertura   = EepromHdlr.GetTemperaturaDeAbertura();
    const int PersistTemperaturaDeFechamento = EepromHdlr.GetTemperaturaDeFechamento();
    const int PersistCalibracaoDeTemperatura = EepromHdlr.GetCalibracaoDeTemperatura();
    const String PersistSSID  = EepromHdlr.GetSSID();
    const String PersistSenha = EepromHdlr.GetSenha();

    if(PersistTemperaturaDeAbertura <= PersistTemperaturaDeFechamento ||
        PersistTemperaturaDeAbertura > 99 ||
        PersistTemperaturaDeFechamento < -99)
    {
        VariablesHdlr.SetTemperaturaDeAbertura(25);
        VariablesHdlr.SetTemperaturaDeFechamento(15);
    }
    else
    {
      VariablesHdlr.SetTemperaturaDeAbertura(PersistTemperaturaDeAbertura);
      VariablesHdlr.SetTemperaturaDeAbertura(PersistTemperaturaDeFechamento);
    }

    if(PersistSSID.length() > 0)
    {
        VariablesHdlr.SetSSID(PersistSSID);
    }
    else
    {
        const String SSIDFabrica = "Robert's Janela 01";
        VariablesHdlr.SetSSID(SSIDFabrica);
        EepromHdlr.SetSSID(SSIDFabrica);
    }

    if (PersistSenha.length() > 0)
    {
      VariablesHdlr.SetSenha(PersistSenha);
    }
    else
    {
        const String SenhaFabrica = "Roberts@0108";
        VariablesHdlr.SetSenha(SenhaFabrica);
        EepromHdlr.SetSenha(SenhaFabrica);
    }

    VariablesHdlr.SetModo(MODO_AUTOMATICO);
}

void AtualizaLeituraBotoes()
{
    Botao_S.AtualizaLeitura();
    Botao_CIMA.AtualizaLeitura();
    Botao_BAIXO.AtualizaLeitura();
}

void AtualizaLeituraSensores()
{
    SensorJanelaAberta.AtualizaLeitura();
    VariablesHdlr.SetTemperaturaAtual(thermistor.read() / 10.0); // mudar pro ReadCalibrated()
    temperaturaFake = thermistor.incrementoFisicoNTC(c, VariablesHdlr.GetTemperaturaAtual());    // ???????????? armazenar 
    // thermistor.setcalibration(val) -> calibration
}

void AtualizaDadosNaMemoriaPersistente()
{
  //if(VariablesHandler.GetTemperaturaDeAbertura != PersistVariablesHandler.GetTemperaturaDeAbertura())
  //{
    // Write to EEPROM
  //}
}

//void handleGetData() {
//  String json = "{";
//  json += "\"temperaturaAtual\":" + String(temperaturaAtual, 1) + ",";
//  json += "\"temperaturaFake\":" + String(temperaturaFake, 1) + ",";
//  json += "\"c\":" + String(c) + ",";
//  json += "\"janelaAberta\":" + String(janelaAberta ? "true" : "false") + ",";
//  json += "\"modoAutomatico\":" + String(modoAutomatico ? "true" : "false") + ",";
//  json += "\"modoFakeAtivo\":" + String(modoFakeAtivo ? "true" : "false");
//  json += "}";
//  
//  server.send(200, "application/json", json);
//}

void janelaManual()
{
  Display.displayLetra(tracos,tracos);

  if(Botao_CIMA.TrasicaoAtivo() || VariablesHdlr.GetComandoJanela() == CMD_ABRIR_JANELA)
  {
    VariablesHdlr.SetComandoJanela(CMD_FAZER_NADA);
    if(!SensorJanelaAberta.EstaAtivo())
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
    }
  }
  else if(Botao_BAIXO.TrasicaoAtivo() || VariablesHdlr.GetComandoJanela() == CMD_FECHAR_JANELA)
  {
    VariablesHdlr.SetComandoJanela(CMD_FAZER_NADA);
    if(SensorJanelaAberta.EstaAtivo())
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
    }
  }
}

// Aqui a temperatura que estamos usando em automático deve ser calibrada.
// Cara pressionada de botão cima/baixo deve incrementar/decrementar a temperatura atual em 0.1 graus celsius
void automaticoFake() {
  static int c_anterior = CalibracaoDeTemperatura;  // Guarda o valor anterior para comparar
  temperaturaFake = thermistor.incrementoFisicoNTC(CalibracaoDeTemperatura, VariablesHdlr.GetTemperaturaAtual());

  if (Botao_CIMA.TrasicaoAtivo())
  {
    const int NovaTemperatura = VariablesHdlr.GetCalibracaoDeTemperatura() + 1;

    if(NovaTemperatura > 50)
    {
      VariablesHdlr.SetCalibracaoDeTemperatura(50);
    }
    else
    {
      VariablesHdlr.SetCalibracaoDeTemperatura(NovaTemperatura);
    }
    Display.showNumber(temperaturaFake);

    // thermistor.SetCalibration(VariablesHdlr.GetCalibracaoDeTemperatura())
    // EepromHdlr.SetCalibracaoDeTemperatura(CalibracaoDeTemperatura); // ???????????????????

  }


  if(Botao_BAIXO.TrasicaoAtivo()) 
  {
    const int NovaTemperatura = VariablesHdlr.GetCalibracaoDeTemperatura() - 1;
    if(NovaTemperatura < -50)
    {
      VariablesHdlr.SetCalibracaoDeTemperatura(-50);
    }
    else
    {
      VariablesHdlr.SetCalibracaoDeTemperatura(NovaTemperatura);
    }

    if(temperaturaFake > 99) temperaturaFake = 25;
  }

  if (CalibracaoDeTemperatura != c_anterior)
  {
    EepromHdlr.SetCalibracaoDeTemperatura(CalibracaoDeTemperatura);
    c_anterior = CalibracaoDeTemperatura;
    Serial.print("CalibracaoDeTemperatura salvo (backup): ");
    Serial.println(CalibracaoDeTemperatura);
  }

  if (temperaturaFake >= VariablesHdlr.GetTemperaturaDeAbertura())
  {
    abrirJanela();
  }
  else if (temperaturaFake <= VariablesHdlr.GetTemperaturaDeFechamento())
  {
    fecharJanela();
  }

  if(Botao_S.TrasicaoAtivo())
  {
    EepromHdlr.SetCalibracaoDeTemperatura(CalibracaoDeTemperatura);
    Serial.print("CalibracaoDeTemperatura salvo (Botão S): ");
    Serial.println(CalibracaoDeTemperatura);
  }
}

void temperaturaAutomatica()
{
  Display.showNumber(VariablesHdlr.GetTemperaturaAtual());

  if (VariablesHdlr.GetTemperaturaAtual() >= VariablesHdlr.GetTemperaturaDeAbertura())
  {
    abrirJanela();
  }
  else if (VariablesHdlr.GetTemperaturaAtual() <= VariablesHdlr.GetTemperaturaDeFechamento())
  {
    fecharJanela();
  }
  else
  {
    ; // Temperatura dentro dos limites. Fazer nada.
  }
}

void ajustarTemperaturaDeAbertura()
{
  if(Botao_CIMA.TrasicaoAtivo())
  { // Incrementa (temperatura de abertura + 1)
    const int NovaTemperaturaDeAbertura = VariablesHdlr.GetTemperaturaDeAbertura() + 1;
    VariablesHdlr.SetTemperaturaDeAbertura(NovaTemperaturaDeAbertura);
  }
  if(Botao_BAIXO.TrasicaoAtivo())
  { // Decrementa (temperatura de abertura - 1)
    const int NovaTemperaturaDeAbertura = VariablesHdlr.GetTemperaturaDeAbertura() - 1;
    VariablesHdlr.SetTemperaturaDeAbertura(NovaTemperaturaDeAbertura);
  }

  if(VariablesHdlr.GetTemperaturaDeAbertura() <= VariablesHdlr.GetTemperaturaDeFechamento())
  { // Temperatura de abertura deve ser maior que temperatura de fechamento
    VariablesHdlr.SetTemperaturaDeAbertura(VariablesHdlr.GetTemperaturaDeFechamento() + 1);
  }
  if(VariablesHdlr.GetTemperaturaDeAbertura() >= 100)
  { // Temperatura de abertura deve ser menor que 100 graus celsius
    VariablesHdlr.SetTemperaturaDeAbertura(99);
  }

  Display.showNumber(VariablesHdlr.GetTemperaturaDeAbertura());
}

void ajustarTemperaturaDeFechamento()
{
  if(Botao_CIMA.TrasicaoAtivo())
  {
    const int NovaTemperaturaDeFechamento = VariablesHdlr.GetTemperaturaDeFechamento() + 1;
    VariablesHdlr.SetTemperaturaDeFechamento(NovaTemperaturaDeFechamento);
  }
  if(Botao_BAIXO.TrasicaoAtivo())
  {
    const int NovaTemperaturaDeFechamento = VariablesHdlr.GetTemperaturaDeFechamento() - 1;
    VariablesHdlr.SetTemperaturaDeFechamento(NovaTemperaturaDeFechamento);
  }
  if(VariablesHdlr.GetTemperaturaDeFechamento() >= VariablesHdlr.GetTemperaturaDeAbertura())
  {
    VariablesHdlr.SetTemperaturaDeFechamento(VariablesHdlr.GetTemperaturaDeAbertura() - 1);
  }
  if(VariablesHdlr.GetTemperaturaDeFechamento() <= 0)
  {
    VariablesHdlr.SetTemperaturaDeFechamento(1);
  }
  Display.showNumber(VariablesHdlr.GetTemperaturaDeFechamento());
}

void handleFecharJanelaViaWeb()
{
  if(fecharJanelawebCmd)
  {
    fecharJanelawebCmd = false;
    if(SensorJanelaAberta.EstaAtivo() && janelaFechada == false)
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
      janelaFechada = true;
      janelaAberta = false;
    }
    else if(!SensorJanelaAberta.EstaAtivo() && janelaAberta == false)
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
      janelaAberta = true;
      janelaFechada = false;
    }
    else
    {
      ;
    }
  }
}

void handleSalvarTemperaturasNaEEPROMViaWeb()
{
  static int TemperaturaDeFechamentoAntiga = VariablesHdlr.GetTemperaturaDeFechamento();
  static int TemperaturaDeAberturaAntiga   = VariablesHdlr.GetTemperaturaDeAbertura();
  static bool tmrEEPROMContando = false;

  if(TemperaturaDeFechamentoAntiga != VariablesHdlr.GetTemperaturaDeFechamento() || TemperaturaDeAberturaAntiga != VariablesHdlr.GetTemperaturaDeAbertura())
  {
    TemperaturaDeFechamentoAntiga = VariablesHdlr.GetTemperaturaDeFechamento();
    TemperaturaDeAberturaAntiga   = VariablesHdlr.GetTemperaturaDeAbertura();
    tmrEEPROM.Init(20000);
    tmrEEPROMContando = true;
  }

  if(tmrEEPROMContando && tmrEEPROM.Finished())
  {
    tmrEEPROMContando = false;
    salvarTemperaturasNaEEPROM();
  }
}

void salvarTemperaturasNaEEPROM()
{
    EepromHdlr.SetTemperaturaDeAbertura(VariablesHdlr.GetTemperaturaDeAbertura(), 3);
    EepromHdlr.SetTemperaturaDeFechamento(VariablesHdlr.GetTemperaturaDeFechamento(), 3);
}

void abrirJanela() {
  if(!SensorJanelaAberta.EstaAtivo() && janelaAberta == false)
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
      tmrRele.Init(10000);
      janelaAberta = true;
      janelaFechada = false;
    }
    if(janelaAberta == true && tmrRele.Finished())
    {
      if (!SensorJanelaAberta.EstaAtivo())
      {
        Rele.Ligar();
        Led.Ligar();
        delay(1000);
        Rele.Desligar();
        Led.Desligar();
        tmrRele.Init(10000);
      }
      else
      {
        janelaAberta = false;
      }
    }
}

void fecharJanela()
{
  if(SensorJanelaAberta.EstaAtivo() && janelaFechada == false)
  {
    Rele.Ligar();
    Led.Ligar();
    delay(1000);
    Rele.Desligar();
    Led.Desligar();
    tmrRele.Init(10000);
    janelaFechada = true;
    janelaAberta = false;
  }
  if(janelaFechada == true && tmrRele.Finished())
  {
    if(SensorJanelaAberta.EstaAtivo())
    {
      Rele.Ligar();
      Led.Ligar();
      delay(1000);
      Rele.Desligar();
      Led.Desligar();
      tmrRele.Init(10000);
    }
    else
    {
      janelaFechada = false;
    }
  }
}

//void handleAbrirJanela(){
//  if(!modoAutomatico) {
//    fecharJanelawebCmd = true;
//  }
//  server.send(200, "text/plain", "OK");
//}

//void handleFecharJanela() {
//  if(!modoAutomatico) {
//    fecharJanelawebCmd = true;
//  }
//  server.send(200, "text/plain", "OK");
//}

//void handleSetMode() {
//  if(server.hasArg("mode")) {
//    int requestedMode = server.arg("mode").toInt();
//    if(requestedMode == 1 || requestedMode == 2) {
//      funcao = requestedMode;
//      modoAutomatico = (funcao == 2);
//      tmr.Init(TempoLetras_ms);
//      server.send(200, "text/plain", "OK");
//      return;
//    }
//  }
//  server.send(400, "text/plain", "Erro");
//}

//void handleSetFakeMode() {
//  funcao = 3;
//  modoFakeAtivo = true;
//  modoAutomatico = false;
//  tmr.Init(TempoLetras_ms);
//  server.send(200, "text/plain", "OK");
//}

//void handleUpdateAjuste() {
//  if(server.hasArg("ajuste")) {
//    int newAjuste = server.arg("ajuste").toInt();
//    
//    // Aplicar os mesmos limites da função automaticoFake
//    if(newAjuste < -50) newAjuste = -50;
//    if(newAjuste > 50) newAjuste = 50;
//    
//    c = newAjuste;
//    
//    // Salvar na EEPROM igual é feito no físico
//    EEPROM.put(eepromTempFake, c);
//    EEPROM.commit();
//    
//    // Recalcular a temperatura fake
//    temperaturaFake = thermistor.incrementoFisicoNTC(c, temperaturaAtual);
//    
//    server.send(200, "text/plain", "OK");
//    Serial.print("Ajuste alterado para: ");
//    Serial.println(c);
//    
//    // Se estiver no modo fake, verificar se precisa abrir/fechar janela
//    if(modoFakeAtivo) {
//      if (temperaturaFake >= TemperaturaDeAbertura) {
//        abrirJanela();
//      } else if (temperaturaFake <= TemperaturaDeFechamento) {
//        fecharJanela();
//      }
//    }
//  } else {
//    server.send(400, "text/plain", "Parâmetro faltando");
//  }
//}

//void handleUpdateSSID() {
//  if(server.hasArg("newssid")) {
//    String newSSID = server.arg("newssid");
//    if(newSSID.length() > 0) {
//      saveSSIDToEEPROM(newSSID);
//      currentSSID = newSSID;
//      WiFi.softAP(currentSSID.c_str(), currentPassword.c_str());
//      server.send(200, "text/plain", "OK");
//      Serial.print("SSID alterado para: ");
//      Serial.println(newSSID);
//    } else {
//      server.send(400, "text/plain", "SSID não pode ser vazio");
//    }
//  } else {
//    server.send(400, "text/plain", "Parâmetro faltando");
//  }
//}

//void handleUpdatePassword() {
//  if(server.hasArg("newpassword")) {
//    String newPassword = server.arg("newpassword");
//    if(newPassword.length() >= 8) {
//      EepromHdlr.SetPassword(newPassword);
//      currentPassword = newPassword;
//      WiFi.softAP(currentSSID.c_str(), currentPassword.c_str());
//      server.send(200, "text/plain", "OK");
//      Serial.println("Senha alterada com sucesso");
//    } else {
//      server.send(400, "text/plain", "Senha deve ter pelo menos 8 caracteres");
//    }
//  } else {
//    server.send(400, "text/plain", "Parâmetro faltando");
//  }
//}


//void handleRoot() {
//  String html = R"=====(
//  <!DOCTYPE html>
//  <html>
//  <head>
//    <title>)=====" + currentSSID + R"=====(</title>
//    <meta name="viewport" content="width=device-width, initial-scale=1">
//    <style>
//      body {
//        font-family: Arial, sans-serif;
//        background-color: #f0f2f5;
//        margin: 0;
//        padding: 20px;
//        color: #333;
//      }
//      .container {
//        max-width: 400px;
//        margin: 0 auto;
//        background: white;
//        border-radius: 15px;
//        padding: 20px;
//        box-shadow: 0 4px 8px rgba(0,0,0,0.1);
//      }
//      h1 {
//        text-align: center;
//        color: #2c3e50;
//        margin-bottom: 25px;
//      }
//      .status-info {
//        text-align: center;
//        margin: 15px 0;
//        font-size: 18px;
//        padding: 10px;
//        border-radius: 8px;
//        background-color: #ecf0f1;
//      }
//      .status-aberta {
//        color: #2ecc71;
//        font-weight: bold;
//      }
//      .status-fechada {
//        color: #e74c3c;
//        font-weight: bold;
//      }
//      .control-panel {
//        margin-bottom: 20px;
//      }
//      .btn {
//        display: block;
//        width: 100%;
//        padding: 12px;
//        margin: 10px 0;
//        border: none;
//        border-radius: 8px;
//        font-size: 16px;
//        font-weight: bold;
//        cursor: pointer;
//        transition: all 0.3s;
//      }
//      .btn-open {
//        background-color: #2ecc71;
//        color: white;
//      }
//      .btn-close {
//        background-color: #e74c3c;
//        color: white;
//      }
//      .btn-open:hover { background-color: #27ae60; }
//      .btn-close:hover { background-color: #c0392b; }
//      .temp-control {
//        background-color: #ecf0f1;
//        padding: 15px;
//        border-radius: 10px;
//        margin: 15px 0;
//      }
//      .temp-header {
//        font-weight: bold;
//        margin-bottom: 10px;
//        color: #2c3e50;
//      }
//      .temp-adjust {
//        display: flex;
//        align-items: center;
//        justify-content: space-between;
//        margin: 10px 0;
//      }
//      .temp-value {
//        font-size: 24px;
//        font-weight: bold;
//        color: #3498db;
//      }
//      .temp-btn {
//        background-color: #bdc3c7;
//        border: none;
//        width: 40px;
//        height: 40px;
//        border-radius: 50%;
//        font-size: 20px;
//        cursor: pointer;
//      }
//      .mode-buttons {
//        display: flex;
//        gap: 10px;
//        margin-bottom: 15px;
//      }
//      .mode-btn {
//        flex: 1;
//        padding: 10px;
//        border: none;
//        border-radius: 8px;
//        font-weight: bold;
//        cursor: pointer;
//      }
//      .mode-btn.active {
//        background-color: #3498db;
//        color: white;
//      }
//      .mode-btn.inactive {
//        background-color: #ecf0f1;
//        color: #7f8c8d;
//      }
//      .current-temp {
//        text-align: center;
//        font-size: 18px;
//        margin: 15px 0;
//        display: flex;
//        justify-content: space-between;
//        align-items: center;
//      }
//      .fake-toggle {
//        background-color: #9b59b6;
//        color: white;
//        border: none;
//        padding: 8px 15px;
//        border-radius: 20px;
//        font-size: 14px;
//        cursor: pointer;
//        font-weight: bold;
//      }
//      .fake-toggle.active {
//        background-color: #8e44ad;
//      }
//      .fake-temp {
//        color: #7f8c8d;
//        font-size: 16px;
//      }
//      .ssid-control {
//        margin-top: 20px;
//        padding: 15px;
//        background-color: #ecf0f1;
//        border-radius: 10px;
//      }
//      .password-control {
//        margin-top: 15px;
//        padding: 15px;
//        background-color: #ecf0f1;
//        border-radius: 10px;
//      }
//      .ssid-header, .password-header {
//        font-weight: bold;
//        margin-bottom: 10px;
//        color: #2c3e50;
//      }
//      .ssid-input, .password-input {
//        width: 100%;
//        padding: 10px;
//        border-radius: 5px;
//        border: 1px solid #ddd;
//        margin-bottom: 10px;
//        box-sizing: border-box;
//        max-width: 100%;
//      }
//      .ssid-btn, .password-btn {
//        width: 100%;
//        padding: 10px;
//        background-color: #3498db;
//        color: white;
//        border: none;
//        border-radius: 5px;
//        font-weight: bold;
//        cursor: pointer;
//      }
//      .loading {
//        opacity: 0.6;
//        pointer-events: none;
//      }
//    </style>
//    <script>
//      let updateInterval;
//      
//      function toggleJanela() {
//        if(!document.getElementById('mode-auto').classList.contains('active')) {
//          fetch(janelaAberta ? '/fechar' : '/abrir')
//            .then(updateData);
//        }
//      }
//      
//      function setMode(mode) {
//        fetch('/set-mode?mode=' + mode)
//          .then(updateData);
//      }
//      
//      function setFakeMode() {
//        fetch('/set-fake-mode')
//          .then(updateData);
//      }
//      
//      function updateTemp(type, change) {
//        const element = document.getElementById(type);
//        const current = parseInt(element.innerText);
//        const newValue = current + change;
//        
//        if(type === 'abertura' && newValue <= parseInt(document.getElementById('fechamento').innerText)) {
//          alert('Temperatura de abertura deve ser maior que de fechamento');
//          return;
//        }
//        if(type === 'fechamento' && newValue >= parseInt(document.getElementById('abertura').innerText)) {
//          alert('Temperatura de fechamento deve ser menor que de abertura');
//          return;
//        }
//        
//        const formData = new FormData();
//        formData.append(type, newValue);
//        formData.append(type === 'abertura' ? 'fechamento' : 'abertura', 
//                       type === 'abertura' ? document.getElementById('fechamento').innerText : 
//                       document.getElementById('abertura').innerText);
//        
//        fetch('/update-temp', {
//          method: 'POST',
//          body: formData
//        }).then(updateData);
//      }
//      
//      function updateAjuste(change) {
//        const element = document.getElementById('ajuste-value');
//        const current = parseInt(element.innerText);
//        const newValue = current + change;
//        
//        if(newValue < -50) newValue = -50;
//        if(newValue > 50) newValue = 50;
//        
//        // Feedback visual imediato
//        element.innerText = newValue;
//        
//        fetch('/update-ajuste', {
//          method: 'POST',
//          body: 'ajuste=' + newValue,
//          headers: {
//            'Content-Type': 'application/x-www-form-urlencoded',
//          }
//        }).then(updateData);
//      }
//      
//      function updateSSID() {
//        const newSSID = document.getElementById('new-ssid').value;
//        if(newSSID.trim() === '') {
//          alert('Por favor, insira um novo SSID');
//          return;
//        }
//        
//        fetch('/update-ssid', {
//          method: 'POST',
//          body: 'newssid=' + encodeURIComponent(newSSID),
//          headers: {
//            'Content-Type': 'application/x-www-form-urlencoded',
//          }
//        }).then(response => {
//          if(response.ok) {
//            alert('SSID alterado com sucesso! Reconecte-se à nova rede.');
//            location.reload();
//          } else {
//            alert('Erro ao alterar SSID');
//          }
//        });
//      }
//      
//      function updatePassword() {
//        const newPassword = document.getElementById('new-password').value;
//        if(newPassword.length < 8) {
//          alert('A senha deve ter pelo menos 8 caracteres');
//          return;
//        }
//        
//        fetch('/update-password', {
//          method: 'POST',
//          body: 'newpassword=' + encodeURIComponent(newPassword),
//          headers: {
//            'Content-Type': 'application/x-www-form-urlencoded',
//          }
//        }).then(response => {
//          if(response.ok) {
//            alert('Senha alterada com sucesso!');
//            location.reload();
//          } else {
//            alert('Erro ao alterar senha');
//          }
//        });
//      }
//      
//      function updateData() {
//        fetch('/get-data')
//          .then(response => response.json())
//          .then(data => {
//            // Atualizar apenas os valores que mudam
//            document.getElementById('current-temp-value').innerText = data.temperaturaAtual + '°C';
//            document.getElementById('fake-temp-value').innerText = data.temperaturaFake + '°C';
//            document.getElementById('ajuste-value').innerText = data.c;
//            
//            // Atualizar status da janela
//            const statusElement = document.querySelector('.status-info span');
//            statusElement.className = data.janelaAberta ? 'status-aberta' : 'status-fechada';
//            statusElement.innerText = data.janelaAberta ? 'ABERTA' : 'FECHADA';
//            
//            // Atualizar botão da janela
//            const janelaBtn = document.querySelector('.control-panel .btn');
//            janelaBtn.className = data.janelaAberta ? 'btn btn-close' : 'btn btn-open';
//            janelaBtn.innerText = data.janelaAberta ? 'FECHAR JANELA' : 'ABRIR JANELA';
//            janelaBtn.disabled = data.modoAutomatico || data.modoFakeAtivo;
//            if (janelaBtn.disabled) {
//              janelaBtn.style.opacity = '0.5';
//              janelaBtn.style.cursor = 'not-allowed';
//            } else {
//              janelaBtn.style.opacity = '';
//              janelaBtn.style.cursor = '';
//            }
//            
//            // Atualizar modos
//            document.getElementById('mode-manual').className = 
//              `mode-btn ${!data.modoAutomatico && !data.modoFakeAtivo ? 'active' : 'inactive'}`;
//            document.getElementById('mode-auto').className = 
//              `mode-btn ${data.modoAutomatico && !data.modoFakeAtivo ? 'active' : 'inactive'}`;
//            
//            // Atualizar botão de ajuste
//            const fakeToggle = document.querySelector('.fake-toggle');
//            fakeToggle.className = `fake-toggle ${data.modoFakeAtivo ? 'active' : ''}`;
//            fakeToggle.innerText = data.modoFakeAtivo ? 'AJUSTE ✓' : 'AJUSTE';
//          })
//          .catch(error => console.error('Erro ao atualizar dados:', error));
//      }
//      
//      // Atualizar dados a cada 2 segundos
//      function startAutoUpdate() {
//        updateInterval = setInterval(updateData, 2000);
//      }
//      
//      // Iniciar quando a página carregar
//      document.addEventListener('DOMContentLoaded', function() {
//        startAutoUpdate();
//      });
//      
//      // Variáveis globais para o template
//      const janelaAberta = )=====" + String(janelaAberta ? "true" : "false") + R"=====(;
//      const modoFakeAtivo = )=====" + String(modoFakeAtivo ? "true" : "false") + R"=====(;
//    </script>
//  </head>
//  <body>
//    <div class="container">
//      <h1>)=====" + currentSSID + R"=====(</h1>
//      
//      <div class="current-temp">
//        <span>Temperatura Atual: <strong id="current-temp-value">)=====" + String(temperaturaAtual) + R"=====(°C</strong></span>
//        <button class="fake-toggle )=====" + String(modoFakeAtivo ? "active" : "") + R"=====(" 
//                onclick="setFakeMode()">
//          )=====" + String(modoFakeAtivo ? "AJUSTE ✓" : "AJUSTE") + R"=====(
//        </button>
//      </div>
//      <div class="fake-temp" style="text-align: center; margin-bottom: 15px;">
//        Temperatura Ajustada: <span id="fake-temp-value">)=====" + String(temperaturaFake) + R"=====(</span>°C
//      </div>
//      
//      <div class="temp-control">
//        <div class="temp-header">Temperatura Ajustada</div>
//        <div class="temp-adjust">
//          <button class="temp-btn" onclick="updateAjuste(-1)">-</button>
//          <div class="temp-value" id="ajuste-value">)=====" + String(c) + R"=====(</div>
//          <button class="temp-btn" onclick="updateAjuste(1)">+</button>
//        </div>
//      </div>
//      
//      <div class="mode-buttons">
//        <button id="mode-manual" class="mode-btn )=====" + String(!modoAutomatico && !modoFakeAtivo ? "active" : "inactive") + R"=====(" 
//                onclick="setMode(1)">
//          MANUAL
//        </button>
//        <button id="mode-auto" class="mode-btn )=====" + String(modoAutomatico && !modoFakeAtivo ? "active" : "inactive") + R"=====(" 
//                onclick="setMode(2)">
//          AUTOMATICO
//        </button>
//      </div>
//      
//      <div class="status-info">
//        Status da Janela: 
//        <span class=")=====" + String(janelaAberta ? "status-aberta" : "status-fechada") + R"=====(">
//          )=====" + String(janelaAberta ? "ABERTA" : "FECHADA") + R"=====(
//        </span>
//      </div>
//      
//      <div class="control-panel">
//        <button class="btn )=====" + String(janelaAberta ? "btn-close" : "btn-open") + R"=====(" 
//                onclick="toggleJanela()" )=====" + String(modoAutomatico || modoFakeAtivo ? "disabled style='opacity:0.5; cursor:not-allowed'" : "") + R"=====(">
//          )=====" + String(janelaAberta ? "FECHAR JANELA" : "ABRIR JANELA") + R"=====(
//        </button>
//      </div>
//      
//      <div class="temp-control">
//        <div class="temp-header">Temperatura de Abertura</div>
//        <div class="temp-adjust">
//          <button class="temp-btn" onclick="updateTemp('abertura', -1)">-</button>
//          <div class="temp-value" id="abertura">)=====" + String(m_VariableHdlr.GetTemperaturaDeAbertura()) + R"=====(</div>
//          <button class="temp-btn" onclick="updateTemp('abertura', 1)">+</button>
//        </div>
//      </div>
//      
//      <div class="temp-control">
//        <div class="temp-header">Temperatura de Fechamento</div>
//        <div class="temp-adjust">
//          <button class="temp-btn" onclick="updateTemp('fechamento', -1)">-</button>
//          <div class="temp-value" id="fechamento">)=====" + String(TemperaturaDeFechamento) + R"=====(</div>
//          <button class="temp-btn" onclick="updateTemp('fechamento', 1)">+</button>
//        </div>
//      </div>
//      
//      <div class="ssid-control">
//        <div class="ssid-header">Alterar Nome da Rede (SSID)</div>
//        <input type="text" id="new-ssid" class="ssid-input" placeholder="Novo nome da rede" 
//               value=")=====" + currentSSID + R"=====(" maxlength="32">
//        <button class="ssid-btn" onclick="updateSSID()">Atualizar SSID</button>
//      </div>
//      
//      <div class="password-control">
//        <div class="password-header">Alterar Senha da Rede</div>
//        <input type="password" id="new-password" class="password-input" placeholder="Nova senha (min. 8 caracteres)" 
//               value=")=====" + currentPassword + R"=====(" maxlength="64">
//        <button class="password-btn" onclick="updatePassword()">Atualizar Senha</button>
//      </div>
//    </div>
//  </body>
//  </html>
//  )=====";
//  server.send(200, "text/html", html);
//}
//
// void handleSetTemp() {
//  if(server.hasArg("temp")) {
//    temperaturaAtual = server.arg("temp").toInt();
//    server.send(200, "text/plain", "OK");
//    Serial.print("Temperatura simulada alterada para: ");
//    Serial.println(temperaturaAtual);
//    if(modoAutomatico) {
//      if(temperaturaAtual >= TemperaturaDeAbertura && !janelaAberta) {
//        fecharJanelawebCmd = true;
//      } else if(temperaturaAtual <= TemperaturaDeFechamento && janelaAberta) {
//        fecharJanelawebCmd = true;
//      }
//    }
//  } else {
//    server.send(400, "text/plain", "Erro");
//  }
//}

//void handleUpdateTemp() {
//  if(server.hasArg("abertura") && server.hasArg("fechamento")) {
//    TemperaturaDeAbertura = server.arg("abertura").toInt();
//    TemperaturaDeFechamento = server.arg("fechamento").toInt();
//    
//    if(TemperaturaDeAbertura > 99) TemperaturaDeAbertura = 25;
//    if(TemperaturaDeFechamento < -99) TemperaturaDeFechamento = 15;
//    
//    if(TemperaturaDeAbertura <= TemperaturaDeFechamento) {
//      TemperaturaDeFechamento = TemperaturaDeAbertura - 1;
//    }
//    
//    if(TemperaturaDeAbertura > TemperaturaDeFechamento) {
//      EEPROM.put(eepromTempMaior, TemperaturaDeAbertura);
//      EEPROM.put(eepromTempMenor, TemperaturaDeFechamento);
//      EEPROM.commit();
//      server.send(200, "text/plain", "OK");
//      if(modoAutomatico) {
//        if(temperaturaAtual >= TemperaturaDeAbertura && !janelaAberta) {
//          fecharJanelawebCmd = true;
//        } else if(temperaturaAtual <= TemperaturaDeFechamento && janelaAberta) {
//              fecharJanelawebCmd = true;
//        }
//      }
//    } else {
//      server.send(400, "text/plain", "Temperatura de abertura deve ser maior que de fechamento");
//    }
//  } else {
//    server.send(400, "text/plain", "Parametros faltando");
//  }
//}
