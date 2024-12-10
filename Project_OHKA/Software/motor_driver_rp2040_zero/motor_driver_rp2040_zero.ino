#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

const int PIN_MOTOR_INA[4] = {D2, D5,  D8, D26};
const int PIN_MOTOR_INB[4] = {D4, D7, D14, D27};
const int PIN_MOTOR_PWM[4] = {D3, D6, D15, D28};

#define DIN_PIN 16            // NeoPixel　の出力ピン番号はGP16
#define LED_COUNT 1           // LEDの連結数
#define WAIT_MS 1000          // 次の点灯までのウエイト
#define BRIGHTNESS 32        // 輝度
Adafruit_NeoPixel pixels(LED_COUNT, DIN_PIN, NEO_GRB + NEO_KHZ800);

class Motor{
private:
  int _rotate;
  int _power;

public:
  /// @brief モーターデータをセットします。
  /// @param power モーター駆動パワー(0~100%)
  /// @param rotate モーター回転方向(0:順転, 1:逆転)
  void set(int power, int rotate)
  {
    this->_power = power;
    this->_rotate = rotate;
  }
  
  /// @brief モーター駆動パワーを取得します。
  /// @return モーター駆動パワー(0-100%)
  int get_power()
  {
    return this->_power;
  }

  /// @brief モーター回転方向を取得します。
  /// @return モーター回転方向(0:順転, 1:逆転)
  int get_rotate()
  {
    return this->_rotate;
  }
};

Motor motors[4];

void get_motor_data()
{
  String data = "aaa";

  if(Serial1.available() > 0)
  {
    data = Serial1.readStringUntil('\n');
    Serial.println(data);

    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(0, 0, BRIGHTNESS));
    pixels.show();
  }
  else
  {
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    pixels.show();
  }

  if(data[0] != 'a')
  {
    int motora_rotate = ( (String)data[0]).toInt();    int motorb_rotate = ( (String)data[4]).toInt();
    int motorc_rotate = ( (String)data[8]).toInt();
    int motord_rotate = ( (String)data[12]).toInt();

    int motora_power = (((String)data[1] + (String)data[2] + (String)data[3])).toInt();
    int motorb_power = (((String)data[5] + (String)data[6] + (String)data[7])).toInt();
    int motorc_power = (((String)data[9] + (String)data[10] + (String)data[11])).toInt();
    int motord_power = (((String)data[13] + (String)data[14] + (String)data[15])).toInt();

    motors[0].set(motora_power, motora_rotate);
    motors[1].set(motorb_power, motorb_rotate);
    motors[2].set(motorc_power, motorc_rotate);
    motors[3].set(motord_power, motord_rotate);
  }
}

void setup() {
  analogWriteFreq(20000);
  analogWriteRange(1023);

  pixels.begin();             //NeoPixel制御開始

  for(int i = 0; i < 4; i++) {
    motors[i].set(0, 0);

    pinMode(PIN_MOTOR_INA[i], OUTPUT);
    pinMode(PIN_MOTOR_INB[i], OUTPUT);
  }

  pixels.clear();
  pixels.setPixelColor(0, pixels.Color(0, 0, 0));
  pixels.show();

  Serial.begin(9600);

  Serial1.setTX(D0);
  Serial1.setRX(D1);
  Serial1.begin(115200);
  Serial1.setTimeout(10);
}

void loop() {
  get_motor_data();

  //4つのモーターに対して処理
  for(int i = 0; i < 4; i++)
  {
    if(motors[i].get_rotate() == 0)
    {
      if(motors[i].get_power() == 0)
      {
        digitalWrite(PIN_MOTOR_INA[i], LOW);
        digitalWrite(PIN_MOTOR_INB[i], LOW);
        analogWrite(PIN_MOTOR_PWM[i], 0);
      }
      else
      {
        digitalWrite(PIN_MOTOR_INA[i], HIGH);
        digitalWrite(PIN_MOTOR_INB[i], LOW);
        analogWrite(PIN_MOTOR_PWM[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
      }
    }
    else
    {
      if(motors[i].get_power() == 0)
      {
        digitalWrite(PIN_MOTOR_INA[i], HIGH);
        digitalWrite(PIN_MOTOR_INB[i], HIGH);
        analogWrite(PIN_MOTOR_PWM[i], 0);
      }
      else
      {
        digitalWrite(PIN_MOTOR_INA[i], LOW);
        digitalWrite(PIN_MOTOR_INB[i], HIGH);
        analogWrite(PIN_MOTOR_PWM[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
      }
    }
  }
}
