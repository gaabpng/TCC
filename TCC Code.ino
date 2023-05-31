// Bibliotecas
#include "WiFi.h"
#include "PubSubClient.h"
#include "ESPmDNS.h"
#include "DHTesp.h"
#include "heltec.h"

// Net Setup
#define NET_SSID "LABProv"
#define NET_PASSWORD "labprov1"

// MQTT Setup
#define MQTT_ID "tcctermostato-esp32-0800313131"
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_MILLIS_TOPIC "tcc_termostato_millis"
#define MQTT_DHT_TEMP_TOPIC "tcc_termostato_temp"
#define MQTT_DHT_HUMD_TOPIC "tcc_termostato_humd"

// Cliente de rede
WiFiClient espClient;

// Cliente MQTT
PubSubClient MQTT(espClient);

// Variáveis que armazenam as informações que são enviadas ao broker
char millis_str[10] = "";
char temp_str[10] = "";
char humd_str[10] = "";

// Definição da Tela e Sensor DHT22
#define BAND 915E6 // Escolha a frequência
DHTesp dht;

// Variavéis do Sensor DHT22
float currentTemp;
float currentHumidity;

// Protótipo da função
void getTemp();
void getHumidity();
void sendPacket();

// Função de conexão Wi-Fi
void setupWifi()
{

    if (WiFi.status() == WL_CONNECTED)
        return;

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(NET_SSID);

    WiFi.begin(NET_SSID, NET_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// Função de conexão ao broker MQTT
void setupMQTT()
{

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

// Função que ler a temperatura e atibiu a variável currentTemp.
void getTemp()
{

    // Atribuindo a variavel float a função
    float temperature = dht.getTemperature();

    // Verifica se houve mudança na temperatura
    if (temperature != currentTemp)
    {
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

// Função que ler a Umidade e atibiu a variável currentHumidity.
void getHumidity()
{
    // Atribuindo a variavel float a função
    float humidity = dht.getHumidity();

    // Verifica se houve mudança na temperatura
    if (humidity != currentHumidity)
    {
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

// Função principal (setup)
void setup()
{

    // inicializa o LED
    pinMode(LED, OUTPUT);

    // Habilita diversas Configurações do Lora32
    Heltec.begin(true, true, true, true, BAND);
    /*(
      true = Habilita o Display,
      true = Heltec.Heltec.Heltec.LoRa Disable,
      true = Habilita debug Serial,
      true = Habilita o PABOOST,
      BAND = Frequência BAND.
    )*/

    // Primeira Tela: Exibe os criadores do projeto enquanto de conecta ao Wi-Fi e ao Broker MQTT
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

    // inicializa o DHT no pino 25
    dht.setup(25, DHTesp::DHT22);

    // Atribuindo as variaveis as funções
    currentTemp = dht.getTemperature();
    currentHumidity = dht.getHumidity();

    // Configuração de Conexões
    setupWifi();
    setupMQTT();
}

// Função de repetição (loop)
void loop()
{
    // Funções de Leitura de Temperatura e Umidade
    getTemp();
    getHumidity();

    // Segunda Tela: Exibe os valores de temperatura na tela do Lora32.
    Heltec.display->clear();
    Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
    Heltec.display->setFont(ArialMT_Plain_16);
    Heltec.display->drawString(30, 5, "Temperatura"); //"Enviando"
    Heltec.display->drawString(33, 30, (String)currentTemp);
    Heltec.display->drawString(78, 30, "°C");
    Heltec.display->display();

    // Monitor Serial
    Serial.print("Time: ");
    Serial.println(millis_str);
    Serial.println("Temp: " + (String)currentTemp + "°C");
    Serial.println("Humidity: " + (String)currentHumidity + "%");
    Serial.println("---");

    // Envia Segundos
    sprintf(millis_str, "%d", millis());
    MQTT.publish(MQTT_MILLIS_TOPIC, millis_str);

    // Envia Temperatura
    sprintf(temp_str, "%s", (String)currentTemp);
    MQTT.publish(MQTT_DHT_TEMP_TOPIC, temp_str);

    // Envia Umidade
    sprintf(humd_str, "%s", (String)currentHumidity);
    MQTT.publish(MQTT_DHT_HUMD_TOPIC, humd_str);

    // Configuração de Conexões e Configuração de Delay
    setupWifi();
    setupMQTT();
    delay(1000);
}