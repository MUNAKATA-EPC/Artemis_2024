#include <Arduino.h>
#include <Wire.h>

//シリアルモニタを起動しなければプログラムが走らない！！！

// Connect L3GD20 with SDA (A4), SCL (A5)

const byte L3GD20_ADDR = B1101010;  // SA0 = GND
//const byte L3GD20_ADDR = B1101011;// SA0 = VDD_IO

const byte L3GD20_WHOAMI = 0x0F;
const byte L3GD20_CTRL1 = 0x20;
const byte L3GD20_CTRL2 = 0x21;
const byte L3GD20_CTRL3 = 0x22;
const byte L3GD20_CTRL4 = 0x23;
const byte L3GD20_CTRL5 = 0x24;
const byte L3GD20_X_L = 0x28;
const byte L3GD20_X_H = 0x29;
const byte L3GD20_Y_L = 0x2A;
const byte L3GD20_Y_H = 0x2B;
const byte L3GD20_Z_L = 0x2C;
const byte L3GD20_Z_H = 0x2D;

#define ledpin 13
#define ledpin2 12
#define analogpin 0
#define resetpin 1
#define outputpin 0
#define CAL_TIMEMS 1000
#define UPDATE_MS 20

double degdata = 0;
double caldata = 0;//←ここにキャリブレーション値のデフォルトを

int current_time;
int last_time;

int before_z;

bool bCalNow;

int cal_count;

void Writegyro(byte reg, byte val)
{
  Wire.beginTransmission(L3GD20_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();  
}

byte Readgyro(byte reg)
{
  byte ret = 0;
  Wire.beginTransmission(L3GD20_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((unsigned int)L3GD20_ADDR, 1);
  
  while (Wire.available()) {
    ret = Wire.read();
  }
  
  return ret;
}

void GetDegdata(){
  short Z = Readgyro(L3GD20_Z_H);
  double z = (Z = (Z << 8) | Readgyro(L3GD20_Z_L)) * 100;

  double microcaldata = analogRead(analogpin);
  microcaldata = (microcaldata - 512) / 500 * 12.5;
  
  double ave_z = before_z == 0 ? z : (z + before_z) / 2;

  if(bCalNow)
  {
    degdata += ave_z;
  }
  else
  {
    degdata += ave_z + caldata;// + microcaldata;
  }

  before_z = z;
}

void Calibration(){
  if(cal_count == 0)
  {
    caldata = 0;
    Serial.println("####[Calibration Started]####");
  }

  GetDegdata();
  cal_count++;
  Serial.println(cal_count);
  
  if(cal_count == (CAL_TIMEMS / UPDATE_MS))
  {
    caldata = (0 - degdata) / (float)cal_count;
    degdata = 0;
    bCalNow = false;
  
    Serial.print("####[Calibration Ended value = ");
    Serial.print(caldata);
    Serial.print("]####");
  }
}

void Output_DegData()
{
  GetDegdata();

  long outputdata = (degdata / 480.0 / 100.0) + 512;
  if(outputdata > 1023 ){
    outputdata = 1023;
  } else if(outputdata < 0){
    outputdata = 0;     
  }
  int a =  (analogRead(analogpin)) - 512;

  float micro = a / 500.0;
  Serial.print(micro);
  Serial.print(" ");
  Serial.print(caldata);
  Serial.print(" ");
  Serial.println(outputdata);

  Serial1.println(outputdata);
}

void setup() {
  analogWriteResolution(10);
  
  pinMode(resetpin, INPUT_PULLDOWN);
  pinMode(ledpin, OUTPUT);
  pinMode(ledpin2, OUTPUT);

  Serial.begin(9600);
  Wire.begin();
  
  Writegyro(L3GD20_CTRL1, B00001111);
                      //   |||||||+ X axis enable
                      //   ||||||+- Y axis enable
                      //   |||||+-- Z axis enable
                      //   ||||+--- PD: 0: power down, 1: active:]
                      //   ||++---- BW1-BW0: cut off 12.5[Hz]
                      //   ++------ DR1-DR0: ODR 95[HZ]

  Writegyro(L3GD20_CTRL4, B00110000);

  Serial1.begin(115200);
}

void loop() {
  if(digitalRead(resetpin) == HIGH){
    degdata = 0;
    cal_count = 0;
    bCalNow = true;
  }

  if(bCalNow)
  {
    Calibration();
  }
  else
  {
    Output_DegData();
  }

  delay(UPDATE_MS);
}
