#include <LiquidCrystal.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// Define the pins for the GPS module
#define GPS_RX_PIN 8
#define GPS_TX_PIN 9

// Define the pins for the GSM module
#define GSM_RX_PIN 10
#define GSM_TX_PIN 11

SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);
SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN);
TinyGPSPlus gps;
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

void displayGPSData();
void sendLocationSMS();
void print2digits(int number);
void checkIncomingSMS(); 

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  gsmSerial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("GPS Tracker");
  delay(1000);
}

void loop() {
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      displayGPSData();
      checkIncomingSMS(); 
      delay(30000); // Delay for 30 seconds before reading the next GPS data
    }
  }

  // Print "NO SIGNALS" if no GPS Signal Found
  if (gps.charsProcessed() < 10) {
    Serial.println("NO SIGNALS");
    delay(10000);
  }
}

void displayGPSData() {
  if (gps.location.isUpdated()) {
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    // Printing on Serial Monitor
    Serial.print(" Latitude= ");
    Serial.print(latitude, 6);
    Serial.print(" Longitude= ");
    Serial.print(longitude, 6);
    Serial.print(" Date: ");
    print2digits(gps.date.month());
    Serial.print("/");
    print2digits(gps.date.day());
    Serial.print("/");
    Serial.print(gps.date.year());
    Serial.print("  Time: ");
    print2digits(gps.time.hour());
    Serial.print(":");
    print2digits(gps.time.minute());
    Serial.print(":");
    print2digits(gps.time.second());
    Serial.println();
    // Print Google Maps location link on serial monitor
    Serial.print("Google Maps Location: https://maps.google.com/?q=");
    Serial.print(gps.location.lat(), 6);
    Serial.print(",");
    Serial.println(gps.location.lng(), 6);
    
    // Number of satellites in use
    Serial.print("Number of satellites in use = ");
    Serial.println(gps.satellites.value());

    // Printing Latitude and Longitude on LCD Display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Latitude: " + String(latitude, 6));
    lcd.setCursor(0, 1);
    lcd.print("Longitude: " + String(longitude, 6));
    delay(10000); // Delay for 10 seconds before updating LCD again
  }
}

void checkIncomingSMS() {
  if (gsmSerial.available()) {
    String incomingSMS = gsmSerial.readStringUntil('\n');
    incomingSMS.trim();

    if (incomingSMS.indexOf("location") != -1) {
      sendLocationSMS();
    }
  }
}

void sendLocationSMS() {
  gsmSerial.println("AT+CMGF=1"); // Setting SMS mode to text
  delay(1000);
  gsmSerial.println("AT+CMGS=\"+91XXXXXXXXX\""); // Phone number
  delay(1000);
  gsmSerial.print("Location: ");
  gsmSerial.print(gps.location.lat(), 6);
  gsmSerial.print(", ");
  gsmSerial.print(gps.location.lng(), 6);
  gsmSerial.write(26);
  delay(10000); // Delay for 10 seconds
}
void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}
