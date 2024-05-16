#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

// Variables del sensor DHT11
#define DHTTYPE DHT11
#define DHTPIN 14
DHT dht(DHTPIN, DHTTYPE, 27);
float temperatura;
float humedad;

// Variables de conexión WiFi
const char* ssid = "SmartLab";
const char* password = "D5cy6$3y)5&$wT$7ry";

// Variables de conexión MQTT
const char* mqtt_server = "192.168.0.18"; // Dirección IP del servidor MQTT
const char* mqtt_topic_temp = "temperaturaH";
const char* mqtt_topic_hume = "humedadH";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Dirección IP:");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Manejar los mensajes entrantes (si es necesario)
}

void reconnect() {
  // Loop hasta que estemos reconectados
  while (!client.connected()) {
    Serial.print("Intentando conexión MQTT...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
    } else {
      // Depurar: Imprimir el estado de la conexión MQTT
      Serial.print("Falló, rc=");
      Serial.print(client.state());
      Serial.println(" Intentando de nuevo en 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); // Inicializar el puerto serie para la depuración
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leer datos del sensor DHT11
  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();

  // Depurar: Imprimir los datos del sensor en el puerto serie
  Serial.print("Temperatura: ");
  Serial.println(temperatura);
  Serial.print("Humedad: ");
  Serial.println(humedad);

  // Publicar datos de temperatura en el servidor MQTT
  String tempString = String(temperatura);
  String tempPayload = "{"+ tempString + "}";
  Serial.print("Publicando mensaje de temperatura: ");
  Serial.println(tempPayload);
  client.publish(mqtt_topic_temp, tempPayload.c_str());

  // Publicar datos de humedad en el servidor MQTT
  String humString = String(humedad);
  String humPayload = "{" + humString + "}";
  Serial.print("Publicando mensaje de humedad: ");
  Serial.println(humPayload);
  client.publish(mqtt_topic_hume, humPayload.c_str());

  delay(5000); // Esperar 5 segundos antes de enviar el siguiente dato
}

