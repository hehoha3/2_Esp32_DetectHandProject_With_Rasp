#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid        = "pepu";
const char* password    = "88888888";

const char* mqtt_server = "left01.local";

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

int light_led = 18;
int door_led  = 19;

String lightStatus = "OFF";
String doorStatus  = "CLOSE";

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;

void setup_wifi() {
  delay(50);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int c=0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); //
    Serial.print(".");
    c=c+1;
    if(c>10){
        ESP.restart(); //restart ESP after 10 seconds
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void connect_mqttServer() {
  // Loop until we're reconnected
  while (!client.connected()) {

        //first check if connected to wifi
        if(WiFi.status() != WL_CONNECTED){
          //if not connected, then first connect to wifi
          setup_wifi();
        }

        //now attemt to connect to MQTT server
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32_client01")) { // Change the name of client here if multiple ESP32 are connected
          //attempt successful
          Serial.println("connected");
          // Subscribe to topics here
          client.subscribe("home/lightControl");
          client.subscribe("home/doorControl");
        } else {
          //attempt not successful
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" trying again in 2 seconds");
    
          // Wait 2 seconds before retrying
          delay(2000);
        }
  }
  
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String msg;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    msg += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "home/lightControl") {
    if(msg == "Light_ON"){
      digitalWrite(light_led, HIGH);
      lightStatus = "ON";
      lcd.clear();

      // Light
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.setCursor(7, 0);
      lcd.print(lightStatus);

      // Door
      lcd.setCursor(0, 1);
      lcd.print("Door: ");
      lcd.setCursor(7, 1);
      lcd.print(doorStatus);

      Serial.println("Light is now ON !!");
    } else if (msg == "Light_OFF") {
      digitalWrite(light_led, LOW);
      lightStatus = "OFF";
      lcd.clear();

      // Light
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.setCursor(7, 0);
      lcd.print(lightStatus);

      // Door
      lcd.setCursor(0, 1);
      lcd.print("Door: ");
      lcd.setCursor(7, 1);
      lcd.print(doorStatus);

      Serial.println("Light is now OFF !!");
    }
  } else if (String(topic) == "home/doorControl") {
    if (msg == "Door_OPEN") {
      digitalWrite(door_led, HIGH);
      doorStatus = "OPEN";
      lcd.clear();

      // Light
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.setCursor(7, 0);
      lcd.print(lightStatus);

      // Door
      lcd.setCursor(0, 1);
      lcd.print("Door: ");
      lcd.setCursor(7, 1);
      lcd.print(doorStatus);

      Serial.println("Door is now OPEN !!");
    } else if (msg == "Door_CLOSE") {
      digitalWrite(door_led, LOW);
      doorStatus = "CLOSE";
      lcd.clear();

      // Light
      lcd.setCursor(0, 0);
      lcd.print("Light: ");
      lcd.setCursor(7, 0);
      lcd.print(lightStatus);

      // Door
      lcd.setCursor(0, 1);
      lcd.print("Door: ");
      lcd.setCursor(7, 1);
      lcd.print(doorStatus);

      Serial.println("Door is now CLOSE !!");
    }
  }
}

void setup(){
  pinMode(light_led, OUTPUT);
  pinMode(door_led, OUTPUT);

  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server,1883);//1883 is the default port for MQTT server
  client.setCallback(callback);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();
  // set cursor to first column, first row

  lcd.setCursor(0, 0);
  lcd.print("Light: ");
  lcd.setCursor(7, 0);
  lcd.print(lightStatus);

  // Door
  lcd.setCursor(0, 1);
  lcd.print("Door : ");
  lcd.setCursor(7, 1);
  lcd.print(doorStatus);
}

void loop(){
  if (!client.connected()) {
    connect_mqttServer();
  }

  client.loop();
}