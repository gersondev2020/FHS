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
* LIBRARIES AND AUXILIARY FILES
*******************************************************************************/
#include <TimeLib.h>
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include "FLEX_HOME_SMARTDef.h"

//Prototypes
//void initMQTT(String broker, int porta);
//void mqtt_callback(char* topic, byte* payload, unsigned int length);

/*******************************************************************************
* FUNCTIONS
*******************************************************************************/
void hold(const unsigned int &ms) {
  // Non blocking delay
  unsigned long m = millis();
  while (millis() - m < ms) {
    yield();
  }
}

String dateTimeStr(const time_t &t, const bool flBr = true) {
  // Return time_t as String "yyyy-mm-dd hh:mm:ss" para "dd/mm/yyyy hh:mm:ss"
  String sFn;
  if(statusNTP || flBr == true){
  if (t == 0) {
    return F("N/A");
  } else if (flBr) {
    // dd/mm/yyyy hh:mm:ss
    sFn = "";
    if (day(t) < 10) {
      sFn += '0';
    }
    sFn += String(day(t)) + '/';
    if (month(t) < 10) {
      sFn += '0';
    }
    sFn += String(month(t)) + '/' + String(year(t)) + ' ';
  } else {
    // yyyy-mm-dd hh:mm:ss
    sFn = String(year(t)) + '-';
    if (month(t) < 10) {
      sFn += '0';
    }
    sFn += String(month(t)) + '-';
    if (day(t) < 10) {
      sFn += '0';
    }
    sFn += String(day(t)) + ' ';
  }
  
  if (hour(t) < 10) {
    sFn += '0';
  }
  sFn += String(hour(t)) + ':';
  if (minute(t) < 10) {
    sFn += '0';
  }
  sFn += String(minute(t)) + ':';
  if (second(t) < 10) {
    sFn += '0';
  }
  sFn += String(second(t));
  return sFn;
  }else{ // // caso não esteja conectado ao wifi ou não conector ao servido de horas
    // yyyy-mm-dd hh:mm:ss
    sFn = String(year(t)+10) + '-';
    if (month(t) < 10) {
      sFn += '0';
    }
    sFn += String(month(t)+10) + '-';
    if (day(t) < 10) {
      sFn += '0';
    }
    sFn += String(day(t)+20) + ' ';
  
  
  if (hour(t) < 10) {
    sFn += '0';
  }
  sFn += String(hour(t)+3) + ':';
  if (minute(t) < 10) {
    sFn += '0';
  }
  sFn += String(minute(t)+4) + ':';
  if (second(t) < 10) {
    sFn += '0';
  }
  sFn += String(second(t)+5);
  return sFn;
 }
}

String convert_para_hex(const unsigned long &h, const byte &l = 8) {
  // Return value as Hexadecimal String
  String sFn;
  sFn= String(h, HEX);
  sFn.toUpperCase();
  sFn = ("00000000" + sFn).substring(sFn.length() + 8 - l);
  return sFn;
}

String deviceID() {
  // Return the Device ID  
  return "FHS" + convert_para_hex(ESP.getChipId());
  
}

String tempo_ativoStr(const time_t &t){
  // Return time_t as String "d:hh:mm:ss"
  String sFn = String(t / SECS_PER_DAY) + ':';
  if (hour(t) < 10) {
    sFn += '0';
  }
  sFn += String(hour(t)) + ':';
  if (minute(t) < 10) {
    sFn += '0';
  }
  sFn += String(minute(t)) + ':';
  if (second(t) < 10) {
    sFn += '0';
  }
  sFn += String(second(t));
  return sFn;
}

int timeZone() {
  // Return Time Zone config value
  return int8(EEPROM.read(CFG_TIME_ZONE));
}

boolean pwdNeeded() {
  // Return SEMPRE_SOLICITAR_SENHA config value
  return EEPROM.read(SEMPRE_SOLICITAR_SENHA);
}

boolean softAPOn() {
  // Return SOFTAP_SEMPRE_LIGADO config value
    return EEPROM.read(SOFTAP_SEMPRE_LIGADO);
}

