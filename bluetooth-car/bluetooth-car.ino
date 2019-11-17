#define DEBUG

#define ENA_PIN 10
#define ENB_PIN 11
#define IN1_PIN 3
#define IN2_PIN 2
#define IN3_PIN 5
#define IN4_PIN 4
#define BT_RX_PIN 6
#define BT_TX_PIN 7
#define BUZZ_PIN 13

#define CRS_PIN A0
#define CRS_BASE 512
#define CRS_PCT 25

#define MIN_SPEED 64
#define MAX_SPEED 255

#include <SoftwareSerial.h>
SoftwareSerial bt(BT_RX_PIN, BT_TX_PIN);

void setup() {
  pinMode(ENA_PIN, OUTPUT);
  pinMode(ENB_PIN, OUTPUT);
  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  pinMode(BT_RX_PIN, INPUT);
  pinMode(BT_TX_PIN, OUTPUT);

  Serial.begin(9600);
  bt.begin(9600);

  info("Initialization complete ;)");
}

byte _spd = MAX_SPEED;
byte _spdc = 10;

char btcmd;

const unsigned int buzzint = 100;
const unsigned int buzzfreq = 1000;
bool buzzer = false;
bool buzzac = true;
unsigned long buzztime = millis();

int crs;
byte cR;
byte cL;

void loop() {
  if (bt.available()) {
    btcmd = (char)bt.read();

    _spd = calculateSpeed(_spdc);

    switch (btcmd) {
      case 'D':
        fullStop();
        break;
      case 'S':
        fullStop();
        break;
      case 'F':
        crs = analogRead(CRS_PIN);
        cR = crs2cR(crs);
        cL = crs2cL(crs);
#ifdef DEBUG
        Serial.print("CRS: "); Serial.print(crs);  Serial.print(" => "); Serial.print(cR); Serial.print(" / "); Serial.println(cL);
#endif
        goForward(_spd);
        break;
      case 'I':
        goForwardRight(_spd);
        break;
      case 'G':
        goForwardLeft(_spd);
        break;
      case 'B':
        goBackward(_spd);
        break;
      case 'J':
        goBackwardRight(_spd);
        break;
      case 'H':
        goBackwardLeft(_spd);
        break;
      case 'R':
        turnRight(_spd);
        break;
      case 'L':
        turnLeft(_spd);
        break;
      case '0':
        _spdc = 0;
        break;
      case '1':
        _spdc = 1;
        break;
      case '2':
        _spdc = 2;
        break;
      case '3':
        _spdc = 3;
        break;
      case '4':
        _spdc = 4;
        break;
      case '5':
        _spdc = 5;
        break;
      case '6':
        _spdc = 6;
        break;
      case '7':
        _spdc = 7;
        break;
      case '8':
        _spdc = 8;
        break;
      case '9':
        _spdc = 9;
        break;
      case 'q':
        _spdc = 10;
        break;
      case 'V':
        buzzer = true;
        break;
      case 'v':
        buzzer = false;
        break;
      default:
        Serial.print("Unhandled command: "); Serial.println(btcmd);
        break;
    }
  }

  if (buzzer) {
    if (millis() - buzztime > buzzint) {
      buzztime = millis();
      buzzac = !buzzac;
    }

    if (buzzac) {
      tone(BUZZ_PIN, buzzfreq);
    } else {
      noTone(BUZZ_PIN);
    }
  } else {
    noTone(BUZZ_PIN);
  }

  delay(10);
}

void info(String txt){ 
  Serial.print("INFO: "); Serial.println(txt);
}

void goForward(byte mspd) {
  byte rspd = cR * mspd / 100;
  byte lspd = cL * mspd / 100;

  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, rspd);
  analogWrite(ENB_PIN, lspd);

#ifdef DEBUG
  Serial.print("R Speed: "); Serial.print(rspd); Serial.print(" / L Speed: "); Serial.println(lspd);
#endif
}

void goForwardRight(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, 0);
}

void goForwardLeft(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, mspd);
}

void goBackward(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void goBackwardRight(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, 0);
}

void goBackwardLeft(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, mspd);
}

void turnRight(byte mspd) {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, HIGH);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void turnLeft(byte mspd) {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
  analogWrite(ENA_PIN, mspd);
  analogWrite(ENB_PIN, mspd);
}

void fullStop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  analogWrite(ENA_PIN, 0);
  analogWrite(ENB_PIN, 0);
}

byte calculateSpeed(byte spdc) {
  if (spdc == 10) {
    return MAX_SPEED;
  }

  byte cspd = MIN_SPEED + (MAX_SPEED - MIN_SPEED) / 10 * spdc;

#ifdef DEBUG
  Serial.print("Speed: "); Serial.print(spdc); Serial.print(" / "); Serial.println(cspd);
#endif

  return cspd;
}

byte crs2cR(int crs) {
  if (crs >= 500) {
    crs = CRS_BASE;
  }

  float q = CRS_BASE - crs;
  float p = q / CRS_BASE * CRS_PCT;

  return 100 - p;
}

byte crs2cL(int crs) {
  if (crs < 524) {
    crs = CRS_BASE;
  }

  float q = crs - CRS_BASE;
  float p = q / CRS_BASE * CRS_PCT;

  return 100 - p;
}