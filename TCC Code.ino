//Bibliotecas
#include "WiFi.h"
#include "PubSubClient.h"
#include "ESPmDNS.h"
#include "DHTesp.h"
#include "heltec.h"
 
//Net Setup
#define NET_SSID "Toin Boiadeiro"
#define NET_PASSWORD "26149898"

//MQTT Setup
#define MQTT_ID "tcctermostato-esp32-0800313131"
#define MQTT_BROKER "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_MILLIS_TOPIC "tcc_termostato_millis"
#define MQTT_SECONDS_TOPIC "tcc_termostato_seconds"
#define MQTT_DHT_TEMP_TOPIC "tcc_termostato_temp"
#define MQTT_DHT_HUMD_TOPIC "tcc_termostato_humd"
#define MQTT_NIVEL_TOPIC "tcc_termostato_nivel"
#define MQTT_LED_TOPIC "gaab-led-power-manage"

// Set ports
#define LED_R 13

// Define net client
WiFiClient espClient; 

// Define mqtt client
PubSubClient MQTT(espClient);

//Variáveis que armazenam as informações que são enviadas ao broker
char millis_str[10] = "";
char seconds_str[10] = "";
char temp_str[10] = "";
char humd_str[10] = "";
char nivel_str[10] = "";

//Definição da Tela e Sensor DHT22
#define BAND    915E6  //Escolha a frequência
DHTesp dht;

//Variavéis do Nivel d'água
using namespace std;
bool isPressed = "";
int nivel = 0;
int buttonState = 0;

//Variavéis do Sensor DHT22
float currentTemp;
float currentHumidity;

// Variável global para armazenar o tempo inicial
unsigned long tempoInicial;

//Protótipo da função
void getTemp();
void getHumidity();
void getNivel();
void sendPacket();

void setupWifi(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  } else {
    // Connect with WiFi

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(NET_SSID);
  
    WiFi.begin(NET_SSID, NET_PASSWORD);

    // Loop para checar a conexão

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    // Display connection info

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP Address:");
    Serial.println(WiFi.localIP());
    }
}

void setupMQTT(){
    //Config MQTT Broker connection

    MQTT.setServer(MQTT_BROKER, MQTT_PORT);
    MQTT.setCallback(mqtt_ifrj_callback); 

    //Executar conexão

    while (!MQTT.connected()){
        Serial.print("- MQTT Setup: Tentando se conectar ao Broker MQTT: ");
        Serial.println(MQTT_BROKER);

        if(MQTT.connect(MQTT_ID)){
            Serial.println("- MQTT Setup: Conectado com sucesso");
             MQTT.subscribe(MQTT_LED_TOPIC);
        } else {
            Serial.println("- MQTT Setup: Falha ao se conectar, tentando novamente em 2s");
            delay(2000);
        }
    }
}

//Função que ler a temperatura e atibiu a variável currentTemp.
void getTemp()
{

  //Atribuindo a variavel float a função
  float temperature = dht.getTemperature();

  //Verifica se houve mudança na temperatura
  if (temperature != currentTemp) {
    currentTemp = temperature;
    
    // Liga o LED
    digitalWrite(LED, HIGH);
    
    // Espera 500 milissegundos
    delay(500);
    
    // Desliiga o LED
    digitalWrite(LED, LOW);
    
    // Espera 500 milissegundos
    delay(500);
  }
  delay(1000);
}

//Função que ler a Umidade e atibiu a variável currentHumidity.
void getHumidity()
{
  //Atribuindo a variavel float a função
  float humidity = dht.getHumidity();

  //Verifica se houve mudança na temperatura
  if (humidity != currentHumidity) {
    currentHumidity = humidity;
    
    // Liga o LED
    digitalWrite(LED, HIGH);
    
    // Espera 500 milissegundos
    delay(500);
    
    // Desliiga o LED
    digitalWrite(LED, LOW);
    
    // Espera 500 milissegundos
    delay(500);                
  }
  delay(1000);
}

//Função referente ao funcionamento do nivel d'agua
void getNivel()
{

    if (digitalRead(36) == 1)
    {
        nivel = 4;
    }
    
    else if (digitalRead(37) == 1)
    {
        nivel = 3;
    }

    else if (digitalRead(38) == 1)
    {
        nivel = 2;
    }

    else if (digitalRead(39) == 1)
    {
        nivel = 1;
    }
    else
    {
        nivel = 0;
    }

}