String networkID() {
  // Return Network ID config parameter
  String sFn = "";
  char c;
  for (int bFn = CFG_WIFI_PWD; bFn < CFG_NETWORK_ID; bFn++) {
    if (EEPROM.read(bFn) == 0) {
      break;
    }
    c = char(EEPROM.read(bFn));
    // Check for valid characters
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || c == '_') {
      sFn += c;
    }
  }
  if (sFn == "") {
    sFn = deviceID();
  }
  return sFn;
}
// Ler nome de dispositivo salvo na memoria eeprom
String nome_dispositivos(byte disp) {
  // Return Reference config parameter
  String sFn = "";
  switch (disp) {
    case 0:  
   for (int bFn = POSICAO_INICIAL_EEPROM; bFn < NOME_DISP_1_EEPROM; bFn++) {
    if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break;
     case 1: 
   for (int bFn = NOME_DISP_1_EEPROM; bFn < NOME_DISP_2_EEPROM; bFn++) {
    if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
     break;
     case 2:  
   for (int bFn = NOME_DISP_2_EEPROM; bFn < NOME_DISP_3_EEPROM; bFn++) {
    if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break;
    case 3:  
   for (int bFn = NOME_DISP_3_EEPROM; bFn < NOME_DISP_4_EEPROM; bFn++) {
    if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break;
  }
  if(sFn == ""){return "Dispositivo "+String(disp+1); }else{return sFn;}
  
}
// Ler da memoria eeprom nome wifi da rede salva
String wifiSSID() {
  // Return WiFi SSID config parameter
  String sFn = "";
  for (int bFn = SOFTAP_SEMPRE_LIGADO+1; bFn < CFG_WIFI_SSID; bFn++) {
    if (EEPROM.read(bFn) == 0) {
      break;
    }
    sFn += char(EEPROM.read(bFn));
  }
  if(sFn == ""){return "Nome rede WI-FI"; }else{return sFn;}
}
// Ler da memoria eeprom senha wifi da rede salva
String wifiPwd() {
  // Return WiFi password config parameter
  String sFn = "";
  for (int bFn = CFG_WIFI_SSID; bFn < CFG_WIFI_PWD; bFn++) {
    if (EEPROM.read(bFn) == 0) {
      break;
    }
    sFn += char(EEPROM.read(bFn));
  }
  return sFn;
}
// Ler dados do servidor MQTT salvo na memoria eeprom
String dados_do_broker_gravados(byte dados) {
  // Return Reference config parameter
  String sFn = "";
  switch (dados) {
    case 0:  
     for (int bFn = NOME_DISP_4_EEPROM; bFn < BROKER_MQTT; bFn++) {
     if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break; 
    case 1:  
     for (int bFn = BROKER_MQTT; bFn < USUARIO_MQTT; bFn++) {
     if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break;    
    case 2:  
     for (int bFn = USUARIO_MQTT; bFn < SENHA_MQTT; bFn++) {
     if (EEPROM.read(bFn) == 0) {
     break;
    }
    sFn += char(EEPROM.read(bFn));
    }
    break;   
    case 3:  
     //sFn = "17122";
     for (int bFn = CFG_TIME_ZONE+1; bFn < PORTA_MQTT; bFn++) {
     if (EEPROM.read(bFn) == 0) {
     break;
     }
     sFn += char(EEPROM.read(bFn));

    }
    break;  
  }   
  return sFn;
}
//=================== Fonções do serviço MQTT ========================
// Comando saidas de acordo com a mensargem recebida via MQTT
void comandoSaidaMQTT(String msg){
  String MSG_MQTT = "";
  if(msg.equals("D1")){
    digitalWrite(PINO_SAIDA_1, !digitalRead(PINO_SAIDA_1));
      if (digitalRead(PINO_SAIDA_1)) {
        ultimo_evento_btn1 = "Ligado via APP " + dateTimeStr(now());  
      } else {
        ultimo_evento_btn1 = "Desligado via APP " + dateTimeStr(now());
      }
      MSG_MQTT = ultimo_evento_btn1 + " D1";
  }else if(msg.equals("D2")){
    digitalWrite(PINO_SAIDA_2, !digitalRead(PINO_SAIDA_2));
      if (digitalRead(PINO_SAIDA_2)) {
        ultimo_evento_btn2 = "Ligado via APP " + dateTimeStr(now());
      } else {
        ultimo_evento_btn2 = "Desligado via APP " + dateTimeStr(now());
      }
    MSG_MQTT = ultimo_evento_btn2 + " D2";
  }else if(msg.equals("D3")){
    digitalWrite(PINO_SAIDA_3, !digitalRead(PINO_SAIDA_3));
      if (digitalRead(PINO_SAIDA_3)) {
        ultimo_evento_btn3 = "Ligado via APP " + dateTimeStr(now());
      } else {
        ultimo_evento_btn3 = "Desligado via APP " + dateTimeStr(now());
      }
    MSG_MQTT = ultimo_evento_btn3 + " D3";
  }else if(msg.equals("D4")){
    digitalWrite(PINO_SAIDA_4, !digitalRead(PINO_SAIDA_4));
      if (digitalRead(PINO_SAIDA_4)) {
        ultimo_evento_btn4 = "Ligado via APP " + dateTimeStr(now());
      } else {
        ultimo_evento_btn4 = "Desligado via APP " + dateTimeStr(now());
      }  
      MSG_MQTT = ultimo_evento_btn4 + " D4";
  }
  if(msg.equals("Status")){ 
    String m = String(digitalRead(PINO_SAIDA_1) ? "Ligado " : "Desligado ")+" D1"+"&"+
               String(digitalRead(PINO_SAIDA_2) ? "Ligado " : "Desligado ")+" D2"+"&"+
               String(digitalRead(PINO_SAIDA_3) ? "Ligado " : "Desligado ")+" D3"+"&"+
               String(digitalRead(PINO_SAIDA_4) ? "Ligado " : "Desligado ")+" D4";
    String t = String(TOPICO_PUBLISH)+"/Status";
    MQTT.publish(t.c_str(), m.c_str());
  }else if(msg.equals("Dados")){ 
    String d = networkID()         +"&"+
               deviceID()          +"&"+
               nome_dispositivos(0)+"&"+
               nome_dispositivos(1)+"&"+
               nome_dispositivos(2)+"&"+
               nome_dispositivos(3);              
               
    String td = String(TOPICO_PUBLISH)+"/Dados";
    MQTT.publish(td.c_str(), d.c_str());
  }else{
  MQTT.publish(TOPICO_PUBLISH.c_str(), MSG_MQTT.c_str());
  }
}
// Função que "esculta" as mensagem MQTT
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
    String msg;
     //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
      char c = (char)payload[i];
      msg += c;
    }    
     comandoSaidaMQTT(msg);        
}
// Conectar ao servidor MQTT
void ConectarBROKER(const String& host, String porta) {
  static char pHost[64] = {0};
  strcpy(pHost, host.c_str()); 
  MQTT.setServer(pHost, porta.toInt()); 
  MQTT.setCallback(mqtt_callback); //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
// Enviar dados para o servidor
void EnviaEstadoOutputMQTT(String msg){    
      MQTT.publish(TOPICO_PUBLISH.c_str(), msg.c_str());    
}
// Reconectar ao servidor MQTT
void reconnectMQTT(){  
      while (!MQTT.connected() && cont != 2) {
                
        if (MQTT.connect(networkID().c_str(), dados_do_broker_gravados(1).c_str(), dados_do_broker_gravados(2).c_str())) 
        {
            MQTT.subscribe(TOPICO_SUBSCRIBE.c_str());             
            cont = 0;
            Serial.println("Conectado ao Broker MQTT: ");
        }else{
            hold(2000);
            cont ++;
            Serial.println("Tentando se conectar "+dados_do_broker_gravados(0)+ String(cont)+" "+"°");
        }
    }//while
}
//======================================================================

// Converter IP em String
String ipStr(const IPAddress &ip) {
  // Return IPAddress as String "n.n.n.n"
  String sFn = "";
  for (byte bFn = 0; bFn < 3; bFn++) {
    sFn += String((ip >> (8 * bFn)) & 0xFF) + ".";
  }
  sFn += String(((ip >> 8 * 3)) & 0xFF);
  return sFn;
}

// Status do WIFI em String
String wifiStatusStr() {
  // Return WiFi Status as a readable String
  String s;
  switch (WiFi.status()) {
    case WL_IDLE_STATUS:
      s = F("Ocioso");
      break;
    case WL_NO_SSID_AVAIL:
      s = F("SSID indisponível");
      break;
    case WL_SCAN_COMPLETED:
      s = F("Scan concluído");
      break;
    case WL_CONNECTED:
      s = F("Conectado");
      break;
    case WL_CONNECT_FAILED:
      s = F("Falha de conexão");
      break;
    case WL_CONNECTION_LOST:
      s = F("Conexão perdida");
      break;
    case WL_DISCONNECTED:
      s = F("Desconectado");
      WiFi.reconnect();
      break;
    default:
      s = F("N/D");
      break;
  }
  return s;
}

// Verifica se o usuario esta autenticado
boolean chkWebAuth() {
  // Check for Web Interface credencials
  if (server.authenticate(WWW_USERNAME, PASSWORD)) {
    // Authenticated
    return true;
  } else {
    // Need Username and Password
    server.sendHeader(F("WWW-Authenticate"), F("Basic realm=\"Flex Home Web Interface\""));
    server.send(401);
    return false;
  }
}

// Time em NTP 
time_t timeNTP() {
  // Return time_t from NTP Server
  if (wifiStatus != WL_CONNECTED) {
    // Se o wifi não estivar conectado
    return 0; // Retorna 0
    statusNTP = false;
  }
  // NTP Server
  const char  NTP_SERVER[]    = "pool.ntp.br";
  // NTP Packet Size
  const byte  NTP_PACKET_SIZE = 48;
  // UDP Port
  const int   UDP_LOCALPORT   = 2390;
  // NTP Packet
  byte        ntp[NTP_PACKET_SIZE];

  memset(ntp, 0, NTP_PACKET_SIZE);
  ntp[ 0] = 0b11100011; // LI, Version, Mode
  ntp[ 1] = 0;          // Stratum, or type of clock
  ntp[ 2] = 6;          // Polling Interval
  ntp[ 3] = 0xEC;       // Peer Clock Precision
  ntp[12] = 49;
  ntp[13] = 0x4E;
  ntp[14] = 49;
  ntp[15] = 52;
  // Get time from server
  WiFiUDP udp;
  udp.begin(UDP_LOCALPORT);
  udp.beginPacket(NTP_SERVER, 123);
  udp.write(ntp, NTP_PACKET_SIZE);
  udp.endPacket();
  hold(1000);
  unsigned long l;
  if (udp.parsePacket()) {
    // Success
    udp.read(ntp, NTP_PACKET_SIZE);
    l = word(ntp[40], ntp[41]) << 16 | word(ntp[42], ntp[43]);
    l -= 2208988800UL;      // Calculate from 1900 to 1970
    l += timeZone() * 3600; // Adjust time zone (+- timeZone * 60m * 60s)
    logStr[logIndex] = dateTimeStr(l) + F(";NTP;Ok");
    statusNTP = true;
  } else {
    //Error
    logStr[logIndex] = dateTimeStr(1) + F(";NTP;ERRO");
    l = 0;
    statusNTP = false;
  }
  // Simulate Log
  Serial.println(logStr[logIndex]);
  logIndex = (logIndex + 1) % LOG_ENTRIES;
  // Return DateTime
  return l;
  
}

//Reiniciar ESP
void reboot() {
  // Reboot device
  hold(2000);
  ESP.restart();
}


/*******************************************************************************
* SCHEDULE SYSTEM FUNCTIONS
* 10/2018 - Andre Michelon
* Options:
*   - agendamento at a specific Date/Time
*     On (High): SHyyyy-mm-dd hh:mm  diaiamente
*     Off (Low): SLyyyy-mm-dd hh:mm
*   - Por mês
*     On (High): MHdd hh:mm  mensal
*     Off (Low): MLdd hh:mm
*   - Semanal
*     On (High): WHd hh:mm  semanal
*     Off (Low): WLd hh:mm
*   - Daily
*     On (High): DHhh:mm    diaiamente
*     Off (Low): DLhh:mm
*   - Intervaled
*     On (High): IHhh:mm    intermalado
*     Off (Low): ILhh:mm
* Example Strings:
*   SH2018-10-12 16:30  - set On on Oct 12 2018 16:30
*   MH12 16:30          - set On monthly on day 12 16:30
*   WL6 16:30           - set Off weekly on Fridays 16:30 
*   DH16:30             - set On daily at 16:30
*   IH00:30             - set Off after being On for 30 minutes
*   IL00:10             - set On after being Off for 10 minutes
*******************************************************************************/

String agendamentoChk(const String &agendamento) {
  // agendamento System Main Function
  // Local variables
  String        event     = "";
  byte          relay;
  byte          pin;
  static time_t Ultima_verif = 0,
                highDT    = now(),
                lowDT     = highDT;

  // Check for Interval reset
  if (agendamento == "") {
    highDT = now(),
    lowDT = highDT;
    return "";
  }

  // Check for minimum interval (10s)
  if (now() - Ultima_verif < varredura) {
    return "";
  }

  // Get DateTime
  Ultima_verif = now(); // yyyy-mm-dd hh:mm

  // Get DateTime as "yyyy-mm-dd hh:mm" String

  // Obtem o Ano do DataTime yyyy-
  String dt = String(year(Ultima_verif)) + '-';
  
  // Obtem o Mês da DataTime mm-
  if (month(Ultima_verif) < 10) {dt += '0';}
  dt += String(month(Ultima_verif)) + '-';
  
  // Obtem o Dia da DataTime dd 
  if (day(Ultima_verif) < 10) {dt += '0';}
  dt += String(day(Ultima_verif)) + ' ';

  // Obtem o Horas da DataTime hh:
  if (hour(Ultima_verif) < 10) {dt += '0';}
  dt += String(hour(Ultima_verif)) + ':';

  // Obtem o Minutos da DataTime mm
  if (minute(Ultima_verif) < 10) {dt += '0';}  
  dt += String(minute(Ultima_verif));

  // dt = yyyy-mm-dd hh:mm nesse ponto
 
  dt = dt.substring(11); // Get DateTime as "hh:mm" String
  
  s = "D1H0" + dt; //D1H0612:00

  String agenda = agendamento;
  String diaSemana = String(weekday(Ultima_verif));
  String hora = dt;
  String evento = "";
  String HouL = "";
  if(horapula+1 == hora.substring(3).toInt()){
     horapula = 60;
    for(int e = 0; e < 5; e++){
      pula[e] = 0;      
    }
  }
  for(int n = 1; n <= 4; n++){
    for(int t = 0; t <= 1; t++){
    boolean(t) ? HouL = "H" : HouL = "L";    
    if(agenda.indexOf("D"+String(n)+HouL) != -1){
       for(int p = 1; p <= 15; p++){
        if((p > 0 && p <= 7)){
          if(agenda.indexOf("D"+String(n)+HouL+"0"+diaSemana+hora) != -1 && n != pula[n]){
               event = "D"+String(n)+HouL+"0"+diaSemana+hora; // D1H0112:00
               relay = boolean(t);           
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();                        
               goto passa;
          }
        }
        if(p > 7 && p <= 15){
          if(agenda.indexOf("D"+String(n)+HouL+"08"+hora) != -1  && n != pula[n]){
               event = "D"+String(n)+HouL+"08"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"09"+hora) != -1 && ((diaSemana != "7" && diaSemana != "1") && n != pula[n])){
               event = "D"+String(n)+HouL+"09"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"10"+hora) != -1 && ((diaSemana == "7" || diaSemana == "1") && n != pula[n])){
               event = "D"+String(n)+HouL+"10"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"11"+hora) != -1 && (diaSemana != "1" && n != pula[n])){
               event = "D"+String(n)+HouL+"11"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"12"+hora) != -1 && ((diaSemana == "2" || diaSemana == "4" || diaSemana == "6") && n != pula[n])){
               event = "D"+String(n)+HouL+"12"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"13"+hora) != -1 && ((diaSemana == "3" || diaSemana == "5" || diaSemana == "7") && n != pula[n])){
               event = "D"+String(n)+HouL+"13"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"14"+hora) != -1 && ((diaSemana == "2" || diaSemana == "3" || diaSemana == "4") && n != pula[n])){
               event = "D"+String(n)+HouL+"14"+hora;
               relay = boolean(t);
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }
          if(agenda.indexOf("D"+String(n)+HouL+"15"+hora) != -1 && ((diaSemana == "5" || diaSemana == "6" || diaSemana == "7") && n != pula[n])){
               event = "D"+String(n)+HouL+"15"+hora;
               relay = boolean(t); 
               switch (n) {case 1: pin = PINO_SAIDA_1;
                        break;
                           case 2: pin = PINO_SAIDA_2;
                        break;
                           case 3: pin = PINO_SAIDA_3;
                        break;
                           case 4: pin = PINO_SAIDA_4;
                        break;}  
                        pula[n] = n;
                        horapula = hora.substring(3).toInt();  
               goto passa;
          }//if(agenda
        } // if
      } //for
    }  //if         
  }   // for 
}    // for 
//D1H00HH:MM a D4H15HH:MM
//D1L00HH:MM a D4L15HH:MM

