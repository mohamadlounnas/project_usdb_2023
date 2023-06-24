
#include <LinkedList.h>
#include <Wire.h>
#include <Arduino.h>
#include <Servo.h>
#include <TM1637Display.h>
#include <HX711_ADC.h>
#include "segs.cpp"

// pd print debug
#define pd(x) Serial.println(x)

// state
int _balance = 20;
float _distance = 0;
float _weight = 0;
int _shashaLastCallAt = 0;
int _shashaMode = 1;
int  _thowedCounter=0;
int _deliverdCounter=0;
// ultrasonic sensor pins
const int trig_pin = 10;
const int echo_pin = 9;
// A0 for potentiometer
const int potentiometer_pin = A0;
// readDistance
float readDistance(int trigPin = trig_pin, int echoPin = echo_pin);
// mizan pins:
const int mizan_dout = 4;
const int mizan_sck = 5;
// shasha pins
const int shasha_dout = 3;
const int shasha_sck = 2;
// buzzer pin
const int buzzer = 13;
// leds pins
const int gate_servo = 6;
const int money_taker_servo = 12;
const int x_servo = 7;
const int y_servo = 8;
const int scanner_servo = 11;
// potentiometer pin
const int potentiometer = A0;
// th mode button pin (toggle), it used to change the senario
const int mode_button = 10;
// init mizan
HX711_ADC mizan(mizan_dout, mizan_sck);
// init el shasha
TM1637Display shasha(shasha_sck, shasha_dout);

// message handling
String receivedMessage;
String requestData;

// state
// servos
Servo gateServo;
Servo XServo;
Servo YServo;
Servo ScanneServo;
Servo MoneyTakerServo;

// the tesk queue
// marks
bool markNeedReset = false;
bool markNeedScan = false;
bool markNeedScanMap = false;

// play sound
void beep();

void setup()
{

  Wire.begin(8);                /* join I2C bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */

  Serial.begin(9600);
  Serial.println("Hello!");

  // init servos
  gateServo.attach(gate_servo);
  MoneyTakerServo.attach(money_taker_servo);
  XServo.attach(x_servo);
  YServo.attach(y_servo);
  ScanneServo.attach(scanner_servo);

  // potentiometer
  pinMode(potentiometer_pin, INPUT);

  // 9ala3 el mizan
  mizan.begin();
  mizan.start(1000, true);
  // configer el mizan (calibration)
  mizan.setCalFactor(-1853.39);

  // el light ta3 el shasha
  shasha.setBrightness(0x0f);
  shasha.clear();
  shasha.setSegments(SEG_ON);
  beep();
}
int timer = 0;

int mode = 0; // 0 auto, 1 manual
String currentTesk = "";
String nextTask = "reset";
// time on tesk
int teskStartAt = 0;
int teskLastCallAt = 0;
int teskCount = 0;
int teskInsureCount = 0;
int teskInsureMax = 0;

String next(String task)
{
  pd("next: " + task);
  if (task == nextTask)
  {
    teskStartAt = 0;
    teskLastCallAt = 0;
    teskCount = 0;
    teskInsureCount = 0;
  }
  nextTask = mode == 0 ? task : "";
  return nextTask;
}
bool accept(String tesk, int _delay = 1000, int max = 0)
{
  if (tesk == nextTask)
  {
    if (teskStartAt == 0)
    {
      // beep();
      teskStartAt = millis();
      teskLastCallAt = millis();
      return true;
    }
    else
    {
      teskLastCallAt = millis();
    }
    if (teskLastCallAt - teskStartAt > _delay)
    {
      teskStartAt = 0;
      teskLastCallAt = 0;
      teskCount++;
      return true;
    }
  }
  return false;
}
void repeat()
{
  teskInsureCount++;
}
int countOfBoxSuccess = 0;
int countOfBalanceSuccess = 0;
int countOfNoBoxSuccess = 0;
int countOfMizan = 0;
int averageOfMizan = 0;

