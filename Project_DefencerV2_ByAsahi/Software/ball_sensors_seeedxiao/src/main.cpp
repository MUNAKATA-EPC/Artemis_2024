#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>


//IR_SENSORの許可できる最大反応値を代入します。
const int PermissionRange = 100;    //about 90% (1024 * 0.9)
int BallDeg;                        //最終出力時のボールの角度
int BallDistance;                   //最終出力時のボールの距離
typedef struct{
  int index = 0;
  int value = 0;
} IR_SENSOR;

IR_SENSOR IR_Sen[8];
IR_SENSOR IR_Sen_backup[8];

void Read_IR(); 
int OutputValue(int index);

//比較対象のvalue値の比較を行います。
//n1<n2 : 1 
//n1>n2 : -1
//n1=n2 : 0
int CompareIRValue(const void* n1, const void* n2){
  if(((IR_SENSOR*)n1)->value < ((IR_SENSOR*)n2)->value)
    return 1;
  else if(((IR_SENSOR*)n1)->value > ((IR_SENSOR*)n2)->value)
    return -1;
  else 
    return 0;
}

void setup() {
  Serial.begin(9600);

  Serial1.begin(38400);
}

void loop() {
  Read_IR();

  int permission_check = 0;

  for(int i = 0; i < 8; i++)
  {
    IR_Sen_backup[i] = IR_Sen[i];

    if(IR_Sen[i].value <= PermissionRange)
    {
      permission_check++;
    }
  }

  

  if(permission_check == 8)
  {
    BallDeg = -1;
    BallDistance = -1;
  }
  else
  {
    BallDeg = 0;
    BallDistance = 0;
  }

  //qsort関数により配列の値を降順ソートします。
  qsort(IR_Sen, sizeof(IR_Sen) / sizeof(IR_Sen[0]), sizeof(IR_SENSOR), CompareIRValue);

  int max_sensor_index = IR_Sen[0].index - 1;
  int left_max_sensor_index = (max_sensor_index + 8 - 1) % 8;
  int right_max_sensor_index = (max_sensor_index + 1) % 8;

  int max_sensor_deg = max_sensor_index * 45;
  int left_max_sensor_deg = left_max_sensor_index * 45;
  int right_max_sensor_deg = right_max_sensor_index * 45;

  double max_sensor_pos[2] = { IR_Sen_backup[max_sensor_index].value * cos(radians(max_sensor_deg)) , IR_Sen_backup[max_sensor_index].value * sin(radians(max_sensor_deg)) };
  double left_max_sensor_pos[2] = { IR_Sen_backup[left_max_sensor_index].value * cos(radians(left_max_sensor_deg)) , IR_Sen_backup[left_max_sensor_index].value * sin(radians(left_max_sensor_deg)) };
  double right_max_sensor_pos[2] = { IR_Sen_backup[right_max_sensor_index].value * cos(radians(right_max_sensor_deg)) , IR_Sen_backup[right_max_sensor_index].value * sin(radians(right_max_sensor_deg)) };

  int ir_ball_x = (int)(max_sensor_pos[0] + left_max_sensor_pos[0] + right_max_sensor_pos[0]);
  int ir_ball_y = (int)(max_sensor_pos[1] + left_max_sensor_pos[1] + right_max_sensor_pos[1]);

  if(BallDeg != -1)
  {
    BallDeg = (int)(degrees(atan2(ir_ball_y, ir_ball_x)));
    BallDeg = BallDeg < 0 ? BallDeg + 360 : BallDeg;

    BallDistance = (int)sqrt(pow(ir_ball_x, 2) + pow(ir_ball_y, 2)) / 1500.0 * 100.0;
  }

 

  if(BallDeg == 0)
  {
    BallDeg = -1;
  } 
  Serial1.print(String(BallDeg));
  Serial1.print(String('a'));
  Serial1.print(String("\t"));
  Serial1.print(String(BallDistance));
  Serial1.print(String('b'));
  Serial1.println(String("\t"));

  delay(10);
}


//各ポートのIRセンサーの値を読み取り、構造体のvalue変数に代入します。
void Read_IR(){
  for(int i = 0; i < 8; i++){
    IR_Sen[i].index = i + 1;
  }
  IR_Sen[0].value = 1023 - analogRead(0);
  IR_Sen[1].value = 1023 - analogRead(1);
  IR_Sen[2].value = 1023 - analogRead(2);
  IR_Sen[3].value = 1023 - analogRead(3);
  IR_Sen[4].value = 1023 - analogRead(4);
  IR_Sen[5].value = 1023 - analogRead(5);
  IR_Sen[6].value = 1023 - analogRead(8);
  IR_Sen[7].value = 1023 - analogRead(9);


  Serial.println(BallDistance);
}