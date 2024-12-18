#include <Arduino.h>

#define PIN_DATA 0

#define PIN_S0 1
#define PIN_S1 2
#define PIN_S2 3
#define PIN_S3 4

#define PIN_E 5


int line_pins[16] = {7, 6, 5, 4, 3, 2, 1, 0, 15, 14, 13, 12, 11, 10, 9, 8};

int line_circle_values[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int output_value;

/// @brief デバッグ出力をする関数です。
void print_debug_value()
{
  for(int i = 0; i < 19; i++)
  {
    Serial.print(line_circle_values[i]);
    Serial.print(", ");
  }

  Serial.println(output_value);
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

    return digitalRead(PIN_DATA);
}


    void print_debug_value_()
    {
        for(int i = 0; i < 19; i++)
        {
            Serial.print(line_circle_values[i]);
            Serial.print(", ");
        }
        Serial.println();
    }

void setup() {
  Serial.begin(9600);

  pinMode(PIN_DATA, INPUT_PULLUP);
  pinMode(PIN_S0, OUTPUT);
  pinMode(PIN_S1, OUTPUT);
  pinMode(PIN_S2, OUTPUT);
  pinMode(PIN_S3, OUTPUT);

  
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);

  pinMode(PIN_E, OUTPUT);

  Serial1.begin(57600);
}

void loop() {
  print_debug_value();
  //出力時は常にPIN_EはLOWにする必要がある
  digitalWrite(PIN_E, LOW);

  //16ピン分のセンサーの値をマルチプレクサから取得する
  for(int i = 0; i < 16; i++)
  {
    line_circle_values[i] = get_from_multiplexer(line_pins[i]);
  }

  line_circle_values[16]  =  digitalRead(8);
  line_circle_values[17]  =  digitalRead(9);
  line_circle_values[18]  =  digitalRead(10);

  output_value = 0;
  
  for(int i = 0; i < 19; i++)
  {
    if(line_circle_values[i] == 0)
    {
      output_value += pow(2, i);
    }
  }

  print_debug_value();
  Serial1.println(output_value);
  Serial1.flush();

  delay(10);
}