int downSpeed = 0;
void loop()
{

  if (accept("reset"))
  {
    pd("reset");
    reset();
    next("take_box");
  }
  if (accept("take_box"))
  {
    pd("take_box");
    // turn the X servo to 180deg with max speed
    XServo.write(180);
    next("scan_till_box");
  }
  if (accept("scan_till_box", 100))
  {
    _shashaMode = 2; // show distance
    pd("scan_till_box:" + String(updateDistance()));
    if (_distance > 8 && _distance < 12)
    {
      _shashaMode = 0; // show nothing
      shasha.setBrightness(7);
      shasha.setSegments(SEG_GOOD);
      countOfBoxSuccess++;
      // must 5 times
      if (countOfBoxSuccess > 20)
      {
        countOfBoxSuccess = 0;
        next("goto_to_mizan_slowly");
      }
    }
    else
    {
      // brightness 30%
      shasha.setBrightness(1);
      _shashaMode = 2; // show distance
      countOfBoxSuccess = 0;
    }
  }
  // move slowly to mizan, each accept rotate 1deg, accept every 50ms
  if (accept("goto_to_mizan_slowly", 50))
  {
    MoneyTakerServo.write(180);
    pd("goto_to_mizan_slowly");
    int currentAngle = XServo.read();
    // servo can be right or left
    if (currentAngle > 0)
    {
      XServo.write(currentAngle - 1);

    }
    else
    {
      next("tare_before_balance");
    }
  }
  // tare before balance
  if (accept("tare_before_balance"))
  {
    pd("tare_before_balance");
    mizan.tare();
    next("balance");
  }
  // balance, open the gate slowly and repeat till mizan >= 20g
  if (accept("balance", 1000))
  {
    MoneyTakerServo.write(0);
    _shashaMode = 1; // show weight
    pd("balance");
    // check th _weight is >= 20g if yes close the gate and next(dilever)
    if (updateMizan() >= _balance)
    {
      gateServo.write(0);
      // take 10 mes
      for (int i = 0; i < 10; i++)
      {
        updateShasha();
        delay(100);
      }
      
      if (updateMizan() < (_balance+5))
      {
        next("dilever");
      }
      else
      {
        next("throw");
      }
    }
    else
    {
      // open the gate slowly and mesur th speed of the material get down to th box
      // speed is the angle of gate, min=0 max=90
      // speed start 0 and increesed till reach the 1/2 of our targe balance, then dicrised ass nessery
      gateServo.write(35);
      delay(_weight < (_balance/4) ? 35:25);
      gateServo.write(1);
    }
  }

  // throw
  if (accept("throw", 50))
  {
    _thowedCounter++;
    // mo x slowly 90deg, after that move to 50 ofr 1sec
    int xPos = XServo.read();
    if (xPos > 90)
    {
      XServo.write(xPos - 1);
    }
    if (xPos < 90)
    {
      XServo.write(xPos + 1);
    }
    else
    {
      int yPos = YServo.read();
      YServo.write(135);
      delay(200);
      YServo.write(96);
      // if (yPos < 135)
      // {
      //   YServo.write(yPos + 1);
      // } else if (yPos > 135)
      // {
      //   YServo.write(yPos - 1);
      // }  
      
      //  else {
        next("take_box");
      // }
    }
  }

  if (accept("dilever", 50))
  {
    _deliverdCounter++;
    MoneyTakerServo.write(180);
    pd("take_box");
    int currentAngle = XServo.read();
    // servo can be right or left
    if (currentAngle < 180)
    {
      XServo.write(currentAngle + 1);
    }
    else
    {
      next("scan_till_no_box");
    }
  }


  if (accept("scan_till_no_box", 100))
  {
    _shashaMode = 2;
    pd("scan_till_box:" + String(updateDistance()));
    if (_distance > 12)
    {
      shasha.setBrightness(7);
      countOfNoBoxSuccess++;
      // must 5 times
      if (countOfNoBoxSuccess > 20)
      {
        countOfNoBoxSuccess = 0;
        next("scan_till_box");
      }
    }
    else
    {
      // brightness 30%
      shasha.setBrightness(1);
      _shashaMode = 2; // show distance
      countOfNoBoxSuccess = 0;
    }
  }

  // if mode is manual, use potentiometer to control the gateServo
  if (mode == 1)
  {
    int potentiometerValue = analogRead(potentiometer_pin);
    pd("potentiometerValue: " + String(potentiometerValue));
    int gateServoAngle = map(potentiometerValue, 0, 1023, 0, 180);
    gateServo.write(gateServoAngle);
  }
  updateShasha();
}
void receiveEvent(int howMany)
{
  requestData = "";
  while (0 < Wire.available())
  {
    char c = Wire.read(); /* receive byte as a character */
    requestData += c;     /* append the character to the received data */
  }
  Serial.println(requestData); /* print the received data */
}

