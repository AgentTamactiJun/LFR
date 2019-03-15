int L1 = 7;
int L2 = 6; //for left motor

int R2 = 4;
int R1 = 2; //for right motor

int ENR = 3; //enable right
int ENL = 5; //enable left
int numofones = 0;
float pos = 0;
float Kp = 10, Kd = 15, Ki = 8;
int flag = 0;//black
int flagwhite = 0; //white
int base_speed = 50;
int ledPin = 10;
float prev_error = 0;
unsigned int sensor[10];/*sensor readings are saved here*/
boolean firstData[10];/*to eliminate effect of noise*/
byte sensorPin[10] = {
  A1, A2, A3, A4, A5, A6, A7, A8, A9, A10
};/*arduino pins to read sensors*/
byte NumOfSensor = 10;
byte i; /*just to run for loop!!*/
unsigned int MaxWaitTime = 4096; /*equivalent to 12 bit ADC*/

void setup() {
  Serial.begin(9600);
  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(ENL, OUTPUT);
  pinMode(ENR, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.begin(9600);
}
void readSensor()
{
  for (i = 0; i < NumOfSensor; i++)
  {
    digitalWrite(sensorPin[i], HIGH);
    pinMode(sensorPin[i], OUTPUT);
  }
  delayMicroseconds(10);
  for (i = 0; i < NumOfSensor; i++)
  {
    pinMode(sensorPin[i], INPUT);
    digitalWrite(sensorPin[i], LOW);
    sensor[i] = MaxWaitTime;
    firstData[i] = false;
  }
  unsigned long startTime = micros();
  while (micros() - startTime < MaxWaitTime)
  {
    unsigned int time = micros() - startTime;
    for (i = 0; i < NumOfSensor; i++)
    {
      if ((digitalRead(sensorPin[i]) == LOW) && (firstData[i] == false))
      {
        sensor[i] = time;
        firstData[i] = true;
      }
    }
  }
  for (i = 0; i < NumOfSensor; i++)
  {
    if (sensor[i] < 200) sensor[i] = 0;   //sensing white
    else sensor[i] = 1;                   //sensing black
  }
}
void showsensorData()
{
  for (i = 0; i < NumOfSensor; i++)
  {
       Serial.print(sensor[i]);
       Serial.print(" ");
  }
   Serial.println();
}

float calculate_error()
{
  numofones=0;              // 1 0 1 1        no=3
  for (int i = 0; i < 10; i++)
  {
    numofones += sensor[i];
  }
    if (numofones == 0) return 50;
    if (numofones == 10) return -50;
  //
  float pos = 0;
  for (int i = 9; i >= 0; i--)    // 9 8 7 6 5 4 3 2 1 0
    pos += sensor[i] * i;         // 0 0 0 0 1 1 1 0 0 0     12
  pos = pos / float(numofones);       //12/3 = 4
  return pos - 4.5;                   //4-4.5=-.5

}

void pid(float error)
{
  int motor_correction = 0;
  motor_correction = int(Kp * error + Kd * (error - prev_error) + Ki * (error + prev_error));  //motorspeed is proportional to error
  int left_speed = constrain(base_speed - motor_correction , -255, 255);
  int right_speed = constrain(base_speed + motor_correction, -255, 255);
  set_motor(left_speed, right_speed);

}

void set_motor(int left, int right)    // motor direction + speed control
{                                      // left: +  forward
  analogWrite(ENL, abs(left));          //      -   backward
  analogWrite(ENR, abs(right));       // absolute value  = speed 

  if (left >= 0)
  {
    digitalWrite(L1, HIGH);
    digitalWrite(L2, LOW);
  }
  else
  {
    digitalWrite(L1, LOW);
    digitalWrite(L2, HIGH);
  }

  if (right >= 0)
  {
    digitalWrite(R1, HIGH);
    digitalWrite(R2, LOW);
  }
  else
  {
    digitalWrite(R1, LOW);
    digitalWrite(R2, HIGH);
  }

}

void sharp_Right()
{
  set_motor(100, -100);
}

void sharp_Left()
{
  set_motor(-100, 100);
}
void loop()
{

  readSensor();
  showsensorData();
  float error = calculate_error();
  if (error != 50 && error != -50)
  pid(error);

  else if(error == -50){
    sharp_Left();
  }


//    if (error == 50){
//    sharp_Right();
//  }
//  if (error == 50){
//    sharp_Right();
//  }
//   if (error >= 2){
//    sharp_Left();
//  }
//  
//  
  
//
//  else if (error > 3){
//    sharp_Right();
//  }
//  
//  Serial.print(calculate_error());
////  Serial.print(numofones);
//  Serial.println(" ");
//  delay(500);


  
//    else if (error > 6)
//    {
//      set_motor(50,255);
//      delay(100);
//    }
//    else if (error < -6)
//    {
//      set_motor(150,50);
//      delay(100);
//    }
  //
  //  else if (error == -50)
  //  {
  //    if (flag <= 1) {   //B theke D te gelo
  //      sharp_Left();
  //      delay(100);
  //      flag++;
  //    }
  //    else set_motor(0, 0);
  //  }
  //  else if (error == 50)
  //  {
  //    if (!(abs(prev_error) < 1.5)) pid(prev_error); //lose line korle
  //    // discontinuous line
  //    else if ( flagwhite == 0)
  //    {
  //      flagwhite++;
  //      pid(prev_error);
  //    }
  //    //acute angle
  //    else if (flagwhite >0)
  //    {
  //      sharp_Left();
  //      delay(250);
  //    }
  //  }

}