//00 Nem um dia     // Dia ""
//02 Seg            // Dia 2
//03 Ter            // Dia 3
//04 Qua            // Dia 4
//05 Qui            // Dia 5
//06 Sex            // Dia 6
//07 Sab            // Dia 7
//01 Dom            // Dia 1
//08 Seg a Dom      // Dia 2,3,4,5,6,7,1
//09 Seg a Sex      // Dia 2,3,4,5,6
//10 Sab e Dom      // Dia 7 e 1
//11 Seg a Sab      // Dia 2,3,4,5,6,7
//12 Seg, Qua e Sex // Dia 2, 4 e 6
//13 Ter, Qui e Sab // Dia 3, 5 e 7
//14 Seg a Qua      // Dia 2,3,4
//15 Qui a Sab      // Dia 5,6,7
  passa:
  if (event != "" && relay != digitalRead(pin)) {
    digitalWrite(pin, relay);    
    if (relay) {
      // Store HIGH DateTime
      highDT = Ultima_verif;
    } else {
      // Store LOW DateTime
      lowDT = Ultima_verif;
    }
    return event;
  }
  
  return "";
}

boolean agendamentoSet(const String &agendamento) {
  // Save agendamento entries
  File file = SPIFFS.open(F("/Sched.txt"), "w+");
  if (!file) {
    //Serial.println("sem arquivos");  
    return false;  
     }else{ 
    file.println(agendamento);    
    //agendamentoChk("");
    //Serial.println("Arquivos passado set: "+agendamento);
    return true;
    }
     file.close();
}

