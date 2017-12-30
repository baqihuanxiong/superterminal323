/*
 * 2017/12/30 by slw
 * 可通过上位机直接指令行走或根据上位机所给的区段指令集连续行走
 * 支持前后标签控制
 * 问题:
 * (1)wifi成功连接后首条指令时而接收不到
 * (2)舵机受干扰严重
 * MFRC522 typical pin layout:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <MFRC522.h>
#include<Servo.h>
#include<SPI.h>
#include<Wire.h>
#include<SoftwareSerial.h>
#include<Metro.h>


SoftwareSerial ESP8266(3,4); // 软件串口 RX 3 TX 4

const int RST_PIN_F = 9;
const int SS_PIN_F = 10;
MFRC522 rfid_f(SS_PIN_F, RST_PIN_F); // 前置RFID感应模块
byte nuidPICC_F[4]; // 存储前卡识别到的RFID标签id

const int RST_PIN_B = 2;
const int SS_PIN_B = A1;
MFRC522 rfid_b(SS_PIN_B, RST_PIN_B); // 后置RFID感应模块
byte nuidPICC_B[4];

byte rfidMap[6][4] = {
  {48,182,131,190},
  {208,175,140,190},
  {32,126,119,44},
  {16,02,189,121},
  {84,49,104,185},
  {125,00,189,121}
  }; // 测试用标签表

const int EN = 6; // 电机使能，接L298N ENA与ENB
const int IN1 = 7; // 接L298N IN1与IN3
const int IN2 = 8; // 接L298N IN2与IN4

Servo myservo; // 转向舵机
const int init_pos = 85;

bool FAIL_8266 = false; // wifi连接状态


void setup() {
  Serial.begin(9600); 
  ESP8266.begin(9600);
  SPI.begin();
  rfid_f.PCD_Init();
  rfid_b.PCD_Init();
  myservo.attach(5);
  pinMode(EN,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(A0,INPUT);
  pinMode(A1,OUTPUT);
  
  ESP8266.println("AT+RST");
  Serial.println("Reset ESP8266...");
  delay(1000);
  if (ESP8266.find("OK")) {
    Serial.println("ESP8266 is ready");
    connectWiFi();
  }
  else {
    Serial.println("Load ESP8266 fail");
  }
  
  Serial.println("Runing...");
}

bool connectWiFi() { // find OK 存在问题，即使连接成功也无法找到OK
  ESP8266.println("AT+CWJAP=\"Xiaomi_agv\",\"xiaomiagv\"");
  Serial.println("Connecting AP...");
  delay(6000);
  ESP8266.println("AT+CIPSTART=\"TCP\",\"192.168.31.164\",5005");
  Serial.println("Connecting AGVServer...");
  delay(4000);
  ESP8266.println("AT+CIPSEND");
  delay(1000);
  ESP8266.println("This is AGV0");
  delay(1000);
  flushESP8266();
  return true;
}

void flushESP8266() {
  while(ESP8266.read()>=0){}
}

// 电机运转， pwm为0~255的值，dir为 'f' 正转，dir为 'b' 反转
void go(int pwm,char dir) {
  analogWrite(EN, pwm);
  if (dir=='f') {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
  }
  else if (dir=='b') {
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
  }
}

// 停止动力输出
void brake() { 
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(EN,LOW);
}

// 急停
void brake_immediate() {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(EN,LOW); 
}

// 转向
void turn(int pos) {
  if (pos > 60) {
    pos = 60;
  }
  else if (pos < -60) {
    pos = -60;
  }
  myservo.write(init_pos + pos);
}

// 前卡读取，若标签已读取过则不执行
bool scanRFID_F() {
  if (rfid_f.PICC_IsNewCardPresent() && rfid_f.PICC_ReadCardSerial()) {
    if (rfid_f.uid.uidByte[0] != nuidPICC_F[0] || 
      rfid_f.uid.uidByte[1] != nuidPICC_F[1] || 
      rfid_f.uid.uidByte[2] != nuidPICC_F[2] || 
      rfid_f.uid.uidByte[3] != nuidPICC_F[3] ) {
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        nuidPICC_F[i] = rfid_f.uid.uidByte[i];
      }
      return true;
    }
    rfid_f.PICC_HaltA();
  }
  return false;
}

// 后卡读取，若标签已读取过则不执行
bool scanRFID_B() {
  if (rfid_b.PICC_IsNewCardPresent() && rfid_b.PICC_ReadCardSerial()) {
    if (rfid_b.uid.uidByte[0] != nuidPICC_B[0] || 
      rfid_b.uid.uidByte[1] != nuidPICC_B[1] || 
      rfid_b.uid.uidByte[2] != nuidPICC_B[2] || 
      rfid_b.uid.uidByte[3] != nuidPICC_B[3] ) {
      // Store NUID into nuidPICC array
      for (byte i = 0; i < 4; i++) {
        nuidPICC_B[i] = rfid_b.uid.uidByte[i];
      }
      return true;
    }
    rfid_b.PICC_HaltA();
  }
  return false;
}

void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
  Serial.println();
}

void printStr(String *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void ESPprintDec(byte *buffer, byte bufferSize, String custom) {
  ESP8266.print(custom);
  for (byte i = 0; i < bufferSize; i++) {
    ESP8266.print(buffer[i] < 0x10 ? " 0" : " ");
    ESP8266.print(buffer[i], DEC);
  }
  ESP8266.println();
}

String esp_rec = ""; // ESP8266串口缓存
bool newLineReceived = false;

// 读取ESP8266串口缓存
void readESP8266() {
  while (ESP8266.available() > 0) {
    esp_rec += (char)ESP8266.read();
    newLineReceived = true;
    delay(2); // 不添加延迟会造成无法一次读取全部串口缓存
  }
}

String cmd = "f000r000"; // dir('f'/'b'),pwm(0~255),turn('r'/'l'),pos(-60~60)

// 识别上位机控制指令
bool cmdReceive() {
  if (newLineReceived&&
  (esp_rec[0]=='f'||esp_rec[0]=='b')&&
  (esp_rec.substring(1,4).toInt()>=0&&esp_rec.substring(1,4).toInt()<=255)&&
  (esp_rec[4]=='l'||esp_rec[4]=='r')&&
  (esp_rec.substring(5).toInt()>=-60&&esp_rec.substring(5).toInt()<=60)) {
    cmd = esp_rec;
    esp_rec = "";
    newLineReceived = false;
    return true;
  }
  else {
    return false;
  }
}

// 识别上位机区段指令
byte rfids[6] = {0,0,0,0,0,0};
String cmd_f[6] = {"","","","","",""};
String cmd_b[6] = {"","","","","",""};
bool zoneReceive() {
  if (newLineReceived&&esp_rec.startsWith("rfids")) {
    byte numRfid = (esp_rec.length()-5)/20;
    for(byte i=0;i<numRfid;i++) {
      rfids[i] = esp_rec.substring(6+20*i,7+20*i).toInt();
      cmd_f[i] = esp_rec.substring(8+i*20,16+i*20);
      cmd_b[i] = esp_rec.substring(17+i*20,25+i*20);
    }
    esp_rec = "";
    newLineReceived = false;
    return true;
  }
  else {
    return false;
  }
}

// 通过标签(buffer)查找对应指令
String getZoneCmd(byte *buffer,char card) {
  int rfidIndex = 0;
  for (int i=0;i<sizeof(rfidMap)/4;i++) {
    if (buffer[0]==rfidMap[i][0]&&buffer[1]==rfidMap[i][1]&&buffer[2]==rfidMap[i][2]&&buffer[3]==rfidMap[i][3]) {
      rfidIndex = i+1;
    }
  }
  byte cmdIndex = -1;
  if (rfidIndex>0) {
    for (byte i=0;i<6;i++) {
      if (rfids[i]==rfidIndex) {
        cmdIndex = i;
      }
    }
  }
  String theCmd = "pass";
  if (cmdIndex>-1) {
    if (card=='f') {
      theCmd = cmd_f[cmdIndex];
    }
    else {
      theCmd = cmd_b[cmdIndex];
    }
  }
  if (theCmd=="00000000") {
    theCmd = "pass";
  }
  return theCmd;
}

bool safe_f = true; // 前方无障碍
bool safe_b = true; // 后方无障碍

void runByCmd(String cmd) {
  char dir = cmd[0];
  int pwm = cmd.substring(1,4).toInt();
  int pos = cmd.substring(5).toInt();
  if (safe_f&&dir=='f') {
    go(pwm,dir);
    turn(pos);
  }
  else if (safe_b&&dir=='b') {
    go(pwm,dir);
    turn(pos);
  }
  else if (pwm==0) {
    brake_immediate();
  }
  else{
    brake();
  }
}


void loop() {
  readESP8266();
  if (newLineReceived) {
    Serial.println("esp_rec:"+esp_rec);
  }
  
  if (cmdReceive()) {
    Serial.println("cmd:"+cmd);
    runByCmd(cmd);
  }

  if (zoneReceive()) {
    Serial.print("rfids:");
    printDec(rfids,6);
    Serial.print("cmd_f:");
    printStr(cmd_f,6);
    Serial.print("cmd_b:");
    printStr(cmd_f,6);
  }
  
  if (scanRFID_F()) {
    cmd = getZoneCmd(nuidPICC_F,'f');
    if (cmd!="pass") {
      runByCmd(cmd);
    }
    Serial.print("Scaned Front Card:");
    printDec(nuidPICC_F,4);
    ESPprintDec(nuidPICC_F,4,"Front:");
  }
  if (scanRFID_B()) {
    cmd = getZoneCmd(nuidPICC_B,'b');
    if (cmd!="pass") {
      runByCmd(cmd);
    }
    Serial.print("Scaned Back Card:");
    printDec(nuidPICC_B,4);
    ESPprintDec(nuidPICC_B,4,"Back");
  }

//  if (volMetro.check()) {
//    float vol = analogRead(A0)/45.0;
//    Serial.println(vol);
//  }

  
//  int rs = digitalRead(A2);
//  int rc = digitalRead(A3);
//  int lc = digitalRead(A4);
//  int ls = digitalRead(A5);
//  String data = String(rs)+','+String(rc)+','+String(lc)+','+String(ls);
//  Serial.println(data);
  
//  int dist_f = analogRead(A6);
//  int dist_b = analogRead(A7);
//  Serial.print(dist_f);
//  Serial.print(',');
//  Serial.println(dist_b);
//  if (dist_f<300) {
//    safe_f = false;
//  }
//  else {
//    safe_f = true;
//  }
//  if (dist_b<300) {
//    safe_b = false;
//  }
//  else {
//    safe_b = true;
//  }

  newLineReceived = false;
  esp_rec = "";
}
