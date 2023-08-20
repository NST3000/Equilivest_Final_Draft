#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;


#include <ESP8266WiFi.h>
//#include <WiFi.h>
#include <WiFiUdp.h>
WiFiUDP Udp;

/////////////////// - MODO - //////////////////////
//boolean inclinacion = false;
//////////////////////////////////////////////////

/////////////// VARIABLES ///////////////////////

////////////// INCLINACION //////////////////////

unsigned long toff1 = 15;
unsigned long ton1 = 80;
unsigned long ton2 = 100;
unsigned long ton3 = 150;
unsigned long ton4 = 200;
unsigned long ton5 = 250;

/////////////////////////////////////////////////

////////////////// 10 M WALK /////////////////////

unsigned long tref = 550;  // tiempo en que puede volver a vibrar post ultima
unsigned long tvibr = 260; // tiempo de vibracino

/////////////////////////////////////////////////
char buff1[8];
char buff2[8];
char buff3[8];
char buff4[8];
char buff5[8];
char buff6[8];
char buff7[8];

String yaw1;
String roll1;
String pitch1;
String gyroy1;
String gyrox1;
String gyroz1;
String msg;

int counter;
String count;

// Network SSID
const char* ssid = "XXXXX";

const char* password = "XXXXXXX";

unsigned int localPort = 20001;

const char* ip = "XXXXXXXX";


float pitch12;
float roll12;

struct sensor {
  float yaw;
  float pitch;

  float roll;
  float gyrox;
  float gyroy;
  float gyroz;
  int counter;
};

boolean debug = false;


boolean on = false;
unsigned long tiempo_cambio = millis();
unsigned long tiempo_vib = millis();
unsigned long ton = 0;
unsigned long toff = 0;


boolean ref = false;
boolean change_side = true;
char side;
char last_side;


///////////////////////////////////////////////////////////
const int buttonPin = 14;  // Change this to match the pin you connected the button to
boolean inclinacion = 0;
boolean lectura = 0;
boolean buttonRested = 0;
boolean buttonActive = 0;
///////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname("Name");
  // WiFi.begin(ssid, password);
  ///////////////////////////////////////////////// - change to work offline
  int retries = 0;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    // WiFi.begin(ssid, password);
    delay(400);
    Serial.print(".");
    retries++;
    if (retries >= 30) {
      Serial.println("");
      Serial.println("Failed to connect to WiFi. Working offline...");
      break;  // Break out of the connection loop and work offline
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");

    // Print the IP address
    Serial.print("IP address: ");
    Serial.print(WiFi.localIP());

  }
  Udp.begin(localPort);

  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while (status != 0) { } // stop everything if could not connect to MPU6050

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  //  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  pinMode(12, OUTPUT);


  pinMode(buttonPin, INPUT);  // Set the button pin as input with internal pull-up resistor
}

