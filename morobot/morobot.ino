#include <MakeblockSmartServo.h>
#include <dabble_include.h>
#include <eef.h>
#include <morobot.h>
#include <morobot_2d.h>
#include <morobot_3d.h>
#include <morobot_p.h>
#include <morobot_s_rrp.h>
#include <morobot_s_rrr.h>
#include <newRobotClass_Template.h>

#define relay_pin 6

#define scara_ref_pin 7

#define IR_1_pin 49
#define IR_2_pin 51
#define IR_3_pin 53

#define stepper_dir_pin 12  //LOW = down; HIGH = up 
#define stepper_step_pin 13

#define pi 3.14159

#define MOROBOT_TYPE   morobot_s_rrr 
#define SERIAL_PORT   "Serial2"  

MOROBOT_TYPE morobot;   // And change the class-name here

const int step_turn=840;   //Steps nötig für eine Umdrhung der Getriebewelle
const float rad_dia=12.2;   //Durchmesser der Zahnriemenscheibe

float absolute_pos=0.0;   //Absolut Position der Linearachse - muss eigenständig mit gezählt werden!! Zu achten ist dass die positive Bewegungsrichtung nach unten gerichtet ist!!

void lin_axis_ref(){
  digitalWrite(stepper_dir_pin, HIGH);
  while(digitalRead(scara_ref_pin)!=LOW){
    digitalWrite(stepper_step_pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepper_step_pin, LOW);
    delayMicroseconds(100);
  }
  delay(500);
  move_lin_axis_mm(-10.0);

  digitalWrite(stepper_dir_pin, HIGH);
  delay(500);
  while(digitalRead(scara_ref_pin)!=LOW){
    digitalWrite(stepper_step_pin, HIGH);
    delayMicroseconds(750);
    digitalWrite(stepper_step_pin, LOW);
    delayMicroseconds(750);
  }
  delay(500);
  move_lin_axis_mm(-20.0);
  morobot.releaseBreaks();
  delay(7000);
  morobot.setBreaks();
}

void move_lin_axis_mm(float move_mm){
  int steps = abs(move_mm*step_turn)/rad_dia*pi;
  
  if(move_mm<0){
    digitalWrite(stepper_dir_pin, LOW);
  }
  else if(move_mm>0){
    digitalWrite(stepper_dir_pin, HIGH);
  }
  else return;

  for(int x=0; x<steps; x++){
    digitalWrite(stepper_step_pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepper_step_pin, LOW);
    delayMicroseconds(100);
  }  
  absolute_pos=absolute_pos + move_mm;
  delay(500);
}

enum Steps{
  detect,
  position,
  suck,
  reposition,
  release,
  idle
}steps;

steps = detect;

void setup() {
  pinMode(relay_pin, OUTPUT);
  pinMode(stepper_dir_pin, OUTPUT);
  pinMode(stepper_step_pin, OUTPUT);
  
  pinMode(scara_ref_pin, INPUT_PULLUP);
  pinMode(IR_1_pin, INPUT);
  pinMode(IR_2_pin, INPUT);
  pinMode(IR_3_pin, INPUT);

  digitalWrite(relay_pin, LOW);
  digitalWrite(stepper_step_pin, LOW);

  delay(2000);
  lin_axis_ref();
  delay(1000);
  

  morobot.begin(SERIAL_PORT);
  //morobot.moveHome();
  morobot.setZero();
  morobot.setSpeedRPM(1);

}

int I1 = 0;
int I2 = 0;
int I3 = 0;

void loop() {
  switch (steps){
    case detect:
      I1 = digitalRead(IR_1_pin);
      I2 = digitalRead(IR_2_pin);
      I3 = digitalRead(IR_3_pin);

      if (I1 == HIGH || I2 == HIGH || I3 == HIGH){
        steps = positions
      }
      break;

    case position:
      if (I1 == HIGH){
        morobot.moveToPose(171.0, -26.0, 0.0);
      } else if (I2 == HIGH){
        morobot.moveToPose(171.0, 17.0, 0.0);
      } else if (I3 == HIGH){
        morobot.moveToPose(171.0, 60.0, 0.0);
      }
      steps = suck;
      break;

    case suck:
      digitalWrite(relay_pin, HIGH);
      steps = reposition;
      break;

    case reposition:
      morobot.moveToPose(65.5, 160.0, 90.0);
      steps = release;
      break;
    
    case release:
      digitalWrite(relay_pin, LOW);
      steps = idle;
      break;

    case idle:
      morobot.moveHome();
      steps = detect;
      break;
    default:
      //return error
  }
}
