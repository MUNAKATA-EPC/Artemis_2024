#include <Arduino.h>
#include <stdlib.h>

#define PIN_DATA 0

#define PIN_S0 1
#define PIN_S1 2
#define PIN_S2 3
#define PIN_S3 4

#define PIN_E 5

/// @brief センサ反応チェック値
#define DETECTED_CHECK_VALUE 960

class TSSP58038{
public:
  int value;
  int index;
};

TSSP58038 sensor_data[16];
TSSP58038 sensor_data_temp[16];

int ball_deg;
int ball_distance;

int ball_distance_data[5] = {-1, -1, -1, -1, -1};

class Vector{
private:
  double _x;
  double _y;
public:
  void set(int deg)
  {
    _x = cos(radians(deg));
    _y = sin(radians(deg));
  }

  double get_x()
  {
    return _x;
  }
  double get_y()
  {
    return _y;
  }
};

/// @brief デバッグ出力をする関数です。
void print_debug_value()
{
  for(int i = 0; i < 16; i++)
  {
    Serial.print(sensor_data[i].value);
    Serial.print(", ");
  }

  Serial.println();
}

/// @brief マルチプレクサーからの値を取得する関数です。
/// @param idx マルチプレクサから取得する値のインデックス。
/// @return 指定したインデックスのアナログ値。
int get_from_multiplexer(int idx)
{
    uint8_t mul_address = idx;
    uint8_t bit;
    
    for(uint8_t j = 0; j < 4; j++)
    {
      bit = 1 << j;

      if(bit & mul_address){
        digitalWrite(j + PIN_S0, HIGH);
      }
      else{
        digitalWrite(j + PIN_S0, LOW);
      }
    }

    delayMicroseconds(10);

    return analogRead(PIN_DATA);
}

/// @brief センサーの値が一つでも反応しているかどうか
/// @return 反応していたらTrue、反応していなかったらFalse
bool is_any_detected()
{
  bool ret = false;
  for(int i = 0; i < 16; i++)
  {
    if(sensor_data[i].value <= DETECTED_CHECK_VALUE)
    {
      ret = true;
      break;
    }
  }

  return ret;
}

/// @brief 配列比較用関数(qsortで使用)
/// @param a 比較する値1
/// @param b 比較する値2
/// @return a=b : 0, a<b : -1, a>b : 1
int compare_int(const void* a, const void* b)
{
  int a_num = ((TSSP58038*)a)->value;
  int b_num = ((TSSP58038*)b)->value;

  if(a_num < b_num)
  {
    return -1;
  }
  else if(a_num > b_num)
  {
    return 1;
  }

  return 0;
}

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DATA, INPUT_PULLUP);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);

  pinMode(PIN_E, OUTPUT);

  Serial1.begin(19200);
}

void loop() {

  //出力時は常にPIN_EはLOWにする必要がある
  digitalWrite(PIN_E, LOW);

  //16ピン分のセンサーの値をマルチプレクサから取得する
  for(int i = 0; i < 16; i++)
  {
    sensor_data[i].index = i + 1;
    sensor_data[i].value = get_from_multiplexer(i);
  }

  //ボールセンサのデバッグ用に値出力する
  if(is_any_detected())
  {
    for(int i = 0; i < 16; i++)
    {
      sensor_data_temp[i] = sensor_data[i];
    }

    qsort(sensor_data, 
          sizeof(sensor_data) / sizeof(sensor_data[0]),
          sizeof(TSSP58038),
          compare_int);
    
    int max_detected_index = sensor_data[0].index - 1;
    int index_of_around_max_detected_sensor[5] = {
        (max_detected_index - 2 + 16) % 16, 
        (max_detected_index - 1 + 16) % 16, 
        max_detected_index,
        (max_detected_index + 1) % 16,
        (max_detected_index + 2) % 16
      };

    double x_of_ball_deg = 0;
    double y_of_ball_deg = 0;

    for(int i = 0; i < 5; i++)
    {
      x_of_ball_deg += cos(radians(index_of_around_max_detected_sensor[i] * 22.5)) * (1023 - sensor_data_temp[index_of_around_max_detected_sensor[i]].value);
      y_of_ball_deg += sin(radians(index_of_around_max_detected_sensor[i] * 22.5)) * (1023 - sensor_data_temp[index_of_around_max_detected_sensor[i]].value);
    }
    
    double deg_of_ball = degrees(atan2(y_of_ball_deg, x_of_ball_deg));
    deg_of_ball = deg_of_ball < 0 ? deg_of_ball + 360 : deg_of_ball;

    ball_deg = (int)deg_of_ball;
    ball_distance = (int)sqrt(pow(x_of_ball_deg, 2) + pow(y_of_ball_deg, 2)) / 1500.0 * 100.0;

    int temp = ball_distance;
    for(int i = 4; i > 0; i--)
    {
      ball_distance_data[i] = ball_distance_data[i - 1];
    }

    ball_distance_data[0] = temp;

    if(ball_distance_data[4] != -1)
    {
      ball_distance = (ball_distance_data[0] + ball_distance_data[1] + ball_distance_data[2] + ball_distance_data[3] + ball_distance_data[4]) / 5;
    }
  }
  else
  {
    ball_deg = -1;
    ball_distance = -1;
  }

  Serial1.print(String(ball_deg));
  Serial1.print(String('a'));
  Serial1.print(String("\t"));
  Serial1.print(String(ball_distance));
  Serial1.print(String('b'));
  Serial1.println(String("\t"));

  delay(10);
}