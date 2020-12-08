#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define LED6_PIN D5
#define DSEN1_PIN D1
#define DSEN2_PIN A0
String dsen1;
String dsen2;
int dsenc1;
int dsenc2;

// Update these with values suitable for your network.
const char* ssid = "Kaustav";
const char* password = "k3t15841";
const char* mqtt_server = "test.mosquitto.org";
//const char* mqtt_server = "iot.eclipse.org";
//const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(100);
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "air";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    //if you MQTT broker has clientID,username and password
    //please change following line to    if (client.connect(clientId,userName,passWord))
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      //once connected to MQTT broker, subscribe command if any
      client.subscribe("project/test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
} //end reconnect()

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  pinMode(DSEN1_PIN, INPUT);
  pinMode(DSEN2_PIN, INPUT);
  pinMode(LED6_PIN, OUTPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    dsen1 = digitalRead(DSEN1_PIN);
    dsenc1 = digitalRead(DSEN1_PIN);
    dsen2 = analogRead(DSEN2_PIN);
    dsenc2 = analogRead(DSEN2_PIN);
    char message1[4];
    char message2[4];;
    dsen1.toCharArray(message1, 4);
    dsen2.toCharArray(message2, 4);
    client.publish("airq", message2);
    Serial.print("Smoke=");
    Serial.println(dsen1);
    Serial.print("Airquality=");
    Serial.println(dsen2);
    Serial.println("");
    if (dsenc1 == 0)
    {
      Serial.println("SMOKE HIGH");
      digitalWrite(LED6_PIN, HIGH);
      client.publish("sen2", "SMOKE HIGH");
      delay(500);
    }
    if (dsenc1 == 1)
    {
      Serial.println("LOW/NO SMOKE");
      digitalWrite(LED6_PIN, LOW);
      client.publish("sen2", "LOW/NO SMOKE");
      delay(500);
    }
    if (dsenc2 >= 400)
    {
      Serial.println("POOR AIR QUALITY");
      digitalWrite(LED6_PIN, HIGH);
      client.publish("sen3", "POOR AIR QUALITY");
      delay(500);
    }
    if ((dsenc2 >= 300)&&(dsenc2 <= 400))
    {
      Serial.println("AIR QUALITY NORMAL");
      digitalWrite(LED6_PIN, LOW);
      client.publish("sen3", "AIR QUALITY NORMAL");
      delay(500);
    }
    if (dsenc2 < 300)
    {
      Serial.println("GOOD AIR QUALITY");
      digitalWrite(LED6_PIN, LOW);
      client.publish("sen3", "GOOD AIR QUALITY");
      delay(500);
    }
  }
}
