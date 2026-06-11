#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFiManager.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>


// 电机引脚
#define motor1_pin1 27
#define motor1_pin2 26
#define motor2_pin1 25
#define motor2_pin2 33

#define motor3_pin1 32
#define motor3_pin2 16
#define motor4_pin1 17
#define motor4_pin2 18

// 舵机引脚
Servo myservo1;
Servo myservo2;
Servo myservo3;

int pos = 0;

WebSocketsServer webSocket = WebSocketsServer(8686);
int carSpeed = 255;

// 初始化引脚
void initPin()
{
  pinMode(motor1_pin1, OUTPUT);
  pinMode(motor1_pin2, OUTPUT);
  pinMode(motor2_pin1, OUTPUT);
  pinMode(motor2_pin2, OUTPUT);
  pinMode(motor3_pin1, OUTPUT);
  pinMode(motor3_pin2, OUTPUT);
  pinMode(motor4_pin1, OUTPUT);
  pinMode(motor4_pin2, OUTPUT);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{

  switch (type)
  {
  case WStype_TEXT:
    String json = (char *)payload;
    JsonDocument doc;
    deserializeJson(doc, json);
    String dp = doc["dp"];

    if (dp == "servo")
    {
      int value = doc["value"];
      myservo1.write(value);
      myservo2.write(value);
      myservo3.write(value);
    }

    if (dp == "speed")
    {
      carSpeed = doc["value"];
    }

    if (dp == "direction")
    {
      String value = doc["value"];
      Serial.println(value);

      if (value == "forward")
      {
        digitalWrite(motor1_pin1, LOW);
        analogWrite(motor1_pin2, carSpeed);

        digitalWrite(motor2_pin1, LOW);
        analogWrite(motor2_pin2, carSpeed);

        analogWrite(motor3_pin1, carSpeed);
        digitalWrite(motor3_pin2, LOW);

        analogWrite(motor4_pin1, carSpeed);
        digitalWrite(motor4_pin2, LOW);
      }

      if (value == "backward")
      {
        analogWrite(motor1_pin1, carSpeed);
        digitalWrite(motor1_pin2, LOW);

        analogWrite(motor2_pin2, carSpeed);
        digitalWrite(motor2_pin2, LOW);

        digitalWrite(motor3_pin1, LOW);
        analogWrite(motor3_pin1, carSpeed);

        digitalWrite(motor4_pin1, LOW);
        analogWrite(motor4_pin1, carSpeed);
      }

      if (value == "left")
      {
        analogWrite(motor1_pin1, carSpeed);
        digitalWrite(motor1_pin2, LOW);

        digitalWrite(motor2_pin1, LOW);
        analogWrite(motor2_pin2, carSpeed);

        digitalWrite(motor3_pin1, carSpeed);
        digitalWrite(motor3_pin2, LOW);

        digitalWrite(motor4_pin1, LOW);
        analogWrite(motor4_pin2, carSpeed);
      }

      if (value == "right")
      {
        digitalWrite(motor1_pin1, LOW);
        analogWrite(motor1_pin2, carSpeed);

        analogWrite(motor2_pin1, carSpeed);
        digitalWrite(motor2_pin2, LOW);

        digitalWrite(motor3_pin1, LOW);
        analogWrite(motor3_pin2, carSpeed);

        analogWrite(motor4_pin1, carSpeed);
        digitalWrite(motor4_pin2, LOW);
      }

      if (value == "counterclockwise")
      {
        analogWrite(motor1_pin1, carSpeed);
        digitalWrite(motor1_pin2, LOW);

        digitalWrite(motor2_pin1, LOW);
        analogWrite(motor2_pin2, carSpeed);

        digitalWrite(motor3_pin1, LOW);
        analogWrite(motor3_pin2, carSpeed);

        analogWrite(motor4_pin1, carSpeed);
        digitalWrite(motor4_pin2, LOW);
      }

      if (value == "clockwise")
      {
        digitalWrite(motor1_pin1, LOW);
        analogWrite(motor1_pin2, carSpeed);

        analogWrite(motor2_pin1, carSpeed);
        digitalWrite(motor2_pin2, LOW);

        analogWrite(motor3_pin1, carSpeed);
        digitalWrite(motor3_pin2, LOW);

        digitalWrite(motor4_pin1, LOW);
        analogWrite(motor4_pin2, carSpeed);
      }

      if (value == "forwardLeft")
      {
        digitalWrite(motor2_pin1, LOW);
        analogWrite(motor2_pin2, carSpeed);

        analogWrite(motor3_pin1, carSpeed);
        digitalWrite(motor3_pin2, LOW);
      }

      if (value == "forwardRight")
      {
        digitalWrite(motor1_pin1, LOW);
        analogWrite(motor1_pin2, carSpeed);

        analogWrite(motor4_pin1, carSpeed);
        digitalWrite(motor4_pin2, LOW);
      }

      if (value == "backwardLeft")
      {
        analogWrite(motor1_pin1, carSpeed);
        digitalWrite(motor1_pin2, LOW);

        digitalWrite(motor4_pin1, LOW);
        analogWrite(motor4_pin2, carSpeed);
      }

      if (value == "backwardRight")
      {
        analogWrite(motor2_pin1, carSpeed);
        digitalWrite(motor2_pin2, LOW);

        digitalWrite(motor3_pin1, LOW);
        analogWrite(motor3_pin2, carSpeed);
      }

      if (value == "stop")
      {
        initPin();
        digitalWrite(motor1_pin1, LOW);
        digitalWrite(motor1_pin2, LOW);

        digitalWrite(motor2_pin1, LOW);
        digitalWrite(motor2_pin2, LOW);

        digitalWrite(motor3_pin1, LOW);
        digitalWrite(motor3_pin2, LOW);

        digitalWrite(motor4_pin1, LOW);
        digitalWrite(motor4_pin2, LOW);
      }
    }

    break;
  }
}

uint32_t getChipId()
{
  uint32_t chipId = 0;
  for (int i = 0; i < 17; i = i + 8)
  {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }

  return chipId;
}

void setup()
{
  Serial.begin(115200);

  initPin();

  myservo1.attach(13);
  myservo2.attach(2);
  myservo3.attach(4);

  // 获取芯片ID并生成配网热点名称
  uint32_t chipId = getChipId();
  char apName[50];
  sprintf(apName, "%s%u", "电机驱动板热点-", chipId);

  WiFiManager wm;
  bool res;
  res = wm.autoConnect(apName); // WiFi配网热点

  webSocket.begin();                 // 启动WebSocket服务器
  webSocket.onEvent(webSocketEvent); // 设置WebSocket事件处理函数

  String ipString = WiFi.localIP().toString();
  Serial.println("手机连接地址: ws://" + ipString + ":8686");
}

void loop()
{

  webSocket.loop(); // 处理WebSocket事件
}