/*
   ====================== G_AUTOMATION ===========================
   PROJETO FLEX HOME SMART
   13/07/20 V10
   GERSON FELIPE DE SOUSA
   DISPOSITIVO DE AUTOMAÇÃO RESIDENCIAL COM
   ACIONAMENTO VIA APP E VARIOS RECURSOS COMO
   MQTT, PROG HORARIS E MUITO MAIS
*/
/*******************************************************************************
  Bibriotecas
*******************************************************************************/
#include "FLEX_HOME_SMARTLib.h"
/*******************************************************************************
  Funcões auxiliares
*******************************************************************************/
String nome_arquivo_ino() { // Função que retorna nome do raquivo para log
  // Retornar o nome do arquivo de origem como String
  return String(__FILE__).substring(String(__FILE__).lastIndexOf("\\") + 1);
}
/*******************************************************************************
  WEB REQUESTS HANDLING
*******************************************************************************/
void handleRelay() { // Requests do acionamentos das saidas 
  // Acionamentos de saidas
  if (!pwdNeeded() || chkWebAuth()) {
    File file = SPIFFS.open(F("/Aciona.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String s = file.readString();
      file.close();
      // Replace markers
      s.replace("#Dispositivo_1#" , nome_dispositivos(0));
      s.replace("#Dispositivo_2#" , nome_dispositivos(1));
      s.replace("#Dispositivo_3#" , nome_dispositivos(2));
      s.replace("#Dispositivo_4#" , nome_dispositivos(3));
      s.replace("#IP_WIFI#" , ipStr(WiFi.localIP()));
      s.replace("#ID#" , deviceID());
      // envia dados
      server.send(200, F("text/html"), s);
    } else {
      server.send(500, F("text/plain"), F("Relay - ERROR 500"));
    }
  }
}

void handleRelayStatus(){ // Requests dos status das saidas 
  // Relay status
  if (!pwdNeeded() || chkWebAuth()) {
    String s = String(digitalRead(PINO_SAIDA_1)) + "&" + // dados 0
               dateTimeStr(now())                + "&" + // dados 1
               ultimo_evento_btn1                + "&" + // dados 2
               String(digitalRead(PINO_SAIDA_2)) + "&" + // dados 3
               dateTimeStr(now())                + "&" + // dados 4
               ultimo_evento_btn2                + "&" + // dados 5
               String(digitalRead(PINO_SAIDA_3)) + "&" + // dados 6
               dateTimeStr(now())                + "&" + // dados 7
               ultimo_evento_btn3                + "&" + // dados 8
               String(digitalRead(PINO_SAIDA_4)) + "&" + // dados 9
               dateTimeStr(now())                + "&" + // dados 10
               ultimo_evento_btn4                + "&" + // dados 11
               wifiStatusStr()                   + "&" + // dados 12
               deviceID()                        + "&" + // dados 13
               ipStr(WiFi.softAPIP())            + "&" + // dados 14
               wifiSSID()                        + "&" + // dados 15
               ipStr(WiFi.localIP());                    // dados 16               
    
    EnviaEstadoOutputMQTT(s); // Envia o stadois das saidas para o broker mqtt
    //String json = "{" + String(digitalRead(PINO_SAIDA_1)) + "}";
    server.send(200, F("text/plain"), s);
    
  }
}

void handleRelaySet() { // Requests de acionamentos das saidas
  // Set Relay status
  String MSG_MQTT = "";
  if (!pwdNeeded() || chkWebAuth()) {
    String SS = "";
    String s1 = server.arg("set1");
    String s2 = server.arg("set2");
    String s3 = server.arg("set3");
    String s4 = server.arg("set4");
    if (s1 == "1") {
      // Set relay on
      digitalWrite(PINO_SAIDA_1, !digitalRead(PINO_SAIDA_1));
      if (digitalRead(PINO_SAIDA_1)) {
        ultimo_evento_btn1 = "Ligado Dia " + dateTimeStr(now());
      } else {
        ultimo_evento_btn1 = "Desligado Dia " + dateTimeStr(now());
      }
      MSG_MQTT = ultimo_evento_btn1+ " D1";
      goto passa;
    }
    if (s2 == "1") {
      // Set relay on
      digitalWrite(PINO_SAIDA_2, !digitalRead(PINO_SAIDA_2));
      if (digitalRead(PINO_SAIDA_2)) {
        ultimo_evento_btn2 = "Ligado Dia " + dateTimeStr(now());
      } else {
        ultimo_evento_btn2 = "Desligado Dia " + dateTimeStr(now());
      }
      MSG_MQTT = ultimo_evento_btn2+ " D2";
      goto passa;
    }
    if (s3 == "1") {
      // Set relay on
      digitalWrite(PINO_SAIDA_3, !digitalRead(PINO_SAIDA_3));
      if (digitalRead(PINO_SAIDA_3)) {
        ultimo_evento_btn3 = "Ligado Dia " + dateTimeStr(now());
      } else {
        ultimo_evento_btn3 = "Desligado Dia " + dateTimeStr(now());
      }
      MSG_MQTT = ultimo_evento_btn3+ " D3";
      goto passa;
    }
    if (s4 == "1") {
      // Set relay on
      digitalWrite(PINO_SAIDA_4, !digitalRead(PINO_SAIDA_4));
      if (digitalRead(PINO_SAIDA_4)) {
        ultimo_evento_btn4 = "Ligado Dia " + dateTimeStr(now());
      } else {
        ultimo_evento_btn4 = "Desligado Dia " + dateTimeStr(now());
      }
      MSG_MQTT = ultimo_evento_btn4+ " D4";
    }
passa: // Se a variavel for set1 pula o proximo rele
    MQTT.publish(TOPICO_PUBLISH.c_str(), MSG_MQTT.c_str());
    server.send(200, F("text/plain"), s);
  }
}

void handleConfig() { // Requets da tela de configuração 
  // Config
  if (chkWebAuth()) {
    File file = SPIFFS.open(F("/Config.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String s = file.readString();
      file.close();
      // Replace markers
      s.replace(F("#networkID#"), networkID());
      s.replace(F("#Dispositivo_1#"), nome_dispositivos(0));
      s.replace(F("#Dispositivo_2#"), nome_dispositivos(1));
      s.replace(F("#Dispositivo_3#"), nome_dispositivos(2));
      s.replace(F("#Dispositivo_4#"), nome_dispositivos(3));
      s.replace(F("#timeZone#") , String(timeZone()));
      s.replace(F("#aut#")      , (pwdNeeded() ? " checked" : ""));
      s.replace(F("#ap#")       , (softAPOn() ? " checked" : ""));
      s.replace(F("#ssid#")     , wifiSSID());
      s.replace(F("#broker#")     , dados_do_broker_gravados(0));
      s.replace(F("#usuario#")    , dados_do_broker_gravados(1));
      s.replace(F("#senha#")      , dados_do_broker_gravados(2));
      s.replace(F("#porta#")      , dados_do_broker_gravados(3));
      s.replace(F("#sched#")      , agendamento);//agendamento
      // Send data
      server.send(200, F("text/html"), s);
    } else {
      server.send(500, F("text/plain"), F("Config - ERROR 500"));
    }
  }
}

void handleProgHoraria() { //Requests Programação horaria
  // Config
  if (chkWebAuth()) {
    File file = SPIFFS.open(F("/Prog_H.htm"), "r");
    if (file) {
      file.setTimeout(100);
      String s = file.readString();
      file.close();
      // Replace markers
      s.replace(F("#Dispositivo_1#"), nome_dispositivos(0));
      s.replace(F("#Dispositivo_2#"), nome_dispositivos(1));
      s.replace(F("#Dispositivo_3#"), nome_dispositivos(2));
      s.replace(F("#Dispositivo_4#"), nome_dispositivos(3));
      // Send data
      server.send(200, F("text/html"), s);
    } else {
      server.send(500, F("text/plain"), F("Config - ERROR 500"));
    }
  }
}

void dados_agenda_salvos() { // Mostra na tela de programação horaria a programação atual
  String s = "";
  for (byte i = 1; i < 17; i++) {
    for (byte n = 1; n < 5; n++) {
      s += ret_prog_agend(agendamento, i, n) + "&";
    }
  }
  server.send(200, F("text/plain"), s);

} //Fim dados_agenda_salvos

void handleSalvarConfig() {
  String Antes_AP_ON = "";
  String AP_ON = "";
  // Config Save
  if (chkWebAuth()) {
    // Check for number os fields received
    if (server.args() >= 6 && server.args() <= 1000) {
      // Save CFG_NETWORK_ID ============================================================================
      byte b = 0;
      String s = server.arg("id");
      s.trim();
      s.replace(" ", "_");
      s.toLowerCase();
      if (s == "") {
        s = deviceID();
      }
      for (int i = CFG_WIFI_PWD; i < CFG_NETWORK_ID; i++) {
        EEPROM.write(i, s[b++]);
      }
      // Save nome dispisitivo 1 ============================================================================
      b = 0;
      s = server.arg(F("Dispositivo_1"));
      s.trim();
      if (s == "") {
        s = "Dispositivo 1";
      }
      for (int i = POSICAO_INICIAL_EEPROM; i < NOME_DISP_1_EEPROM; i++) {
        EEPROM.write(i, s[b++]);
      }
      // Save Save nome dispisitivo 2 ============================================================================
      b = 0;
      s = server.arg(F("Dispositivo_2"));
      s.trim();
      if (s == "") {
        s = "Dispositivo 2";
      }
      for (int i = NOME_DISP_1_EEPROM; i < NOME_DISP_2_EEPROM; i++) {
        EEPROM.write(i, s[b++]);
      }
      // Save Save nome dispisitivo 3 ============================================================================
      b = 0;
      s = server.arg(F("Dispositivo_3"));
      s.trim();
      if (s == "") {
        s = "Dispositivo 3";
      }
      for (int i = NOME_DISP_2_EEPROM; i < NOME_DISP_3_EEPROM; i++) {
        EEPROM.write(i, s[b++]);
      }
      // Save Save nome dispisitivo 4 ============================================================================
      b = 0;
      s = server.arg(F("Dispositivo_4"));
      s.trim();
      if (s == "") {
        s = "Dispositivo 4";
      }
      for (int i = NOME_DISP_3_EEPROM; i < NOME_DISP_4_EEPROM; i++) {
        EEPROM.write(i, s[b++]);
      }
      // SALVAR NOME BROKER MQTT ============================================================================
      b = 0;
      s = server.arg(F("broker"));
      s.trim();
      if (s == "") {
        s = "";
      }
      for (int i = NOME_DISP_4_EEPROM; i < BROKER_MQTT; i++) {
        EEPROM.write(i, s[b++]);
      }
      // SALVAR USUARIO BROKER MQTT  ============================================================================
      b = 0;
      s = server.arg(F("usuario"));
      s.trim();
      if (s == "") {
        s = "";
      }
      for (int i = BROKER_MQTT; i < USUARIO_MQTT; i++) {
        EEPROM.write(i, s[b++]);
      }
      // SALVAR SENHA BROKER MQTT  ============================================================================
      b = 0;
      s = server.arg(F("senha"));
      s.trim();
      if (s == "") {
        s = "";
      }
      for (int i = USUARIO_MQTT; i < SENHA_MQTT; i++) {
        EEPROM.write(i, s[b++]);
      }
      // SALVAR PORTA BROKER MQTT  ============================================================================
      b = 0;
      s = server.arg(F("porta"));
      s.trim();
      if (s == "") {
        s = "17122";
      }
      for (int i = CFG_TIME_ZONE+1; i < PORTA_MQTT; i++) {
        EEPROM.write(i, s[b++]);
      }
      // ============================================================================
      // Save CFG_TIME_ZONE
      EEPROM.write(CFG_TIME_ZONE,   server.arg(F("timeZone")).toInt());
      // Save SEMPRE_SOLICITAR_SENHA
      EEPROM.write(SEMPRE_SOLICITAR_SENHA, server.arg(F("aut")).toInt());
      // Save SOFTAP_SEMPRE_LIGADO
      Antes_AP_ON = softAPOn();
      EEPROM.write(SOFTAP_SEMPRE_LIGADO, server.arg(F("ap")).toInt());      
      AP_ON = softAPOn();
      // Save CFG_WIFI_SSID
      b = 0;
      s = server.arg(F("ssid"));
      s.trim();
      for (int i = SOFTAP_SEMPRE_LIGADO+1; i < CFG_WIFI_SSID; i++) {
        EEPROM.write(i, s[b++]);
      }
      // Save CFG_WIFI_PWD
      s = server.arg(F("pwd"));
      s.trim();
      if (s != "") {
        b = 0;
        for (int i = CFG_WIFI_SSID; i < CFG_WIFI_PWD; i++) {
          EEPROM.write(i, s[b++]);
        }
      }
      // Save to flash
      EEPROM.commit();
      // Save agendamento entries
      agendamentotestes = server.arg(F("sched"));

      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração salva.');window.location = 'config';</script></html>"));
      if (Antes_AP_ON != AP_ON){
        //handleReiniciar();
      }
    } else {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
    }
  }
}
void handleReiniciar() {
  // Reboot
  if (chkWebAuth()) {
    File file = SPIFFS.open(F("/Reboot.htm"), "r");
    if (file) {
      server.streamFile(file, F("text/html"));
      file.close();
      reboot();      
    } else {
      server.send(500, F("text/plain"), F("Reboot - ERROR 500"));
    }
  }
}
void handleProg_H_Save() {
  // Config Save
  if (chkWebAuth()) {
    agendamento =     "Pg01:" + server.arg(F("1DS")) + server.arg(F("1DH")) + server.arg(F("1DI")) + server.arg(F("1hora")) +
                    "\nPg02:" + server.arg(F("2DS")) + server.arg(F("2DH")) + server.arg(F("2DI")) + server.arg(F("2hora")) +
                    "\nPg03:" + server.arg(F("3DS")) + server.arg(F("3DH")) + server.arg(F("3DI")) + server.arg(F("3hora")) +
                    "\nPg04:" + server.arg(F("4DS")) + server.arg(F("4DH")) + server.arg(F("4DI")) + server.arg(F("4hora")) +
                    "\nPg05:" + server.arg(F("5DS")) + server.arg(F("5DH")) + server.arg(F("5DI")) + server.arg(F("5hora")) +
                    "\nPg06:" + server.arg(F("6DS")) + server.arg(F("6DH")) + server.arg(F("6DI")) + server.arg(F("6hora")) +
                    "\nPg07:" + server.arg(F("7DS")) + server.arg(F("7DH")) + server.arg(F("7DI")) + server.arg(F("7hora")) +
                    "\nPg08:" + server.arg(F("8DS")) + server.arg(F("8DH")) + server.arg(F("8DI")) + server.arg(F("8hora")) +
                    "\nPg09:" + server.arg(F("9DS")) + server.arg(F("9DH")) + server.arg(F("9DI")) + server.arg(F("9hora")) +
                    "\nPg10:" + server.arg(F("10DS")) + server.arg(F("10DH")) + server.arg(F("10DI")) + server.arg(F("10hora")) +
                    "\nPg11:" + server.arg(F("11DS")) + server.arg(F("11DH")) + server.arg(F("11DI")) + server.arg(F("11hora")) +
                    "\nPg12:" + server.arg(F("12DS")) + server.arg(F("12DH")) + server.arg(F("12DI")) + server.arg(F("12hora")) +
                    "\nPg13:" + server.arg(F("13DS")) + server.arg(F("13DH")) + server.arg(F("13DI")) + server.arg(F("13hora")) +
                    "\nPg14:" + server.arg(F("14DS")) + server.arg(F("14DH")) + server.arg(F("14DI")) + server.arg(F("14hora")) +
                    "\nPg15:" + server.arg(F("15DS")) + server.arg(F("15DH")) + server.arg(F("15DI")) + server.arg(F("15hora")) +
                    "\nPg16:" + server.arg(F("16DS")) + server.arg(F("16DH")) + server.arg(F("16DI")) + server.arg(F("16hora"));
    agendamento.trim();
    agendamento.toUpperCase();
    agendamentoSet(agendamento);
    // Done
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Programação Horaria salva.');window.location = 'ProgH';</script></html>"));
  } else {
    server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
  }
}

void handleLimpar() {
  // Config Save
  if (chkWebAuth()) {
    // Check for number os fields received
    if (server.args() >= 0 && server.args() <= 10) {
      for (int i = 0; i < CFG_TOTAL_LENGHT; i++) {
        if(SOFTAP_SEMPRE_LIGADO != i){
        EEPROM.write(i, 0);
        }
      }
      EEPROM.commit();  

      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Configuração Limpas.');window.location = 'limpar';</script></html>"));
    } else {
      server.send(200, F("text/html"), F("<html><meta charset='UTF-8'><script>alert('Erro de parâmetros.');history.back()</script></html>"));
    }
  }
}
void Acionamento_local(){
  String s = " LOCAL ";
  //================= Acionamento 1 ===========================
  if(digitalRead(PINO_ENTRAR_1)&& aux1){
    aux1 = false;
    digitalWrite(PINO_SAIDA_1, !digitalRead(PINO_SAIDA_1));
    ultimo_evento_btn1 = (digitalRead(PINO_SAIDA_1) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  if(!digitalRead(PINO_ENTRAR_1) && !aux1){
    aux1 = true;
    digitalWrite(PINO_SAIDA_1, !digitalRead(PINO_SAIDA_1));
    ultimo_evento_btn1 = (digitalRead(PINO_SAIDA_1) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  //================= Acionamento 2 ===========================
  if(digitalRead(PINO_ENTRAR_2)&& aux2){
    aux2 = false;
    digitalWrite(PINO_SAIDA_2, !digitalRead(PINO_SAIDA_2));
    ultimo_evento_btn2 = (digitalRead(PINO_SAIDA_2) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  if(!digitalRead(PINO_ENTRAR_2) && !aux2){
    aux2 = true;
    digitalWrite(PINO_SAIDA_2, !digitalRead(PINO_SAIDA_2));
    ultimo_evento_btn2 = (digitalRead(PINO_SAIDA_2) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  //================= Acionamento 3 ===========================
  if(digitalRead(PINO_ENTRAR_3) && aux3){
    aux3 = false;
    digitalWrite(PINO_SAIDA_3, !digitalRead(PINO_SAIDA_3));
    ultimo_evento_btn3 = (digitalRead(PINO_SAIDA_3) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  if(!digitalRead(PINO_ENTRAR_3) && !aux3){
    aux3 = true;
    digitalWrite(PINO_SAIDA_3, !digitalRead(PINO_SAIDA_3));
    ultimo_evento_btn3 = (digitalRead(PINO_SAIDA_3) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  //================= Acionamento 4 ===========================
  if(digitalRead(PINO_ENTRAR_4)&& aux4){
    aux4 = false;
    digitalWrite(PINO_SAIDA_4, !digitalRead(PINO_SAIDA_4));
    ultimo_evento_btn4 = (digitalRead(PINO_SAIDA_4) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
  if(!digitalRead(PINO_ENTRAR_4) && !aux4){
    aux4 = true;
    digitalWrite(PINO_SAIDA_4, !digitalRead(PINO_SAIDA_4));
    ultimo_evento_btn4 = (digitalRead(PINO_SAIDA_4) ? "Ligado " : "Desligado ") + 
    s + dateTimeStr(now());
  }
}
/*******************************************************************************
  SETUP
*******************************************************************************/
void setup() {
  // Serial
  Serial.begin(74880);
  // Slow down
  hold(1000);
  Serial.println();
  // Relay pin
  pinMode(PINO_ENTRAR_1, INPUT);
  pinMode(PINO_ENTRAR_2, INPUT_PULLUP);
  pinMode(PINO_ENTRAR_3, INPUT);
  pinMode(PINO_ENTRAR_4, INPUT_PULLUP);

  pinMode(PINO_SAIDA_1, OUTPUT);
  pinMode(PINO_SAIDA_2, OUTPUT);
  pinMode(PINO_SAIDA_3, OUTPUT);
  pinMode(PINO_SAIDA_4, OUTPUT);
  
  digitalWrite(PINO_SAIDA_1, LOW);
  digitalWrite(PINO_SAIDA_2, LOW);
  digitalWrite(PINO_SAIDA_3, LOW);
  digitalWrite(PINO_SAIDA_4, LOW);
  // File System begin
  if (!SPIFFS.begin()) {
  // reboot(F("ERRO Inicializando SPIFFS"));
  }

  EEPROM.begin(CFG_TOTAL_LENGHT);  
  Serial.println("Tamanho Eeprom: " + CFG_TOTAL_LENGHT);
  // WiFi - Access Point & Station
  WiFi.hostname(networkID().c_str());
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(true);
  WiFi.mode(softAPOn() ? WIFI_AP_STA : WIFI_STA);
  WiFi.begin(wifiSSID().c_str(), wifiPwd().c_str());  
  byte b = 0;
  while (wifiStatus == WL_DISCONNECTED && b < 60) {
    b++;
    Serial.print("|");
    hold(500);
    wifiStatus = WiFi.status();
  }
  if (wifiStatus == WL_CONNECTED) {
  // WiFi connected
    Serial.println("WiFi Ok Modo Station RSSI " + String(WiFi.RSSI()) + " IP " + ipStr(WiFi.localIP()));
   
    ConectarBROKER(dados_do_broker_gravados(0),dados_do_broker_gravados(3));

  }   
  // Check for SoftAP mode
  if (softAPOn() || wifiStatus != WL_CONNECTED) {
    if (wifiStatus != WL_CONNECTED) {
      // No Wifi connection, enter SoftAP
      WiFi.mode(WIFI_AP);
    }
    //WiFi.softAP(networkID().c_str(), PASSWORD);
      WiFi.softAP(networkID().c_str(), "12345678");
    // Set flag
    softAPActive = true;
    // Enable DNS routing
    dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
    dnsServer.start(DNSSERVER_PORT, "*", WiFi.softAPIP());
  }
  // Date/Time
  if (wifiStatus == WL_CONNECTED) {
    // Station mode, get date/time
    setSyncProvider(timeNTP);
    setSyncInterval(NTP_INT);
    if (timeStatus() != timeSet) {
    }
  } else {
    // Soft AP mode, ignore date/time
  }
  // Date/Time set

  // Register Boot
  bootDT = now();
  // mDNS
  if (!MDNS.begin(networkID().c_str())) {
  }
  // WebServer
  server.on(F("/relayStatus") , handleRelayStatus);
  server.on(F("/relaySet")    , handleRelaySet);
  server.on(F("/config")      , handleConfig);
  server.on(F("/ProgH")      ,  handleProgHoraria);
  server.on(F("/dados_agenda_salvos")   , dados_agenda_salvos);
  server.on(F("/configSave")  , handleSalvarConfig);
  server.on(F("/limpartudo")  , handleLimpar); 
  server.on(F("/reiniciar")  , handleReiniciar);
  server.on(F("/Prog_H_Save")  , handleProg_H_Save);
  server.onNotFound(handleRelay);
  server.collectHeaders(WEBSERVER_HEADER_KEYS, 1);
  server.begin();
  // agendamento entries
  agendamento = agendamentoGet();
  TOPICO_SUBSCRIBE = "Envia/"+deviceID();  
  TOPICO_PUBLISH   = "Recebe/"+deviceID(); 
  Serial.println(wifiSSID());
  Serial.println(wifiPwd());  
}//SETUP fim
/*******************************************************************************
  LOOP
*******************************************************************************/
void loop() {
  // Take care of WatchDog ---------------------------------------------------
  yield();
  // DNS Requests ------------------------------------------------------------
  if (softAPActive) {
    dnsServer.processNextRequest();
  }
  // Web Requests ------------------------------------------------------------
  server.handleClient();

  // WiFi Status -----------------------------------------------------------
  if (WiFi.status() != wifiStatus) {
    wifiStatus = WiFi.status();
    if (wifiStatus == WL_CONNECTED) {
      // WiFi reconnected, check No-IP
    }
  }
  if (wifiStatus == WL_CONNECTED) {
    if (!MQTT.connected() && dados_do_broker_gravados(0) != "") {      
        reconnectMQTT();
       }else{
        
       }
        MQTT.loop();
  }   
  // Registra evento na tela de rele toda quando o as saidas são acionada
  //pelo agendamento
  s = agendamentoChk(agendamento);
  String MSG_MQTT = "";
  if (s != "" && s.indexOf("D1") != -1) {
    ultimo_evento_btn1 = (digitalRead(PINO_SAIDA_1) ? "Ligado " : "Desligado ") +
                         s + " - " + dateTimeStr(now());
    MSG_MQTT = ultimo_evento_btn1+ " D1";                     
  }
  if (s != "" && s.indexOf("D2") != -1) {
    ultimo_evento_btn2 = (digitalRead(PINO_SAIDA_2) ? "Ligado " : "Desligado ") +
                         s + " - " + dateTimeStr(now());
    MSG_MQTT = ultimo_evento_btn2+ " D2";                     
  }
  if (s != "" && s.indexOf("D3") != -1) {
    ultimo_evento_btn3 = (digitalRead(PINO_SAIDA_3) ? "Ligado " : "Desligado ") +
                         s + " - " + dateTimeStr(now());
    MSG_MQTT = ultimo_evento_btn3+ " D3";                     
  }
  if (s != "" && s.indexOf("D4") != -1) {
    ultimo_evento_btn4 = (digitalRead(PINO_SAIDA_4) ? "Ligado " : "Desligado ") +
                         s + " - " + dateTimeStr(now());
    MSG_MQTT = ultimo_evento_btn4+ " D4";
  }
  if(!MSG_MQTT.equals("")){
    MQTT.publish(TOPICO_PUBLISH.c_str(), MSG_MQTT.c_str());
  }  
  Acionamento_local();
}
