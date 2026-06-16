#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// Configurações do Sensor DHT22
#define DHTPIN 4      // Pino onde o DHT22 está conectado
#define DHTTYPE DHT22 // Tipo do sensor
DHT dht(DHTPIN, DHTTYPE);

// Configurações Wi-Fi (Wokwi padrão)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurações MQTT
const char* mqtt_server = "test.mosquitto.org"; // Broker público para testes
const char* topic_alert = "satelite/cubesat/alerta";
const char* topic_telemetry = "satelite/cubesat/telemetria";

WiFiClient espClient;
PubSubClient client(espClient);

// Variáveis para a lógica do "Modelo" TinyML
float temperaturaAnterior = 0.0;
unsigned long ultimoTempo = 0;
const long intervalo = 2000; // Lê a cada 2 segundos

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando na rede WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    // ID do cliente aleatório
    String clientId = "CubeSatClient-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado ao Broker MQTT!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  
  // Inicializa leitura
  temperaturaAnterior = dht.readTemperature();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long tempoAtual = millis();
  if (tempoAtual - ultimoTempo >= intervalo) {
    ultimoTempo = tempoAtual;

    float temperaturaAtual = dht.readTemperature();

    if (isnan(temperaturaAtual)) {
      Serial.println("Falha na leitura do DHT!");
      return;
    }

    // ====================================================================
    // LÓGICA DO MODELO (TINYML EXTRAÍDO DO EDGE IMPULSE PARA IF/ELSE)
    // No Edge Impulse identificamos que:
    // 1. Temp acima de 45C é superaquecimento crítico.
    // 2. Variação brusca (Delta T > 5C em 2s) indica falha de hardware.
    // ====================================================================
    
    float deltaT = abs(temperaturaAtual - temperaturaAnterior);
    String estado = "NOMINAL";
    
    if (temperaturaAtual > 45.0 || deltaT > 5.0) {
      estado = "FALHA_IMINENTE";
      
      // Monta a mensagem de emergência
      String msgAlerta = "{\"alerta\":\"FALHA DETECTADA\", \"temp_atual\": " + String(temperaturaAtual) + ", \"delta_t\": " + String(deltaT) + "}";
      
      // Envia imediatamente via MQTT
      client.publish(topic_alert, msgAlerta.c_str());
      Serial.println(">>> ALERTA DE EMERGÊNCIA ENVIADO VIA MQTT <<<");
    }

    // Log e Telemetria padrão
    Serial.print("Temp: ");
    Serial.print(temperaturaAtual);
    Serial.print(" °C | DeltaT: ");
    Serial.print(deltaT);
    Serial.print(" | Estado: ");
    Serial.println(estado);

    // Atualiza a variável para o próximo ciclo
    temperaturaAnterior = temperaturaAtual;
  }
}