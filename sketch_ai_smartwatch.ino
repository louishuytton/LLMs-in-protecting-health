#include "DFRobot_LTR390UV.h"
#include <DHT.h>
#include <Adafruit_GC9A01A.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_TinyUSB.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial mySerial(1, 2); // RX, TX
StaticJsonDocument<100> doc;

#define TFT_CS 8
#define TFT_RST 10
#define TFT_DC 9
#define TFT_SDI 2   // Chân MOSI
#define TFT_CLK 1   // Chân SCK SCL
#define TFT_BLK 9  // Chân đèn nền (backlight)

DFRobot_LTR390UV ltr390(/*addr = */LTR390UV_DEVICE_ADDR, /*pWire = */&Wire);
DHT dht(0,21);
Adafruit_GC9A01A tft = Adafruit_GC9A01A(TFT_CS, TFT_DC, TFT_RST);

uint32_t data_UV = 0;
float temp = 0;
float humidity = 0;
float dataUV = 0;

void setup()  {
  Serial.begin(9600);
  setup_UVsensor();
  setup_DHT21();
  mySerial.begin(9600);
}

void loop()  {
  collectDataUV();
  controlDHT21();
  transmitData();

}

void collectDataUV()  {
  data_UV = ltr390.readOriginalData();//Get UV raw data
  dataUV = data_UV / 40;
}

void setup_UVsensor()  {
  while(ltr390.begin() != 0){
    Serial.println(" Sensor initialize failed!!");
  }
  Serial.println(" Sensor  initialize success!!");
  ltr390.setALSOrUVSMeasRate(ltr390.e18bit,ltr390.e100ms);//18-bit data, sampling time of 100ms 
  ltr390.setALSOrUVSGain(ltr390.eGain3);//Gain of 3
  ltr390.setMode(ltr390.eUVSMode);//Set UV mode
}

void setup_DHT21()  {
  dht.begin();
}

void controlDHT21()  {
  temp = dht.readTemperature();
  humidity = dht.readHumidity();
}
void setup_data()  {

}
void transmitData() {
  // Gán giá trị nhiệt độ và độ ẩm vào đối tượng JsonDocument
  doc["temperature"] = round(temp);
  doc["humidity"] = round(humidity);
  doc["UV"] = round(dataUV);
  // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp ảo
  serializeJson(doc, mySerial);
  mySerial.println();
  // Chuyển đổi đối tượng JsonDocument thành chuỗi JSON và gửi nó qua cổng nối tiếp thật
  serializeJson(doc, Serial);
  Serial.println();
  //Serial.print(UV);
  delay(5000);
}
