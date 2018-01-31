/*
 * 2018/1/18 by slw
 * 增加pid控制
 * 支持前后标签控制
 * 问题:
 * (1)wifi成功连接后首条指令时而接收不到
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
#include<MsTimer2.h>
#include<PID_v1.h>


const int RST_PIN_F = 46;
const int SS_PIN_F = 49;
MFRC522 rfid_f(SS_PIN_F, RST_PIN_F); // 前置RFID感应模块
byte nuidPICC_F[4]; // 存储前卡识别到的RFID标签id

const int RST_PIN_B = 48;
const int SS_PIN_B = 47;
MFRC522 rfid_b(SS_PIN_B, RST_PIN_B); // 后置RFID感应模块
byte nuidPICC_B[4];

byte rfidMap[32][4] = {
 {32,79,104,115},  //1
 {48,82,101,115},  //2
 {208,136,133,115},//3
 {96,77,117,115},  //4
 {00,135,145,115}, //5
 {192,89,111,115}, //6
 {80,243,101,115}, //7
 {128,169,114,115},//8
 {00,181,109,115}, //9
 {64,220,121,115}, //10
 {64,134,115,115}, //11
 {64,230,103,115}, //12
 {112,105,95,115}, //13
 {144,191,134,115},//14
 {128,35,142,115}, //15
 {48,69,107,115},  //16
 {208,181,105,115},//17
 {16,156,114,115}, //18
 {240,163,101,115},//19
 {16,53,106,115},  //20
 {48,180,108,115}, //21
 {208,175,140,190},//22
 {160,74,144,115}, //23
 {160,19,120,115}, //24
 {32,126,119,44},  //25
 {224,18,140,115}, //26
 {64,19,115,115},  //27
 {00,78,109,115},  //28
 {128,70,118,115}, //29
 {176,129,110,115},//30
 {48,182,131,190}, //31
 {208,90,126,115}  //32
}; // 测试用标签表

const int ENA = 8; // 电机使能，接L298N ENA
const int IN1 = 9; // 接L298N IN1
const int IN2 = 10; // 接L298N IN2
const int ENB = 12; // 电机使能，接L298N ENB
const int IN3 = 7; // 接L298N IN3
const int IN4 = 11; // 接L298N IN4

double pwmSet_l,speed_l,pwmOut_l;
double pwmSet_r,speed_r,pwmOut_r;
double posSet,posErro,posOut;
PID pid_l(&speed_l,&pwmOut_l,&pwmSet_l,0.55,0.35,0,P_ON_E,DIRECT);
PID pid_r(&speed_r,&pwmOut_r,&pwmSet_r,0.55,0.35,0,P_ON_E,DIRECT);
PID pid_p(&posErro,&posOut,&posSet,0.85,0,0,DIRECT);

Servo myservo; // 转向舵机
const int init_pos = 85;

const int CLK = 22;
const int SI = 24;
bool PixelArray[128];
bool whiteLineCovered = false;

bool FAIL_8266 = false; // wifi连接状态


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  SPI.begin();
  rfid_f.PCD_Init();
  rfid_b.PCD_Init();
  myservo.attach(5);
  pinMode(ENA,OUTPUT);
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(ENB,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(CLK,OUTPUT);
  pinMode(SI,OUTPUT);
  pinMode(30,INPUT);
  pinMode(31,INPUT);
  pinMode(32,INPUT);
  pinMode(33,INPUT);
  
  digitalWrite(SI, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);
  for(byte i=0;i<128;i++) {
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
  
  attachInterrupt(0,counter_l,CHANGE);
  attachInterrupt(1,counter_r,CHANGE);
  
  MsTimer2::set(150,getSpeed);
  MsTimer2::start();
  pid_l.SetMode(AUTOMATIC);
  pid_r.SetMode(AUTOMATIC);
  pid_p.SetMode(AUTOMATIC);
  pid_l.SetSampleTime(150);
  pid_r.SetSampleTime(150);
  pid_p.SetSampleTime(150);
  pid_p.SetOutputLimits(-20,20);
  posSet = 0;
  
  Serial1.println("AT+RST");
  Serial.println("Reset ESP8266...");
  delay(1000);
  if (Serial1.find("OK")) {
    Serial.println("ESP8266 is ready");
    connectWiFi();
  }
  else {
    Serial.println("Load ESP8266 fail");
  }
  
  Serial.println("Runing...");
}

bool connectWiFi() { // find OK 存在问题，即使连接成功也无法找到OK
  Serial1.println("AT+CWJAP=\"Xiaomi_agv\",\"xiaomiagv\"");
  Serial.println("Connecting AP...");
  delay(6000);
  Serial1.println("AT+CIPSTART=\"TCP\",\"192.168.31.164\",5005");
  Serial.println("Connecting AGVServer...");
  delay(4000);
  Serial1.println("AT+CIPSEND");
  delay(1000);
  Serial1.println("This is AGV0");
  delay(1000);
  flushESP8266();
  return true;
}

void flushESP8266() {
  while(Serial1.read()>=0){}
}

// 电机运转， pwm为0~255的值，dir为 'f' 正转，dir为 'b' 反转
void go(int pwm_l,int pwm_r,char dir) {
  analogWrite(ENA, pwm_l);
  analogWrite(ENB, pwm_r);
  if (dir=='f') {
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    digitalWrite(IN3,HIGH);
    digitalWrite(IN4,LOW);
  }
  else if (dir=='b') {
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,HIGH);
    digitalWrite(IN3,LOW);
    digitalWrite(IN4,HIGH);
  }
}

// 停止动力输出
void brake() { 
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(ENA,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
  digitalWrite(ENB,LOW);
}

// 急停
void brake_immediate() {
  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,HIGH);
  digitalWrite(ENA,LOW); 
  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,HIGH);
  digitalWrite(ENB,LOW);
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
  Serial1.print(custom);
  for (byte i = 0; i < bufferSize; i++) {
    Serial1.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial1.print(buffer[i], DEC);
  }
  Serial1.println();
}

String esp_rec = ""; // ESP8266串口缓存
bool newLineReceived = false;

// 读取ESP8266串口缓存
void readESP8266() {
  while (Serial1.available() > 0) {
    esp_rec += (char)Serial1.read();
    newLineReceived = true;
    delay(2); // 不添加延迟会造成无法一次读取全部串口缓存
  }
}

String cmd = "f000r000"; // dir('f'/'b'),pwm(0~255),turn('r'/'l'),pos(-60~60)
bool cmdReceived = false;
long receiveTime = 0;

bool checkCmd(String raw_cmd) {
  if ((raw_cmd[0]=='f'||raw_cmd[0]=='b')&&
  (raw_cmd.substring(1,4).toInt()>=0&&raw_cmd.substring(1,4).toInt()<=255)&&
  (raw_cmd[4]=='l'||raw_cmd[4]=='r')&&
  (raw_cmd.substring(5).toInt()>=-60&&raw_cmd.substring(5).toInt()<=60)) {
    return true;
  }
  else {
    return false;
  }
}

// 识别上位机控制指令
bool cmdReceive() {
  if (newLineReceived&&checkCmd(esp_rec)) {
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
      rfids[i] = esp_rec.substring(6+22*i,9+22*i).toInt();
      cmd_f[i] = esp_rec.substring(10+i*22,18+i*22);
      cmd_b[i] = esp_rec.substring(19+i*22,27+i*22);
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
  String theCmd = "00000000";
  if (cmdIndex>-1) {
    if (card=='f') {
      if (checkCmd(cmd_f[cmdIndex])) {
        theCmd = cmd_f[cmdIndex];
      }
    }
    else {
      if (checkCmd(cmd_b[cmdIndex])) {
        theCmd = cmd_b[cmdIndex];
      }
    }
  }
  return theCmd;
}

bool safe_f = true; // 前方无障碍
bool safe_b = true; // 后方无障碍

void checkDistance() {
  int dist_f = digitalRead(30)+digitalRead(31);
  int dist_b = digitalRead(32)+digitalRead(33);
  if (dist_f<2) {
    safe_f = false;
  }
  else {
    safe_f = true;
  }
  if (dist_b<2) {
    safe_b = false;
  }
  else {
    safe_b = true;
  }
}

void runByPID(String cmd) {
  char dir = cmd[0];
  int pwm = cmd.substring(1,4).toInt();
  int pos = cmd.substring(5).toInt();
  pwmSet_l = pos==0? pwm:pwm*(1+0.00611*pos);
  pwmSet_r = pos==0? pwm:pwm*(1-0.00611*pos);
  if (pwm==0) {
    brake_immediate();
  }
  else {
    go(pwmOut_l,pwmOut_r,dir);
    if (cmdReceived) {
      turn(pos);
    }
    else {
      turn(posOut);
    }
  }
}

void runByCmd(String cmd) {
  char dir = cmd[0];
  int pwm = cmd.substring(1,4).toInt();
  int pos = cmd.substring(5).toInt();
  if (pwm==0) {
    brake_immediate();
  }
  else {
    go(pwm,pwm,dir);
    turn(pos);
  }
}


void loop() {
  readESP8266();
  if (newLineReceived) {
//    Serial.println("esp_rec:"+esp_rec);
  }
  
  if (cmdReceive()) {
//    Serial.println("cmd:"+cmd);
  }

  if (zoneReceive()) {
//    Serial.print("rfids:");
//    printDec(rfids,6);
//    Serial.print("cmd_f:");
//    printStr(cmd_f,6);
//    Serial.print("cmd_b:");
//    printStr(cmd_b,6);
//    tone(13,1500);
  }
  
  if (scanRFID_F()) {
    String theCmd = getZoneCmd(nuidPICC_F,'f');
    if (theCmd!="00000000") {
      cmd = theCmd;
      cmdReceived = true;
      receiveTime = millis();
//      tone(13,2000);
    }
//    Serial.print("Scaned Front Card:");
//    printDec(nuidPICC_F,4);
    ESPprintDec(nuidPICC_F,4,"Front:");
  }
  if (scanRFID_B()) {
    String theCmd = getZoneCmd(nuidPICC_B,'b');
    if (theCmd!="00000000") {
      cmd = theCmd;
      cmdReceived = true;
      receiveTime = millis();
//      tone(13,2000);
    }
//    Serial.print("Scaned Back Card:");
//    printDec(nuidPICC_B,4);
    ESPprintDec(nuidPICC_B,4,"Back");
  }

  checkDistance();
  if ((safe_f&&cmd[0]=='f')||(safe_b&&cmd[0]=='b')) {
//    runByCmd(cmd);
    runByPID(cmd);
  }
  else {
    brake_immediate();
  }

//  if (volMetro.check()) {
//    float vol = analogRead(A0)/45.0;
//    Serial.println(vol);
//  }

  newLineReceived = false;
  esp_rec = "";
//  noTone(13);
  if (millis()-receiveTime>2500) {
    cmdReceived = false;
  }
  
}


unsigned int count_l = 0;
unsigned int count_r = 0;
unsigned int halfPulse_l = 0;
unsigned int halfPulse_r = 0;
unsigned int halfPulseBefore_l = 0;
unsigned int halfPulseBefore_r = 0;
unsigned long tempTime_l = 0;
unsigned long tempTime_r = 0;
unsigned long pulseTimeInterval_l = 1;
unsigned long pulseTimeInterval_r = 1;
unsigned long halfPulseTime_l = 0;
unsigned long halfPulseTime_r = 0;

void counter_l(void) {
  if (micros()-tempTime_l>10000) {
    count_l++;
    pulseTimeInterval_l = micros()-tempTime_l;
    tempTime_l = micros();
  }
}

void counter_r(void) {
  if (micros()-tempTime_r>10000) {
    count_r++;
    pulseTimeInterval_r = micros()-tempTime_r;
    tempTime_r = micros();
  }
}

void getSpeed(void) {
  halfPulseTime_l = count_l>0? (micros()-tempTime_l):0;
  halfPulseTime_r = count_r>0? (micros()-tempTime_r):0;
  halfPulse_l = 100.0*halfPulseTime_l/pulseTimeInterval_l;
  halfPulse_r = 100.0*halfPulseTime_r/pulseTimeInterval_r;
  int theSpeed_l = (100*count_l-halfPulseBefore_l+halfPulse_l)/5;
  int theSpeed_r = (100*count_r-halfPulseBefore_r+halfPulse_r)/5;
  if (theSpeed_l<300) {
    speed_l = theSpeed_l;
  }
  if (theSpeed_r<300) {
    speed_r = theSpeed_r;
  }
  
  getWhiteLine();
  
  pid_l.Compute();
  pid_r.Compute();
  pid_p.Compute();
//  Serial1.println(String(speed_l)+' '+String(pwmOut_l)+' '+String(speed_r)+' '+String(pwmOut_r));
//  Serial1.println(String(posErro)+' '+String(posOut));
  count_l = 0;
  count_r = 0;
  halfPulseBefore_l = halfPulse_l;
  halfPulseBefore_r = halfPulse_r;
}

void ccd_expose(int microseconds) {
  digitalWrite(SI, HIGH);
  digitalWrite(CLK, HIGH);
  digitalWrite(SI, LOW);
  digitalWrite(CLK, LOW);

  for(byte i=0;i<128;i++) {
    delayMicroseconds(microseconds);  //  saturation time.
    PixelArray[i] = digitalRead(A0);
    digitalWrite(CLK, HIGH);
    digitalWrite(CLK, LOW);
  }
}

void getWhiteLine() {
  ccd_expose(100);
  
  int countWhite = 0;
  int countBlack = 0;
  int startWhite = 0;
  int endWhite = 127;
  bool temp_start = false;
  bool temp_end = false;
  for(int i=0;i<128;i++) {
    if (!temp_start&&PixelArray[i]) {
      startWhite = i;
      temp_start = true;
    }
    if (temp_start&&(!temp_end)&&(!PixelArray[i])) {
      countBlack++;
    }
    else if (temp_start&&(!temp_end)&&PixelArray[i]) {
      countWhite++;
    }
    if (temp_start&&(!temp_end)&&countBlack>2&&countWhite<=2) {
      startWhite = 0;
      countWhite = 0;
      countBlack = 0;
    }
    else if (temp_start&&(!temp_end)&&countBlack>2&&countWhite>2) {
      endWhite = i-countBlack;
      temp_end = true;
    }
  }
  int width = endWhite-startWhite;
  if (width>20&&width<55) {
    posErro = (startWhite+endWhite)/2-63;
    whiteLineCovered = true;
  }
  else {
    posErro = 0;
    whiteLineCovered = false;
  }
}

