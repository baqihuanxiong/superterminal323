/*
 * 2018/2/28 by slw
 * 
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
#include"Fieldpro.h"

#define RST_PIN_F 46
#define SS_PIN_F 49
#define RST_PIN_B 48
#define SS_PIN_B 47
#define ENA 8 // 电机使能，接L298N ENA
#define IN1 9 // 接L298N IN1
#define IN2 10 // 接L298N IN2
#define ENB 12 // 电机使能，接L298N ENB
#define IN3 7 // 接L298N IN3
#define IN4 11 // 接L298N IN4
#define CLK 22 // ccd时钟
#define SI 24 // ccd触发

MFRC522 rfid_f(SS_PIN_F, RST_PIN_F); // 前置RFID感应模块
byte nuidPICC_F[4]; // 存储前卡识别到的RFID标签id
MFRC522 rfid_b(SS_PIN_B, RST_PIN_B); // 后置RFID感应模块,暂时不用
byte nuidPICC_B[4];

double pwmSet_l,speed_l,pwmOut_l;
double pwmSet_r,speed_r,pwmOut_r;
double posSet,posErro,posOut;
PID pid_l(&speed_l,&pwmOut_l,&pwmSet_l,0.55,0.35,0.05,P_ON_E,DIRECT);
PID pid_r(&speed_r,&pwmOut_r,&pwmSet_r,0.55,0.35,0.05,P_ON_E,DIRECT);
PID pid_p(&posErro,&posOut,&posSet,0.52,0,0.08,DIRECT);

Servo myservo; // 转向舵机
const int init_pos = 85; // 舵机初始角

bool PixelArray[128]; // ccd像素向量
bool whiteLineCovered = false; // 是否在白线上

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
  pid_p.SetOutputLimits(-15,15);
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

void flushRFID_F() {
  for (byte i = 0; i < 4; i++) {
    nuidPICC_F[i] = 0;
  }
}

bool compare_Arr(byte *arr1, byte *arr2, byte arrSize) {
  for (byte i = 0; i < arrSize; i++) {
    if (arr1[i]!=arr2[i]) {
      return false;
    }
  }
  return true;
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
bool loadState = false;  // 空箱行驶
int runState = 0;  // 0初始状态，1停车待命，2启动前进
Zone zonePresent,zoneNext;
bool enterZone = false; // 进入区段信号
int partProcedure = 0;  // 区段流程序号
bool startTurn = false; // 开始转弯信号
bool retreat = false;  // 后退一小段距离信号
unsigned long temp_mileage = 0; 
unsigned long mileage = 0;  // 里程计
unsigned long ms2_time = 0;  // MsTimer2计时器

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
bool zoneReceive() {
  if (newLineReceived&&esp_rec.startsWith("z")) {
    int startCardIndex = esp_rec.substring(1,4).toInt();
    int procedureIndex = esp_rec.substring(4,6).toInt();
    int endCardIndex = esp_rec.substring(6).toInt();
    Procedure procedure = getZoneProcedure(procedureIndex);
    zoneNext.card_start = rfidMap[startCardIndex-1];
    zoneNext.card_end = rfidMap[endCardIndex-1];
    zoneNext.cmds = procedure.cmds;
    zoneNext.periods = procedure.periods;
    zoneNext.len = procedure.len;
    esp_rec = "";
    newLineReceived = false;
    return true;
  }
  else {
    return false;
  }
}

void dispZone(Zone zone) {
  printDec(zone.card_start,4);
  printDec(zone.card_end,4);
  Serial.println(zone.cmds[0]);
  Serial.println(zone.periods[0]);
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
//    go(pwm-15,pwm-15,dir);
    go(pwmOut_l,pwmOut_r,dir);
    if (startTurn) {
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
  readESP8266();  // 读取wifi串口
  if (newLineReceived) {
    Serial1.println("esp_rec:"+esp_rec);
  }
  
  if (cmdReceive()) {  // 接受上位机控制指令
//    Serial.println("cmd:"+cmd);
  }

  if (zoneReceive()) {  // 接受上位机区段指令
    if (runState==0) {  //首次启动
      cmd = loadState? CMD_HEAVY:CMD_EMPTY;
      zonePresent = zoneNext;  // 添加初始区段
      runState = 2;  // 启动
    }
    else if (runState==1) {  // 若当前正在待命则启动前进
      cmd = loadState? CMD_HEAVY:CMD_EMPTY;
      runState = 2;  // 启动
    }
//    dispZone(zonePresent);
    Serial1.println("OK");
  }
  
  if (scanRFID_F()) {  // 前卡得到标签
    ESPprintDec(nuidPICC_F,4,"F:");
//    ESPprintDec(zonePresent.card_end,4,"C:");

    unsigned long time_compensate = (micros()-ms2_time)/1000;  // 与timer2时间差
    double compensate_ratio = time_compensate/150.0;

    if (compare_Arr(nuidPICC_F,zonePresent.card_start,4)) {  // 到达当前区段开始标签（初始区段）
      Serial1.println("ef");
      enterZone = true;  // 标记已进入区段
      temp_mileage = mileage+compensate_ratio*(speed_l+speed_r)/2; // 记一次里程
    }
    else if (compare_Arr(nuidPICC_F,zonePresent.card_end,4)) {  // 到达当前区段结束标签
      if (compare_Arr(zonePresent.card_end,zoneNext.card_start,4)) {  // 若已得到下一区段
        Serial1.println("en");
        zonePresent = zoneNext;  // 更新当前区段
        enterZone = true;  // 标记已进入区段
        temp_mileage = mileage+compensate_ratio*(speed_l+speed_r)/2; // 记一次里程
      }
      else {  // 若未得到下一区段
        // 进入特殊区段,后退一定距离，待得到下一区段后重启
        Serial1.println("ew");
        runState = 1;  // 待命
        retreat = true;
        temp_mileage = mileage+compensate_ratio*(speed_l+speed_r)/2; // 记一次里程
      }
    }
  }
  
  if (scanRFID_B()) {  // 后卡得到标签
//    ESPprintDec(nuidPICC_B,4,"B");
  }

  if (enterZone) {  //  开始区段流程
    unsigned long time_compensate = (micros()-ms2_time)/1000;  // 与timer2时间差
    double compensate_ratio = time_compensate/150.0;
    int mileInterval = mileage+compensate_ratio*(speed_l+speed_r)/2-temp_mileage;
    if (partProcedure<zonePresent.len) {  // 区段流程中的第partProcedure段
      if (mileInterval>zonePresent.periods[partProcedure]&&mileInterval<zonePresent.periods[partProcedure+1]) {  // 开始转弯
        startTurn = true;
        cmd = zonePresent.cmds[partProcedure];
//        Serial1.println(mileInterval);
      }
      else if (mileInterval>zonePresent.periods[partProcedure+1]) {
        partProcedure++;
//        Serial1.println(mileInterval);
      }
    }
    else {  // 区段流程完成
      startTurn = false;
      enterZone = false;
      partProcedure = 0;
      cmd = loadState? CMD_HEAVY:CMD_EMPTY;
    }
  }

  if (retreat) {  // 开始后退
    int mileInterval = mileage-temp_mileage;
    if (mileInterval<RETREAT_DISTANCE) {
      cmd = CMD_RETREAT;
    }
    else {
      retreat = false;
      cmd = CMD_STOP;
      flushRFID_F();
    }
  }
  
//  Serial1.println("m"+String(mileage));
//  Serial1.println(ms2_time,DEC);

  checkDistance();  // 更新障碍物指示
  if ((safe_f&&cmd[0]=='f')||(safe_b&&cmd[0]=='b')) {
//    runByCmd(cmd);
    runByPID(cmd);
  }
  else {
    brake_immediate();
  }
  
//  if (volMetro.check()) {
//    float vol = analogRead(A0)/45.0;
//    Serial1.println(vol);
//  }

  newLineReceived = false;
  esp_rec = "";
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
  ms2_time = micros();
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
  mileage = mileage+(speed_l+speed_r)/2;
  
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
    double thePosErro = (startWhite+endWhite)/2-63;
    if (abs(thePosErro-posErro)<45) {
      posErro = thePosErro;
    }
    whiteLineCovered = true;
  }
  else {
    posErro = 0;
    whiteLineCovered = false;
  }
}

