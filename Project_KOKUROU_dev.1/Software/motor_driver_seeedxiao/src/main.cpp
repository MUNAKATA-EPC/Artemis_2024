#include <Arduino.h>

const int PIN_MOTOR_INA[3] = { 2, 3, 8 };
const int PIN_MOTOR_INB[3] = { 1, 4, 9 };
const int PIN_MOTOR_PWM[3] = { 0, 5, 10 };

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

void setup() {  
  motors[0].init(50, 0);
  motors[1].init(50, 0);
  motors[2].init(50, 0);
}

void loop() {
  //3つのモーターに対して処理
  for(int i = 0; i < 3; i++)
  {
    if(motors[i].get_rotate() == 0)
    {
      if(motors[i].get_power() == 0)
      {
        analogWrite(PIN_MOTOR_INA[i], 0);
        analogWrite(PIN_MOTOR_INB[i], 0);
        analogWrite(PIN_MOTOR_PWM[i], 0);
      }
      else
      {
        analogWrite(PIN_MOTOR_INA[i], 1023);
        analogWrite(PIN_MOTOR_INB[i], 0);
        analogWrite(PIN_MOTOR_PWM[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
      }
    }
    else
    {
      if(motors[i].get_power() == 0)
      {
        analogWrite(PIN_MOTOR_INA[i], 1023);
        analogWrite(PIN_MOTOR_INB[i], 1023);
        analogWrite(PIN_MOTOR_PWM[i], 0);
      }
      else
      {
        analogWrite(PIN_MOTOR_INA[i], 0);
        analogWrite(PIN_MOTOR_INB[i], 1023);
        analogWrite(PIN_MOTOR_PWM[i], (int)((motors[i].get_power()) / 100.0 * 1023.0));
      }
    }
  }
}