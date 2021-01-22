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
* LIBRARIES
*******************************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <FS.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <TimeLib.h>
#include <PubSubClient.h>


// MQTT
String TOPICO_SUBSCRIBE = "";  
String TOPICO_PUBLISH   = ""; 

int BROKER_PORT = 17122; // Porta do Broker MQTT
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
int cont = 0;
/*******************************************************************************
* CONSTANT DEFINITION
*******************************************************************************/
// Build Information
const char      BUILD[]                         = __DATE__ " " __TIME__;

// Web Interface Username
const char      WWW_USERNAME[]                  = "admin";

// System Password
const char      PASSWORD[]                      = "admin";

// WebServer Port
const byte      WEBSERVER_PORT                  = 80;

// DNS Server Port
const byte      DNSSERVER_PORT                  = 53;

// WebServer Headers
const char*     WEBSERVER_HEADER_KEYS[]         = {"User-Agent"};

// Directory Max Number of Files
const byte      DIRECTORY_MAX_FILES             = 16;

// NTP sync interval
const int       NTP_INT                         = 6 * 60 * 60; // 6h

// Max log entries
const byte      LOG_ENTRIES                     = 50;

// Version Control URL
const char      VCS_URL[]                       = "https://dl.dropboxusercontent.com/s/d09eimuprehprys/ESPReleV.txt";

// Config Parameters
const int      POSICAO_INICIAL_EEPROM      =  0;
const int      NOME_DISP_1_EEPROM          = 25 + POSICAO_INICIAL_EEPROM;
const int      NOME_DISP_2_EEPROM          = 25 + NOME_DISP_1_EEPROM; 
const int      NOME_DISP_3_EEPROM          = 25 + NOME_DISP_2_EEPROM;
const int      NOME_DISP_4_EEPROM          = 25 + NOME_DISP_3_EEPROM;
const int      BROKER_MQTT                 = 30 + NOME_DISP_4_EEPROM;
const int      USUARIO_MQTT                = 30 + BROKER_MQTT;
const int      SENHA_MQTT                  = 30 + USUARIO_MQTT; 
const int      SEMPRE_SOLICITAR_SENHA      =  0 + SENHA_MQTT; 
const int      SOFTAP_SEMPRE_LIGADO        =  1 + SEMPRE_SOLICITAR_SENHA; 
const int      CFG_WIFI_SSID               = 30 + SOFTAP_SEMPRE_LIGADO; 
const int      CFG_WIFI_PWD                = 30 + CFG_WIFI_SSID; 
const int      CFG_NETWORK_ID              = 30 + CFG_WIFI_PWD;  
const int      CFG_TIME_ZONE               = 0  + CFG_NETWORK_ID;
const int      PORTA_MQTT                  = 10 + CFG_TIME_ZONE;
const int      CFG_TOTAL_LENGHT            = 0  + PORTA_MQTT;

/* Saidas do Esp
#define D0    16
#define D1    5
#define D2    4
#define D3    0
#define D4    2
#define D5    14
#define D6    12
#define D7    13
#define D8    15
#define D9    3
#define D10   1*/

const byte      PINO_ENTRAR_1        =  16;
const byte      PINO_ENTRAR_2        =  14;
const byte      PINO_ENTRAR_3        =  12;
const byte      PINO_ENTRAR_4        =  13;

const byte      PINO_SAIDA_1         =  1;
const byte      PINO_SAIDA_2         =  3;
const byte      PINO_SAIDA_3         =  4;
const byte      PINO_SAIDA_4         =  5;

boolean aux1 = false;
boolean aux2 = false;
boolean aux3 = false;
boolean aux4 = false;

// Software Version
const char      SW_VERSION[]                    = "1.0";

/*******************************************************************************
* ENTITIES
*******************************************************************************/
// Web Server
ESP8266WebServer  server(WEBSERVER_PORT);

// DNS Server
DNSServer         dnsServer;

/*******************************************************************************
* VARIABLES
*******************************************************************************/
// WiFi Status
wl_status_t       wifiStatus    = WL_DISCONNECTED;

// WiFi SoftAP flag
boolean           softAPActive  = false;

// File System Version
String            fsVersion     = "";

// Boot DateTime
time_t            bootDT        = 0;

// Last relay event
String            ultimo_evento_btn1     = "N/D";
String            ultimo_evento_btn2     = "N/D";
String            ultimo_evento_btn3     = "N/D";
String            ultimo_evento_btn4     = "N/D";

// System Log
String            logStr[LOG_ENTRIES];
byte              logIndex      = 0;

// Version Control System
String            vcsString     = "";
time_t            vcsNextCheck  = 0;

// Generic String;
String s;
boolean statusNTP = false;

// agendamento entries;
String agendamento;
String agendamentotestes;
int pula[5];
int horapula = 60;

int varredura = 10;

String Hmais1 = "";
String HorasDT = "";