void requestEvent()
{
  String response = "";
  String method = "";
  String args1 = "";
  String args2 = "";
  String args3 = "";

  // get method
  int index = requestData.indexOf(",");
  if (index == -1)
  {
    method = requestData;
  }
  else
  {
    method = requestData.substring(0, index);
    requestData = requestData.substring(index + 1);
    // get args1
    index = requestData.indexOf(",");
    if (index == -1)
    {
      args1 = requestData;
    }
    else
    {
      args1 = requestData.substring(0, index);
      requestData = requestData.substring(index + 1);
      // get args2
      index = requestData.indexOf(",");
      if (index == -1)
      {
        args2 = requestData;
      }
      else
      {
        args2 = requestData.substring(0, index);
        requestData = requestData.substring(index + 1);
        // get args3
        index = requestData.indexOf(",");
        if (index == -1)
        {
          args3 = requestData;
        }
        else
        {
          args3 = requestData.substring(0, index);
          requestData = requestData.substring(index + 1);
        }
      }
    }
  }

  // if upper case of method == "SERVO"
  // arg1 is the name of the servo
  // arg2 is the angle of the servo its nullable, if not found just return the current angle
  // arg3 is the speed of the servo its nullable, if not found just use max speed
  // tesk
  if (method == "TESK") {
    if (args1 == "") {
      response = nextTask;
    } else {
      nextTask = args1;
    }
  }else
  if (method == "SERVO")
  {
    // get servo name
    String servoName = args1;
    // get servo angle
    int servoAngle = args2.toInt();
    // get servo
    Servo servo;
    if (servoName == "gate")
    {
      servo = gateServo;
    }
    else if (servoName == "x")
    {
      servo = XServo;
    }
    else if (servoName == "y")
    {
      servo = YServo;
    }
    else if (servoName == "scanner")
    {
      servo = ScanneServo;
    }

    // if servo angle is not found
    if (servoAngle == 0)
    {
      response = String(servo.read());
    }
    else
    {
      if (args3 != "")
      {
        int servoSpeed = args3.toInt();
        // set speed
        int currentAngle = servo.read();
        if (currentAngle > servoAngle)
        {
          for (int i = currentAngle; i >= servoAngle; i--)
          {
            servo.write(i);
            delay(servoSpeed);
          }
        }
        else
        {
          for (int i = currentAngle; i <= servoAngle; i++)
          {
            servo.write(i);
            delay(servoSpeed);
          }
        }
      }
      else
      {
        servo.write(servoAngle);
      }
      response = "done";
    }
  }
  // read direct distance
  else if (method == "RESET")
  {
    response = String("device reseting...");
    reset();
  }
  // read direct distance
  else if (method == "MODE")
  {
    mode = args1.toInt();
    response = String("mode now is " + String(mode == 0 ? "auto" : "manual"));
  }
  // read direct distance
  else if (method == "MIZAN")
  {
    response = String(updateMizan());
  }
  // custom display number
  else if (method == "SHASHA")
  {
    // change mode to manual
    _shashaMode = 0;
    // get number
    int number = args1.toInt();
    Wire.write(String("writing on shasha.." + args1).c_str());
    // get delay
    if (args2 == "")
    {
      args2 = "1000";
    }

    // display number
    shasha.showNumberDec(number);
    return;
  }
  // shshsa mode
  else if (method == "SHASHA_MODE")
  {
    _shashaMode = args1.toInt();
    response = String("shasha mode now is ");
    if (_shashaMode == 1)
    {
      response += "mizan (g)";
    }
    else if (_shashaMode == 2)
    {
      response += "distance (cm)";
    }
    else
    {
      response += "manual";
    }
  }
  // read direct distance
  else if (method == "D")
  {
    response = String(readDistance());
  }
  // SCAN
  // our servo will run from 45 to 135, each 5deg will read the distance and return list
  else if (method == "SCAN")
  {
    // get servo
    Servo servo;
    servo = ScanneServo;
    // get speed
    int servoSpeed = args1.toInt();
    // get start angle
    int startAngle = args2.toInt();
    // get end angle
    int endAngle = args3.toInt();
    // servo start from start angle to end angle moving by5 deg each time
    for (int i = startAngle; i <= endAngle; i += 5)
    {
      servo.write(i);
      delay(servoSpeed);
      response += String(readDistance(trig_pin, echo_pin)) + ",";
    }
    // remove last comma
    response = response.substring(0, response.length() - 1);
  }

  if (response != "")
  {
    Wire.write(response.c_str());
  }
}