//O baudrate no monitor serial é (115200)
void setup(void)
{

    // Armazene o tempo inicial em milissegundos
    tempoInicial = millis();
    
    //inicializa o LED
    pinMode(LED, OUTPUT);
  
    //Habilita diversas Configurações do Lora32
    Heltec.begin(true, true, true , true , BAND);
    /*(
      true = Habilita o Display, 
      true = Heltec.Heltec.Heltec.LoRa Disable, 
      true = Habilita debug Serial, 
      true = Habilita o PABOOST, 
      BAND = Frequência BAND.
    )*/
  
    //Primeira Tela: Exibe os criadores do projeto enquanto de conecta ao Wi-Fi e ao Broker MQTT
    Heltec.display->init();
    Heltec.display->setContrast(255);
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_24);
    Heltec.display->drawString(0, 0, "IFresources");
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(0, 25, "@ifrjniteroi");
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 45, "Conectando ao Wi-Fi...");
    Heltec.display->display();
    delay(1000);
  
    //inicializa o DHT no pino 17
    dht.setup(25, DHTesp::DHT22); 
  
    //Atribuindo as variaveis as funções
    currentTemp = dht.getTemperature();
    currentHumidity = dht.getHumidity();

    //Pinos referentes ao nivel d'água
    pinMode(36, INPUT);
    pinMode(37, INPUT);
    pinMode(38, INPUT);
    pinMode(39, INPUT);

    // Pinmode
    pinMode(LED_R, OUTPUT);

    // Call setup wifi
    setupWifi();
    // Call setup mqtt
    setupMQTT();
}

void loop(void)
{
  //Funções de Leitura de Temperatura e Umidade
  getTemp();
  getHumidity();
  getNivel();

  // Converte a função millis para uma função que exibe os segundos
  unsigned long tempoAtual = millis();
  unsigned long segundos = (tempoAtual - tempoInicial) / 1000;

  //Segunda Tela: Exibe os valores de temperatura na tela do Lora32.
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_16);
  Heltec.display->drawString(30, 5, "Temperatura"); //"Enviando"
  Heltec.display->drawString(33, 30, (String)currentTemp);
  Heltec.display->drawString(78, 30, "°C");
  Heltec.display->display();

  //Monitor Serial
  Serial.println("Tempo em Milissegundos: " + (String)millis());
  Serial.println("Tempo em Segundos: " + (String)segundos);  
  Serial.println("Temperatura: " + (String)currentTemp + "°C");
  Serial.println("Umidade: " + (String)currentHumidity + "%");
  Serial.println("Nivel da Água: " + (String)nivel);
  Serial.println("---");

  //Envia Milissegundos
  sprintf(millis_str, "%d", millis());
  MQTT.publish(MQTT_MILLIS_TOPIC, millis_str);

  //Envia Segundos
  sprintf(seconds_str, "%s", (String)segundos);
  MQTT.publish(MQTT_SECONDS_TOPIC, seconds_str);

  //Envia Temperatura
  sprintf(temp_str, "%s", (String)currentTemp );
  MQTT.publish(MQTT_DHT_TEMP_TOPIC, temp_str);

  //Envia Umidade
  sprintf(humd_str, "%s", (String)currentHumidity );
  MQTT.publish(MQTT_DHT_HUMD_TOPIC, humd_str);

  //Envia o nivel da água
  sprintf(nivel_str, "%s", (String)nivel);
  MQTT.publish(MQTT_NIVEL_TOPIC, nivel_str);

  //Configuração de Conexões e Configuração de Delay
  setupWifi();
  setupMQTT();
  MQTT.loop();
  delay(1000);
}

// Callback function
// Called when data is received in one of topics
void mqtt_ifrj_callback(char* topic, byte* payload, unsigned int length)
{
  String msg;
  Serial.print("- MQTT Callback Topic: ");
  Serial.println(topic);

  //obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  //Controlando LED RED
  if (msg.equals("R1"))
    {
      digitalWrite(LED_R, HIGH);
      Serial.println("- MQTT Sub Conn: R1 Received");
    } else if (msg.equals("R0"))
    {
      digitalWrite(LED_R, LOW);
      Serial.println("- MQTT Sub Conn: R0 Received");
    }
  
}