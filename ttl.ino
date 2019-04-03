/*
 * The server will open the box via MQTT
 * 
 * author: Long-Jian Guo, Lu-Yu Jiang
 * date: 2019-03-24
 * function: control the breakfast box including check the
 * status and open the box ranging from one to twenty four
 */

#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "crc16.h"

const char* ssid = "your wifi ssid";
const char* password = "your wifi password";

const char* mqttServer = "your MQTT server";
const int mqttPort = ;//your mqtt port
const char* topic = "your MQTT topic";

//initialize MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//define the softserial
SoftwareSerial boxSerial(12, 14, false, 16);//软串口D6 D5

int boxStatus[24];
char suffix[24] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 
'R', 'S', 'T', 'U', 'V', 'W', 'X'};

//传柜门号码，即可打开对应柜门
void openBox(int boxNum){
    unsigned char openCmd[9] = {0xAA, 0x01, 0x02, 0x01, 0x01, 0x02, 0x03};
    openCmd[3] = boxNum;
    USHORT crcStatus = usMBCRC16(openCmd, 7);
    openCmd[7] = crcStatus & 0xFF;
    openCmd[8] = crcStatus >> 8;
    for(char ch : openCmd){
      boxSerial.write(ch);
    }
    delay(1000);//强制delay，否则锁控板死机。
 }

void callback(char* topicTem, byte* payload, unsigned int msg_length){
  Serial.print("Message arrived [");
  Serial.print(topicTem);
  Serial.print("] ");
  for(int i = 0; i < msg_length; i++){
    Serial.print((char)payload[i]);
  }
  if(msg_length == 1){
    int lockerID = (int)(payload[0]-48);
    openBox(lockerID);
  }
  if(msg_length == 2){
    int lockerID = ((int)(payload[0]-48)) * 10 + (int)(payload[1]-48);
    openBox(lockerID);
  }else{
    Serial.print("wrong id");
  }
  Serial.println();
}

void reconnectMQTT(){
  while(!client.connected()){
    Serial.println("Connecting to MQTT...");
    if(client.connect("breakfast", NULL, NULL)){
      Serial.println("connected");
      client.subscribe(topic);
    }else{
      Serial.print("failed with state ");
      Serial.print(client.state());
      Serial.println("try again in 2 seconds");
      delay(2000);
    }
  }
}

void setupWifi(){
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.print(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  boxSerial.begin(9600);
  setupWifi();
  
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  client.publish(topic, "Hello from cabin");
}

void loop() {
  reconnectMQTT();
  client.loop();
  delay(1000);
}
