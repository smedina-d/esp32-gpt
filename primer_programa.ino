#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#include <Wire.h>
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_WIDTH 128 // OLED display width, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid = "Archer24";
const char* password = "laclavenolase";
const char* chatgpt_token = "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";
String res = "";
void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();


  while (!Serial);

  // wait for WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  pinMode(2, OUTPUT);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  // Display static text
  
}

void loop()
{
    // Scroll in various directions, pausing in-between:  
  Serial.print("Ask your Question : ");
  while (!Serial.available());
  while (Serial.available())
  {
    char add = Serial.read();
    res = res + add;
    delay(1);
  }
  int len = res.length();
  res = res.substring(0, (len - 1));
  res = "\"" + res + "\"";
  Serial.println(res);

  HTTPClient https;

  //Serial.print("[HTTPS] begin...\n");
  if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS

    https.addHeader("Content-Type", "application/json");
    String token_key = String("Bearer ") + chatgpt_token;
    https.addHeader("Authorization", token_key);

    String payload = String("{\"model\": \"text-davinci-003\", \"prompt\": ") + res + String(", \"temperature\": 0, \"max_tokens\": 100}"); //Instead of TEXT as Payload, can be JSON as Paylaod

    //Serial.print("[HTTPS] GET...\n");

    // start connection and send HTTP header
    int httpCode = https.POST(payload);

    // httpCode will be negative on error
    // file found at server
    if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
      String payload = https.getString();
      //Serial.println(payload);

      DynamicJsonDocument doc(1024);


      deserializeJson(doc, payload);
      String Answer = doc["choices"][0]["text"];
      Answer = Answer.substring(2);
      digitalWrite(2,HIGH);

      display.clearDisplay();
      Serial.print("Answer : "); 
      display.print(Answer);
      display.display(); 
      display.startscrollleft(0x00, 0x00);
      Serial.println(Answer);

    }
    else {
      Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
    }
    https.end();
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
  }
  delay(5000);
  digitalWrite(2, LOW);
  Serial.println("Wait 10s before next round...");
  res = "";
  delay(10000);
  display.clearDisplay();

}