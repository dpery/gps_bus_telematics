#include <TinyGPS++.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <WiFiS3.h>
#include "pitches.h"
#include <MPU6050_light.h>
#define gpsSerial Serial1 // Serial communication for gps
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD address to 0x27 for a 16 chars and 2 line display
MPU6050 mpu(Wire);// Create a MPU6050 object used for <MPU6050_light.h>
TinyGPSPlus gps;  // Create a GPS object used for <TinyGPS++.h>
WiFiClient client;// Create a client object used for wifi
char HOST_NAME[] = "tdmeng.gr"; //Name of the server
const char ssid[] = "TheMachine"; //SSID of the wifi
const char pass[] = "8ct7gd1ea9kf7"; //Password of the wifi
const int buzzer = 2; //Buzzer pin on the board
int status = WL_IDLE_STATUS;  //Starting wifi status
int HTTP_PORT = 8090; //Http port of the server
bool lHighSpeedActive; //Logic variable for the speedlimit
bool lAlarmActive; //Logic variable for the speedlimit
bool ColissionDetected = false; //Declare a boolean value for collision detection
float axex; //Declare a float value to store the x acceleration
float axey; //Declare a float value to store the y acceleration
float axez; //Declare a float value to store the z acceleration
String HTTP_METHOD = "POST"; //Method of http communication
String PATH_NAME = "/api/v1/9djp2lmxdOxyqxWki43W/telemetry"; //Path of the thingsboard server
String queryString; 


/*NOTES of hardware pins (Arduino uno r4 wifi) 
BUTTON = DPIN5
BUZZER = DPIN2
LCD SCREEN:
A4 = SDA
A5 = SCL
///////////// 
GPS:
TX = RX
RX = TX */


// Melody and note durations
int melody[] = {
  NOTE_AS4, NOTE_AS4, NOTE_AS4,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_F5, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,

  NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F6, NOTE_C6,
  NOTE_AS5, NOTE_A5, NOTE_AS5, NOTE_G5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,

  NOTE_C6, NOTE_G5, NOTE_G5, REST, NOTE_C5,
  NOTE_D5, NOTE_D5, NOTE_AS5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_F5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_D5, NOTE_E5, NOTE_C6, NOTE_C6,
  NOTE_F6, NOTE_DS6, NOTE_CS6, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_F5,
  NOTE_C6
};


int durations[] = {
  8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 8, 8,
  2, 2,
  8, 8, 8, 2, 4,

  8, 8, 8, 2, 4,
  8, 8, 8, 2, 8, 16,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 8, 8, 8, 8,

  8, 16, 2, 8, 8,
  4, 8, 8, 8, 8, 8,
  8, 8, 8, 4, 8, 4, 8, 16,
  4, 8, 4, 8, 4, 8, 4, 8,
  1
};



void displayInfo() { //Function to display data on the 20x4 lcd screen
 // Serial.println("Updating location");
  if (gps.location.isValid()) {
    lcd.setCursor(0, 0); //Set cursor at 0,0 of screen
    lcd.print("Lat: "); //Print in serial "Lat: "
    lcd.print(gps.location.lat(), 8);//Print the latitude on the screen
    lcd.setCursor(0, 1); //Set cursor at 0 of second line
    lcd.print("Lon: "); //Print in serial "Lon: "
    lcd.print(gps.location.lng(), 8);//Print the longitude ont the screen
    lcd.setCursor(0, 2); //Set cursor at 0 of third line
    lcd.print("Satellites: "); //Print in serial "Satellites: "
    lcd.print(gps.satellites.value());//Print the number of satellites on the screen
    lcd.setCursor(0, 3); //Set cursor at 0 of forth line
    lcd.print("Speed: "); //Print in serial "Speed: "
    lcd.print(gps.speed.kmph()); //Print the speed on the screeen

  } else {

    Serial.println("Location: Not Available");//Serial print not available
  }
  //delay(1000);  // Adjust if necessary
}


void axe(){     
  mpu.update(); // Update the accelerometer 
  axex = mpu.getAccX(); //Save the value of x acceleration
  axey = mpu.getAccY(); //Save the value of y acceleration
  axez = mpu.getAccZ(); //Save the value of z acceleration
  // Serial.print(F("ACCELERO  X: "));Serial.print(axex); //Print in serial ACCELERO  X:  	 
  // Serial.print("\tY: ");Serial.print(axey);             //  Y:
  // Serial.print("\tZ: ");Serial.println(axez);           //  Z:
  if((abs(axex) > 2) || (abs(axey) >2)) { // If acceleration of x or y axis is more than 2G then show it crashed 
    //Colission detected
    ColissionDetected = true; //Change the boolean value of ColissionDetected
    Serial.println("ALERT!!!! COLISSION DETECTED");//Print in serial "ALERT!!!! COLISSION DETECTED"
  }
}