float microsecondsToCentimeters(long microseconds)
{
  return ((int)((microseconds / 29 / 2) * 100)) / 100;
}
// read distance
float readDistance(int trigPin = trig_pin, int echoPin = echo_pin)
{
  long duration;
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  return microsecondsToCentimeters(duration);
}

// steps
// reset, reset the system sevros and mizan taring and shasha clear
void reset()
{
  Serial.println("reseting...");
  // reset servos
  Serial.println("reseting servos...");
  gateServo.write(0);
  MoneyTakerServo.write(0);
  ScanneServo.write(90);

  YServo.write(96);
  XServo.write(0);
  // X,Y servos need to reset slowly
  // while (XServo.read() > 0)
  // {
  //   XServo.write(XServo.read() - 1);
  //   delay(100);
  // }
  // while (YServo.read() > 90)
  // {
  //   YServo.write(YServo.read() - 1);
  //   delay(100);
  // }

  // reset mizan
  Serial.println("reseting mizan...");
  mizan.tareNoDelay();
  // reset shasha
  Serial.println("reseting shasha...");
  shasha.clear();
  // shpw off
  shasha.setSegments(SEG_OFF);
  delay(1000);
  // show on
  shasha.setSegments(SEG_ON);
  delay(1000);
  shasha.clear();
}

// tesks
float updateDistance()
{
  // read distance
  _distance = readDistance();
  return _distance;
}
// update shasha
void updateShasha()
{
  if (_shashaLastCallAt == 0)
  {
    _shashaLastCallAt = millis();
  }
  else
  {
    if (millis() - _shashaLastCallAt > 100)
    {
      _shashaLastCallAt = millis();
      // on mode 1 show weight, on mode 2 show distance
      if (_shashaMode == 1)
      {
        pd("in shasha mode: " + String(_shashaMode));
        updateMizan();
        pd("in shasha _weight: " + String(_weight));
        shasha.showNumberDec(int(_weight));
      }
      else if (_shashaMode == 2)
      {
        updateDistance();
        shasha.showNumberDec(int(_distance));
      }
    }
    else
    {
    }
  }
}
// update mizan
float updateMizan()
{
  if (mizan.update())
  {
    _weight = mizan.getData();
  }
  return _weight;
}


// implementaion of sound
void beep()
{
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
  delay(100);
  tone(buzzer, 1000);
  delay(100);
  noTone(buzzer);
}