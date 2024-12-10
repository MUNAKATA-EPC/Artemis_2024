#include <Arduino.h>

const int PIN_MOTOR_INA[3] = { D2, D3, D8 };
const int PIN_MOTOR_INB[3] = { D1, D4, D9 };
const int PIN_MOTOR_PWM[3] = { D0, D5, D10 };

class Motor{
private:
  int _rotate;
  int _power;

public:
  /// @brief モーターデータの初期化をします。
  /// @param power モーター駆動パワー(0~100%)
  /// @param rotate モーター回転方向(0:順転, 1:逆転)
  void init(int power, int rotate)
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

Motor motors[3];

void get_motor_data()
{
  String data = "aaa";

  while(Serial1.available() > 0)
  {
    data = Serial1.readString();
  }

  if(data[0] != 'a')
  {
    Serial.println(data);
    int motora_rotate = ( (String)data[0]).toInt();
    int motorb_rotate = ( (String)data[4]).toInt();
    int motorc_rotate = ( (String)data[8]).toInt();

    int motora_power = (((String)data[1] + (String)data[2] + (String)data[3])).toInt();
    int motorb_power = (((String)data[5] + (String)data[6] + (String)data[7])).toInt();
    int motorc_power = (((String)data[9] + (String)data[10] + (String)data[11])).toInt();

    motors[0].init(motora_power, motora_rotate);
    motors[1].init(motorb_power, motorb_rotate);
    motors[2].init(motorc_power, motorc_rotate);
  }
}

void setup() {  
  motors[0].init(0, 0);
  motors[1].init(0, 0);
  motors[2].init(0, 0);

  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  pinMode(D8, OUTPUT);
  pinMode(D9, OUTPUT);

  analogWriteFreq(5000);
  analogWriteRange(1023);

  Serial.begin(9600);

  Serial1.begin(115200);
  Serial1.setTimeout(10);
  
}

void loop() {
  get_motor_data();

  //3つのモーターに対して処理
  for(int i = 0; i < 3; i++)
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

  delay(10);
}