String agendamentoGet() {
  // Get agendamento entries
  File file = SPIFFS.open(F("/Sched.txt"), "r");
  if (file) {
    file.setTimeout(100);
    String s = file.readString();
    //Serial.print("Arquivos lidos : "+s);
    file.close();
    return s;
  }else{
    return "";
  }
  
}

String ret_prog_agend(String agendamento, byte n_prog, byte dados){
  String programas = "";
  int r,l;  
  if(agendamento.indexOf("PG")!= -1){    
    for(byte f = 1; f < 17; f++){// defina o numero de programas mais 1
    if(n_prog == f){
       if(n_prog <  10) { r = agendamento.indexOf("PG0"+String(f));} else { r = agendamento.indexOf("PG"+String(f));}
       if(f != 16){ // defina o numero maximo de programas
       l = agendamento.indexOf("PG0"+String(f+1));
       }else{
       l=r+15; 
       }
      programas = agendamento.substring(r,l);//PG01:D1H0120:00
      switch (dados){
      case 1:
      programas = programas.substring(5,7); //D01
      break;
      case 2:
      programas = programas.substring(7,8); //H
      break;
      case 3:          
      programas = programas.substring(8,10); //1
      break;
      case 4:
      programas = programas.substring(10,15); //20:00
      break;
    } // fim switch de dados
   } // fim if de n_prog
  }    
 }// fim do if if(agendamento.indexOf("PG")!= -1) 
  return programas;
}// fim do ret_prog_agend
