#include <Arduino.h>
#include <M5StickC.h>
#include <M5GFX.h>

#define CAL_MS 3000

M5GFX lcd;
M5Canvas canvas(&lcd);

float gyro[3];

double yaw_raw;
double yaw_offset;
double yaw;

double cal_val;
bool iscal;
int cal_timems;

int old_cal_ms;

int serial_send_start;
int serial_send_next;

double micro_cal_data = 0;


void loop_ui()
{
  canvas.clear();

  canvas.setTextFont(0);
  canvas.setCursor(0, 0);
  canvas.println("X:" + String(gyro[0]));
  canvas.setCursor(0, 10);
  canvas.println("Y:" + String(gyro[1]));
  canvas.setCursor(0, 20);
  canvas.println("Z:" + String(gyro[2]));
  canvas.setCursor(0, 40);
  canvas.println("cal_val:" + String((cal_val)));
  canvas.setCursor(0, 50);
  canvas.println("deg_raw:" + String((int)(yaw)));
  canvas.setCursor(0, 60);
  canvas.println("deg:" + String((int)(yaw / 100)));
  canvas.setCursor(0, 70);
  canvas.println("cal:" + String((micro_cal_data)));

  canvas.drawCircle(120, 40, 25, M5.Lcd.color565(255, 255, 255));

  if(iscal)
  {
    canvas.setTextFont(2);
    canvas.setCursor(110, 32);
    canvas.println(String((cal_timems / 1000.0), 1));
  }
  else
  {
    canvas.drawLine(120, 
                    40, 
                    120 + sin(radians(yaw / 9500.0 * 90.0)) * 25, 
                    40 + cos(radians(yaw / 9500.0 * 90.0) - PI) * 25, 
                    M5.Lcd.color565(255, 0, 0));
  }

  canvas.pushSprite(&lcd, 0, 0);
}

void reset_value()
{
  yaw_raw = 0;
  yaw_offset = 0;
  yaw = 0;
}

void read_imudata()
{
  M5.Imu.getGyroData(&gyro[0], &gyro[1], &gyro[2]);
}

void process_gyrodata()
{
  // calc yaw
  yaw_raw += ((gyro[2]) * 10) / 10 + (cal_val * ((millis() - old_cal_ms) )) + micro_cal_data;
  old_cal_ms = millis();

  yaw = yaw_raw - yaw_offset;
}

void process_calibration()
{
  iscal = true;
  reset_value();
  
  int cal_start = millis();
  int process_count = 0;

  cal_val = 0;
  micro_cal_data = 0;

  while(millis() - cal_start <= CAL_MS)
  {
    cal_timems = CAL_MS - (millis() - cal_start);
    process_gyrodata();
    read_imudata();
    loop_ui();
  }

  cal_val = -((yaw_raw + 1) / (CAL_MS));
  iscal = false;

  Serial1.println("calibration");

  reset_value();
}

void setup() {
  M5.begin();

  M5.Mpu6886.SetGyroFsr(M5.Mpu6886.GFS_2000DPS);
  M5.Imu.Init();
  
  lcd.begin();
  canvas.setRotation(3);
  canvas.createSprite(lcd.width(), lcd.height());

  Serial1.begin(115200, SERIAL_8N1, 32, 33);

  serial_send_start = millis();

  pinMode(26, ANALOG);

  process_calibration();
}

void loop() {
  M5.update();

  micro_cal_data = (analogRead(26)) / 4096.0 * 2.0;

  if(M5.BtnA.wasPressed())
  {
    yaw_offset = yaw_raw;
  }
  if(M5.BtnA.pressedFor(2000))
  {
    yaw_offset = yaw_raw;
    process_calibration();
  }
  
  read_imudata();

  process_gyrodata();

  loop_ui();

  if(millis() - serial_send_start >= serial_send_next)
  {
    Serial1.println((yaw / 100.0));
    serial_send_next += 10;
  }
}