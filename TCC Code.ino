//Bibliotecas
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <DHT.h>
#include <SSD1306.h>
 
//Net Setup
#define NET_SSID "Wokwi-GUEST"
#define NET_PASSWORD ""

//MQTT Setup
#define MQTT_ID "tcctermostato-esp32-0800313131"
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_MILLIS_TOPIC "tcc_termostato_millis"
#define MQTT_DHT_TEMP_TOPIC "tcc_termostato_temp"
#define MQTT_DHT_HUMD_TOPIC "tcc_termostato_humd"

//Cliente de rede
WiFiClient espClient;

//Cliente MQTT
PubSubClient MQTT(espClient);

//Variáveis que armazenam as informações que são enviadas ao broker
char millis_str[10] = "";
char temp_str[10] = "";
char humd_str[10] = "";

//Display
SSD1306 display(0x3c, 21, 22);

//Sensor DHT22
DHT dht(15, DHT22);

//Temperatura
float t;

//Umidade
float h;


void setupWifi() {

//Configura a conexão à rede sem fio
  if (WiFi.status() == WL_CONNECTED)
        return;
  
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(NET_SSID);
  
  WiFi.begin(NET_SSID, NET_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void setupMQTT() {
  
//Informa qual broker e porta deve ser conectado
    MQTT.setServer(MQTT_BROKER, MQTT_PORT);
   
   while (!MQTT.connected()) 
    {
        Serial.print("*Tentando se conectar ao Broker MQTT: ");
        Serial.println(MQTT_BROKER);
        if (MQTT.connect(MQTT_ID)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentativa de conexao em 2s");
            delay(2000);
        }
    }
}

void setup(void) {

//Configura o baudrate da comunicação serial
  Serial.begin(115200);

  dht.begin();
  display.init();
  display.clear();
 
  setupWifi();

  setupMQTT();

}
 
void loop(void) {

//Strings de temperatura e umidade
  h = dht.readHumidity();
  t = dht.readTemperature();

//Configuração do Display
  display.clear();
  display.drawRect(10, 10, 80, 50);
  display.setFont(ArialMT_Plain_16);
  display.drawString(20, 14, (String)t+" °C");
  display.drawString(20, 35, (String)h+" %");
  display.flipScreenVertically();
  display.display();
  
//Monitor Serial
  Serial.print("Time: ");
  Serial.println(millis_str);
  Serial.println("Temp: " + (String)t+ "°C");
  Serial.println("Humidity: " + (String)h+ "%");
  Serial.println("---");

//Envia Segundos
  sprintf(millis_str, "%d", millis());
  MQTT.publish(MQTT_MILLIS_TOPIC, millis_str);

//Envia Temperatura
  sprintf(temp_str, "%s", (String)t);
  MQTT.publish(MQTT_DHT_TEMP_TOPIC, temp_str);
  
 //Envia Umidade
  sprintf(humd_str, "%s", (String)h);
  MQTT.publish(MQTT_DHT_HUMD_TOPIC, humd_str);

//Configuração de Setup e Delay
  setupWifi();
  setupMQTT();
  delay(1000);
}
