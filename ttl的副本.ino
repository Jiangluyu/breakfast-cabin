/*
 * The server will open the box or check status of the box
 * http://server_ip/openbox/1 will open the box No.1
 * server_ip is the IP address of the ESP8266 module, will
 * be printed to Serial when the module is connected 
 * 
 * author: Long-Jian Guo, Lu-Yu Jiang
 * date: 2019-03-24
 * function: control the breakfast box including check the
 * status and open the box ranging from one to twenty four
 */

#include "SoftwareSerial.h"
#include "crc16.h"
#include <stdlib.h>
#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID ""
#define STAPSK ""
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);//create an instance of the server
SoftwareSerial boxSerial(14, 12, false, 16);//软串口D5 D6

int boxStatus[24]; //柜子状态数组
char suffix[24] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 
'R', 'S', 'T', 'U', 'V', 'W', 'X'};
//柜子状态函数，调用后boxStatus[24]中即有24个柜的状态，0为无物品，1为有物品。
//void checkBox(){
//   unsigned char boxStatusHex[9];
//   unsigned char checkCmd[] = {0xAA,0x01,0x0A,0x00,0x01,0x02,0x03,0x98,0x7B};
//
//   for(char ch : checkCmd){
//     boxSerial.write(ch);
//   }
//  //软串口得到数据后开始覆盖16进制数组中的数据
//   //while(boxSerial.available() <= 0){}
//   
//   while(boxSerial.available() > 0){
//      for(int hexNum=0; hexNum < 9; hexNum++){
//        boxStatusHex[hexNum] = boxSerial.read();
//      }
//   }
//
//  //取代表柜子状态的三个字节转二进制并倒序
//    for (int i = 0; i < 8; i++)
//    {
//        boxStatus[i] = boxStatusHex[3] % 2;
//        boxStatusHex[3] /= 2;
//    }
//    
//    for (int i = 8; i < 16; i++)
//    {
//        boxStatus[i] = boxStatusHex[4] % 2;
//        boxStatusHex[4] /= 2;
//    }
//
//    for (int i = 16; i < 24; i++)
//    {
//        boxStatus[i] = boxStatusHex[5] % 2;
//        boxStatusHex[5] /= 2;
//    }
//
//
//    Serial.println(boxStatusHex[3],HEX);
//    Serial.println(boxStatusHex[4],HEX);
//    Serial.println(boxStatusHex[5],HEX);
//    Serial.println();
//
//    delay(1000);//强制delay，否则锁控板死机。
//}

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

void setup() {
  Serial.begin(9600);
  boxSerial.begin(9600);

  //connect WiFi
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.print(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));

  //start the server
  server.begin();
  Serial.println(F("Server started"));
  
  //print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  WiFiClient client = server.available();
  if(!client){
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000);

  //read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  //match the request
  if(req.indexOf(F("/openbox/A")) != -1){
    openBox(1);
  }else if(req.indexOf(F("/openbox/B")) != -1){
    openBox(2);
  }else if(req.indexOf(F("/openbox/C")) != -1){
    openBox(3);
  }else if(req.indexOf(F("/openbox/D")) != -1){
    openBox(4);
  }else if(req.indexOf(F("/openbox/E")) != -1){
    openBox(5);
  }else if(req.indexOf(F("/openbox/F")) != -1){
    openBox(6);
  }else if(req.indexOf(F("/openbox/G")) != -1){
    openBox(7);
  }else if(req.indexOf(F("/openbox/H")) != -1){
    openBox(8);
  }else if(req.indexOf(F("/openbox/I")) != -1){
    openBox(9);
  }else if(req.indexOf(F("/openbox/J")) != -1){
    openBox(10);
  }else if(req.indexOf(F("/openbox/K")) != -1){
    openBox(11);
  }else if(req.indexOf(F("/openbox/L")) != -1){
    openBox(12);
  }else if(req.indexOf(F("/openbox/M")) != -1){
    openBox(13);
  }else if(req.indexOf(F("/openbox/N")) != -1){
    openBox(14);
  }else if(req.indexOf(F("/openbox/O")) != -1){
    openBox(15);
  }else if(req.indexOf(F("/openbox/P")) != -1){
    openBox(16);
  }else if(req.indexOf(F("/openbox/Q")) != -1){
    openBox(17);
  }else if(req.indexOf(F("/openbox/R")) != -1){
    openBox(18);
  }else if(req.indexOf(F("/openbox/S")) != -1){
    openBox(19);
  }else if(req.indexOf(F("/openbox/T")) != -1){
    openBox(20);
  }else if(req.indexOf(F("/openbox/U")) != -1){
    openBox(21);
  }else if(req.indexOf(F("/openbox/V")) != -1){
    openBox(22);
  }else if(req.indexOf(F("/openbox/W")) != -1){
    openBox(23);
  }else if(req.indexOf(F("/openbox/X")) != -1){
    openBox(24);
  }else{
    Serial.println(F("invalid request"));
  }

  //checkBox();
  
  while(client.available()){
    client.read();
  }

  //send response to the client
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n"));
//  for(int i = 0; i < 24; i++){
//    client.print(F("box"));
//    client.print(i+1);
//    if(boxStatus[i] == 0){
//      client.println(F(": Empty<br>"));
//    }else{
//      client.println(F(": Full<br>"));
//    }
//  }
  
  for(int i = 0; i < 24; i++){
    client.print(F("<a href='http://"));
    client.print(WiFi.localIP());
    client.print(F("/openbox/"));
    client.print(suffix[i]);
    client.print(F("'>box"));
    client.print(i+1);
    client.print(F("</a><br></html>"));
  }
  
  Serial.println(F("Disconnecting from client"));
  delay(1000);
}