void dispatchData() { //Function for sending data on the server as a http post
  JsonDocument doc; //Create a doc object
  doc["lat"] = gps.location.lat(); //Latitude
  doc["lon"] = gps.location.lng(); //Longitude
  doc["sats"] = gps.satellites.value(); //Number of satellites
  doc["speed"] = gps.speed.kmph(); //Speed
  if (ColissionDetected)  //If collision is detected
    doc["colission"] = 1; //Send in the server that collision is detected
  else                    //Or else 
    doc["colission"] = 0; //Don't send collision is detected

  queryString = "";
  serializeJson(doc, queryString);//

  if (client.connect(HOST_NAME, HTTP_PORT)) { //Connect to webserver and in which port
    Serial.println("Connected to server");  //If connected print in serial "connected to server"
    // 
    client.println(HTTP_METHOD + " " + PATH_NAME + " HTTP/1.1"); //Make a HTTP request
    client.println("Host: " + String(HOST_NAME));
    client.println("content-type: application/json");
    client.println("Content-Length: " + String(queryString.length()));
    client.println("Connection: close");
    client.println();
    client.println(queryString);

    while (client.connected()) {
      if (client.available()) {
        // read an incoming byte from the server and print it to serial monitor:
        char c = client.read();
        //Serial.print(c);
      }
    }

    // the server's disconnected, stop the client:
    client.stop();
    Serial.println();
    Serial.println("disconnected");
  } else {  // if not connected:
    Serial.println("connection failed");
  }
}


void play() {  //  Function to play star wars music
  int size = sizeof(durations) / sizeof(int);
  for (int note = 0; note < size; note++) {
    //To calculate the note duration, take one second divided by the note type.
    //E.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int duration = 1000 / durations[note];
    tone(buzzer, melody[note], duration);
    //To distinguish the notes, set a minimum time between them.
    //The note's duration + 30% seems to work well:
    int pauseBetweenNotes = duration * 1.30;
    delay(pauseBetweenNotes);//Pause between notes
    noTone(buzzer); // Stop the tone playing
  }
}


void setup() {
  lcd.init();                     // Initialize the lcd
  lcd.backlight();                // Turn on the LCD screen backlight
  pinMode(buzzer, OUTPUT);        // Set buzzer pin as an output
  pinMode(5, INPUT_PULLUP);       // Button pin as pull_up
  Serial.begin(9600);             // Start the Arduino hardware serial port at 9600 baud
  gpsSerial.begin(9600);          // Start the software serial port at the GPS's default baud
  lcd.setCursor(0, 0);            // Set cursor at 0,0 of screen
  lHighSpeedActive = false;       // Set initial logic variable of speed as false
  lAlarmActive = false;           // Set initial logic variables of alarm as false
  Wire.begin();                   // Initialize the wire library
  byte i2cstatus = mpu.begin();   //
  while(i2cstatus!=0){ }          // While mpu.begin on not
  mpu.setAccConfig(2);            // Set acceleration measurements to +-8g
  mpu.calcOffsets(true,true);     // Gyro and accelero
  Serial.println("Done!\n");      // Print in serial that setup is done

}

void loop() {

  if (status != WL_CONNECTED) {  //If the status is not CONNECTED
    while (status != WL_CONNECTED) { //Do until the status is CONNECTED
      Serial.println("Attempting to connect to SSID: "); //Print in serial that the device is trying to connect
      lcd.clear(); //Clear the screen
      lcd.setCursor(0, 0); //Set cursor at 0,0 of screen
      lcd.print("Connecting..."); //Print "connecting" when the wifi connection is not established
      Serial.println(ssid); //Print in serial the ssid
      
      play(); //Execute the play function to play music
      status = WiFi.begin(ssid, pass); //Chagne the status of the wifi
    }

    lcd.clear(); //Clear the screen
    lcd.setCursor(0, 0); //Set cursor at 0,0 of screen
    lcd.print("Connected"); //Print "connected" when the wifi connection is established
  }
  
  axe();//Accelerometer function
  
  if (gpsSerial.available() > 0) {        // If gps serial is available 
    if (gps.encode(gpsSerial.read())) {   // Read the gps data
      displayInfo();                      // Function to display the data on the lcd screen
      dispatchData();                     // Function to send the data to the server
    }
  }


  if (digitalRead(5) == LOW) {  // If the button is pressed print pressed in serial
    Serial.println("/////////////////////////////////////////////Button_Pressed ///////////////////////////////////////////////////////"); // Show in serial that the button is pressed
    lAlarmActive = false; // Deactivate the alarm
  }

  if ((gps.speed.kmph() > 50) && (!lHighSpeedActive)) { //If you exceed the speed limit 
    lHighSpeedActive = true; //Set variable for speeding true
    lAlarmActive = true; // Activate the alarm
  }

  if (gps.speed.kmph() < 50) {  //If you under the speed limit
    lHighSpeedActive = false; //Set variable for speeding false
    lAlarmActive = false; // Deactivate the alarm
  }

  if (lAlarmActive) { //If alarm is active
    tone(buzzer, 1000); //Activate the buzzer (alarm) and play a tone of 1000hz
    Serial.println("Alarm Active"); //Print in serial "Alarm Active"
  } else { //or else
    noTone(buzzer); //Stop the buzzer(alarm)
    //Serial.println("Alarm Inactive"); //Print in serial "Alarm Inactive"
  }

  //Serial.print("Latitude: ");
  //Serial.println(gps.location.lat(), 8);
  //Serial.print("Longitude: ");
  //Serial.println(gps.location.lng(), 8);
  //Serial.print("Satellites: ");
  //Serial.println(gps.satellites.value());
  //Serial.print("Speed: ");
  //Serial.println(gps.speed.kmph());

  status = WiFi.status();  //Update the wifi status
}