//#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>

/* Constants */
const char rs = 13;
const char en = 12;
const char d4 = 11;
const char d5 = 5;
const char d6 = 4;
const char d7 = 3;
const char tempOffset = 5;
const String password = "1234";
const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {10, 7, A1, 0}; 
byte colPins[COLS] = {A2, A3, A4, A5}; 

/* Variables */
double temp;
double pastValue;
unsigned int motorSpeed = 250;
unsigned int tempControl = 22;
bool pin2State = false;
bool doorState = false;

/* Instances */
LiquidCrystal lcd = LiquidCrystal(rs, en, d4, d5, d6, d7);
Servo servo = Servo();
Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void displayTemperature() {
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  temp = analogRead(A0);
  temp = temp*5000/1024;
  temp = (temp - 500)/10;
  lcd.print(temp);
  lcd.print(" C ");
}

void coolingSystem() {
  lcd.setCursor(0,1);
  if (temp > tempControl + tempOffset && pastValue < temp) {
    digitalWrite(8,HIGH);
    lcd.print("COOLING ON ");
  } else 
  if (temp < tempControl - tempOffset && pastValue > temp) {
      digitalWrite(8,LOW);
      lcd.print("WARMING ON");
  } 
  pastValue = temp;
}

void doorSystem() {
  String key = "0000";
  unsigned int i = 0;
  char digit;

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter the pin!");
  lcd.setCursor(0,1);
  while (i < password.length()) {
    digit = keypad.getKey();
    if (digit) {
      key[i] = digit;
      Serial.print(digit);
      lcd.print(digit);
      i++;
    }
  }

  if (key.equals(password)) {
    lcd.setCursor(0,0);
    if (doorState == false) {
      lcd.print("Door is opening");
      servo.write(90);
      doorState = true;
    } else {
      lcd.print("Closing door");
      servo.write(0);
      doorState = false;
    }    
    delay(1500);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Welcome!");
    delay(1000);
  } else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wrong pin!");
    delay(1500);
  }
}

ISR(INT0_vect) {
  doorSystem();
}

void interruptInit() {
  SREG |= (1<<7); //global interrupt enable
  EIMSK |= 1; // INT0 pin 2 interrupt enable
  EICRA |= 0b0000011; // interrupt on rising edge
}

inline void lockSystem() {
  if (digitalRead(2) == HIGH) {
    if (pin2State == false) {
      pin2State = true;
      doorSystem();
    }
  } else {
    pin2State = false;
  }
  lcd.setCursor(0,1);
  if (digitalRead(8) == HIGH) {
    lcd.print("COOLING ON ");
  } else {
    lcd.print("WARMING ON");
  }
}

void setup() {
  // IO pins init
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(2,INPUT);
  pinMode(1,INPUT);
  digitalWrite(8,LOW);
  // interruptInit();

  // lcd init
  lcd.begin(16,2);

  // servo init
  servo.attach(9);
  servo.write(0);

  // analog read init
  pinMode(A0, INPUT);

  // PWM
  analogWrite(6,motorSpeed);
  Serial.begin(9600);
  //doorSystem();
  lcd.setCursor(0,1);
  lcd.print("WARMING ON");
}

void loop() {
  displayTemperature();
  coolingSystem();
  lockSystem();
}