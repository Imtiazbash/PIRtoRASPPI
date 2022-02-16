

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "SSID";
const char* password = "PASSWORD";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "IP";
const int mqttPort = 1883;

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

// DHT Sensor
const int PIRpin = 14;

// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP8266Client2","username","password")) {
      Serial.println("connected");  
      client.subscribe("esp8266/4");
      client.subscribe("esp8266/5");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(15000);
    }
  }
}

void setup() {
  
  Serial.begin(115200);
   Serial.print("Setting soft-AP ... ");
  /*boolean result = WiFi.softAP("ESPsoftAP_01", "1234");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }*/
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  
 if (!client.connected()) {
  Serial.printf("Stations connected = %d\n",WiFi.softAPgetStationNum());
  delay(3000);
    reconnect();
  }
  if(!client.loop())

    client.connect("ESP8266Client");
    
  now = millis();
  // Publishes new temperature and humidity every 10 seconds
  if (now - lastMeasure > 10000) {
    lastMeasure = now;
    float p = digitalRead(PIRpin);
   

    // Check if any reads failed and exit early (to try again).
    if (isnan(p)) {
      Serial.println("Failed to read from PIR sensor!");
      return;
    }

    // Computes temperature values in Celsius
    static char temperatureTemp[7];
    dtostrf(p, 6, 2, temperatureTemp);
    

    // Publishes PIR values
    client.publish("/esp8266/temperature", temperatureTemp);
 
    
  
    Serial.print("\p PIR: ");
    Serial.print(p);
  }
}