void loop() {
  struct sensor mySensor;
  mpu.update();
  static uint32_t prev_ms = millis();
  //if (millis() > prev_ms + 50) { // print every 25ms
  if (millis() > prev_ms + 30) {
    if ((millis() - timer) > 10) { // print data every 10ms ------ does nothing
      mySensor.yaw = mpu.getAngleZ();
      mySensor.roll = mpu.getAngleX();
      mySensor.pitch = mpu.getAngleY();
      mySensor.gyrox = mpu.getGyroX();
      mySensor.gyroy = mpu.getGyroY();
      mySensor.gyroz = mpu.getGyroZ();
      mySensor.counter = counter;

      //////////////////////////////////////////////////////////////////////////////////////////
      // Read the button state
      lectura = !digitalRead(buttonPin);
      if (lectura == 1) {
        buttonActive = 1;
      }
      else {
        buttonRested = 1;
      }
      if ( buttonActive == 1 && buttonRested == 1) {
        inclinacion = !inclinacion;

        buttonRested = 0;
      }

    //  Serial.print("button active: ");
    //  Serial.println(buttonActive);
    //  Serial.print("button rested: ");
    //  Serial.println(buttonRested);
    //  Serial.print("lectura: ");
    //  Serial.println(lectura);
    //  Serial.println(" ");

      buttonActive = 0;
      ///////////////////////////////////////////////////////////////////////////////////////

      if (debug) {
        Serial.print("X : ");
        Serial.print(mySensor.roll);
        Serial.print("\tY : ");
        Serial.print(mySensor.pitch);
        Serial.print("\tZ : ");
        Serial.print(mySensor.yaw);
        Serial.print("      ");
        Serial.print("GYROX: ");
        Serial.print(mySensor.gyrox);
        Serial.print("\tY: ");
        Serial.print(mySensor.gyroy);
        Serial.print("\tZ: ");
        Serial.print(mySensor.gyroz);
        Serial.print("   ");
        Serial.println(mySensor.counter);

      }
      pitch12 = -mySensor.roll;
      roll12 = mySensor.pitch;

      pitch1 = dtostrf(-mySensor.roll, 1, 2, buff1);
      roll1 = dtostrf(mySensor.pitch, 1, 2, buff2);
      yaw1 = dtostrf(mySensor.yaw, 1, 2, buff3);

      gyroy1 = dtostrf(mySensor.gyrox, 1, 2, buff4);
      gyrox1 = dtostrf(mySensor.gyroy, 1, 2, buff5);
      gyroz1 = dtostrf(mySensor.gyroz, 1, 2, buff6);

      count = dtostrf(mySensor.counter, 1, 2, buff7);

      msg = pitch1 + '=' + roll1 + '=' + yaw1 + '=' + gyroy1 + '=' + gyrox1 + '=' + gyroz1 + '=' + count;
      Serial.println(msg);
     // Serial.println(inclinacion);
      //Serial.println(" ");
      //Serial.println(" ");
      //Serial.println(roll12);

      // aca tengo que modificar
      if (counter < 100) {
        counter = counter + 1;
      } else {
        counter = 0;
      }


      Udp.beginPacket(ip, localPort);
      Udp.print(msg);
      Udp.endPacket();
      prev_ms = millis();
      unsigned long tiempo = millis();
      if (inclinacion) {

        if ( (pitch12 < 3) &&  (pitch12 > -3)) {
          digitalWrite(12, LOW);
          on == false;
          tiempo_cambio = millis();
          ton = 0;
          toff = 0;
        }

        /////     //digitalWrite(12, LOW);

        if ( (pitch12 >= 3) || (pitch12 <= -3)) {
          //Serial.println("linea 200");

          if ( ((pitch12 >= 3) && (pitch12 < 5)) || ((pitch12 > -5) && (pitch12 <= -3))) {
            //if ((15 > pitch12 >= 10) || (-10 >= pitch12 > -15)) {
            ton = ton1;
            toff = toff1;
          }
          if ( ((pitch12 >= 5) && (pitch12 < 10)) || ((pitch12 > -10) && (pitch12 <= -5))) {
            //if ((30 > pitch12 >= 15) || (-15 >= pitch12 > -30)) {
            ton = ton2;
            toff = toff1;
          }
          if ( ((pitch12 >= 10) && (pitch12 < 15)) || ((pitch12 > -15) && (pitch12 <= -10))) {
            //if ((40 > pitch12 >= 30) || (-30 >= pitch12 > -40)) {
            ton = ton3;
            toff = toff1;
          }
          if ( ((pitch12 >= 15) && (pitch12 < 20)) || ((pitch12 > -20) && (pitch12 <= -15))) {
            //if ( (50 > pitch12 >= 40 ) || ( -40 >= pitch12 > -50)) {
            ton = ton4;
            toff = toff1;
          }
          if ( (pitch12 >= 20) || (pitch12 <= -20)) {
            //if (pitch12 >= 50 || -50 >= pitch12) {
            ton = ton5;
            toff = toff1;
          }

          if (on == false) {
            if ((tiempo - tiempo_cambio) > toff) {
              digitalWrite(12, HIGH);
              on = true;
              tiempo_cambio = millis();
            }
          }
          if (on == true) {
            if ((tiempo - tiempo_cambio) > ton) {
              digitalWrite(12, LOW);
              on = false;
              tiempo_cambio = millis();
            }
          }

        }

        //Serial.println(tiempo);
        //Serial.println(tiempo_cambio);
        //Serial.println(on);
        //Serial.println(ton);
        //Serial.println(toff);

      }
      else {

        if ((tiempo - tiempo_vib) > tref) {
          //if ((tiempo - tiempo_vib) > 700) {
          ref = false;
        }
        if (roll12 > 0) {
          side = 'r';

        } else {
          side = 'l';
        }
        if (side != last_side) {
          change_side = true;
        }
        if (ref == false) {
          if (roll12 > 3.7 || roll12 < -3.7) {
            if (change_side == true) {
              digitalWrite(12, HIGH);
              last_side = side;
              ref = true;
              tiempo_vib = millis();
              change_side = false;
            }
          }

        }
        if ((tiempo - tiempo_vib) > tvibr) {
          digitalWrite(12, LOW);

        }
        // Serial.println(tiempo);
        // Serial.println(tiempo_vib);
        // Serial.println(ref);
        // Serial.println(side);

      }


    }
  }


}
