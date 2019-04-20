/*
 * The server will open the cabinet via MQTT
 * 
 * author: Long-Jian Guo, Lu-Yu Jiang
 * date: 2019-03-24
 * function: open the cabinet ranging from one to twenty four
 * 
 * update No.1
 * update: 2019-04-20
 * update log: add callback towards request via json
 */

#include <SoftwareSerial.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc16.h"
#include "cJSON.h"

const char* ssid = "your ssid";
const char* password = "your password";

const char* mqttServer = "your mqtt server";
const int mqttPort = 1883;
const char* topic = "your mqtt topic";

//initialize MQTT
WiFiClient espClient;
PubSubClient client(espClient);

//define the softserial
SoftwareSerial boxSerial(12, 14, false, 16);//SoftwareSerial D6 D5

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
  //byte* msg[50] = "{\"orderID\":\"204\",\"cabinetID\":\"14\"}";
  Serial.println();
  
  cJSON* cjson = cJSON_Parse((char*)payload);
  if(cjson == NULL){
      Serial.println("Json pack into cjson error.");
  }else{
      cJSON_Print(cjson);
  }

  int orderID = cJSON_GetObjectItem(cjson, "orderID")->valueint;
  int cabinetID = cJSON_GetObjectItem(cjson, "cabinetID")->valueint;

  Serial.print("orderID: ");
  Serial.println(orderID);
  
  Serial.print("cabinetID: ");
  Serial.println(cabinetID);
//  int len = strlen(cabinetID);
//  int cabinetNum;
//  if(len == 1){
//      cabinetNum = (int)cabinetID[0] - 48;
//      Serial.println(cabinetNum);
//  }else if(len == 2){
//      cabinetNum = ((int)cabinetID[0] - 48) * 10 + (int)cabinetID[1] - 48;
//      Serial.println(cabinetNum);
//  }
  
  openBox(cabinetID);

  char orderIDTemp[5];
  itoa(orderID, orderIDTemp, 10);
  
  char ans[20] = "";
  strcat(ans, "{\"orderID:\",");
  strcat(ans, orderIDTemp);
  strcat(ans, "}");
  
  if(client.publish("success", ans) == true){
    Serial.println("Success sending message.");
  }else{
    Serial.println("Error sending message.");
  }
  
  cJSON_Delete(cjson);
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
