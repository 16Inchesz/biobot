#include <morobot.h>
#include <morobot_s_rrr.h>
#include <Servo.h>

#define relay_pumpe 6 // LOW = an, HIGH = aus

#define scara_ref_pin 7 // LOW = gedrückt;  HIGH = lose

#define IR_1_pin 10 // rechts
#define IR_2_pin 9  // mitte
#define IR_3_pin 8  // links

#define RLED 0

#define stepper_dir_pin 12 // LOW = down; HIGH = up
#define stepper_step_pin 13
#define stepper_enable 11

#define pi 3.14159

#define MOROBOT_TYPE morobot_s_rrr
#define SERIAL_PORT "Serial2"

MOROBOT_TYPE morobot; // And change the class-name here

const int step_turn = 840;   // Steps needed for 1 rotation of the motor shaft
const float rad_dia = 17.51; // Durchmesser der Zahnriemenscheibe

float absolute_pos = 0.0; // Absolut Position der Linearachse - muss eigenständig mit gezählt werden!! Zu achten ist dass die positive Bewegungsrichtung nach unten gerichtet ist!!

String message = "";
String prev_m = "";

  
void lin_axis_ref()
{
  digitalWrite(stepper_dir_pin, HIGH);
  while (digitalRead(scara_ref_pin) != LOW)
  {
    digitalWrite(stepper_step_pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepper_step_pin, LOW);
    delayMicroseconds(100);
  }

  delay(500);
  move_lin_axis_mm(-10.0);

  digitalWrite(stepper_dir_pin, HIGH);
  delay(500);

  while (digitalRead(scara_ref_pin) != LOW)
  {
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
  morobot.setZero();
  delay(500);
}

void move_lin_axis_mm(float move_mm)
{
  int steps = abs(move_mm * step_turn) / rad_dia * pi;

  if (move_mm < 0)
  {
    digitalWrite(stepper_dir_pin, LOW);
  }
  else if (move_mm > 0)
  {
    digitalWrite(stepper_dir_pin, HIGH);
  }
  else
    return;

  for (int x = 0; x < steps; x++)
  {
    digitalWrite(stepper_step_pin, HIGH);
    delayMicroseconds(100);
    digitalWrite(stepper_step_pin, LOW);
    delayMicroseconds(100);
  }
  absolute_pos = absolute_pos + move_mm;
  delay(500);
}

void blink_RLED()
{
  for (int i = 0; i <= 2; i++)
  {
    digitalWrite(RLED, HIGH);
    delay(500);
    digitalWrite(RLED, LOW);
    delay(500);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial3.begin(9600);
  // put your setup code here, to run once:
  pinMode(relay_pumpe, OUTPUT);
  pinMode(stepper_dir_pin, OUTPUT);
  pinMode(stepper_step_pin, OUTPUT);
  pinMode(stepper_enable, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(scara_ref_pin, INPUT_PULLUP);
  pinMode(IR_1_pin, INPUT);
  pinMode(IR_2_pin, INPUT);
  pinMode(IR_3_pin, INPUT);

  digitalWrite(relay_pumpe, HIGH);

  morobot.begin(SERIAL_PORT);
  morobot.setZero();
  // morobot.moveHome();       // Move the robot into initial position
  morobot.setSpeedRPM(1);

  lin_axis_ref();
  blink_RLED();
}

void measure(){
   // float tray_dis=43;  //distnc to tray on lin axis
  morobot.setSpeedRPM(5);
  float move_distance = 90.0; // distance to move down and up

  // Move to (0, -45, 0)
  morobot.moveAngle(0, -45);
  morobot.waitUntilIsReady();

  // Move down 5mm
  move_lin_axis_mm(-move_distance);
  morobot.waitUntilIsReady();

  delay(10000);

  // Move back up 5mm
  move_lin_axis_mm(move_distance);
  morobot.waitUntilIsReady();

  morobot.moveAngle(0, 45);
  morobot.waitUntilIsReady();
}

void loop()
{

  if (Serial3.available()) {
    char receivedChar[100];
    int bytesRead = Serial3.readBytesUntil('\n', receivedChar, sizeof(receivedChar) - 1);
    receivedChar[bytesRead] = '\0';
    message = String(receivedChar);
    Serial.print("Received message: '");
    Serial.print(message);
    Serial.println("'");
  } else {
    Serial.println("Serial port3 error");
  }

  delay(1000);

  message.trim();

  if ((message.equals("1") || message.equals("2")) && !message.equals(prev_m)) {
    prev_m = message; 
    measure();
  } else if (message.equals("0")){
    Serial.println("nothing");
  }
}