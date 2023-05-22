#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define SS_PIN D4
#define RST_PIN D3

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create an instance of the MFRC522 class

void setup() {


  // Set the Watchdog Timer prescaler value to 8 seconds
  // Available prescaler options: WDTO_15MS, WDTO_30MS, WDTO_60MS, WDTO_120MS, WDTO_250MS,
  // WDTO_500MS, WDTO_1S, WDTO_2S, WDTO_4S, WDTO_8S
  wdt_enable(WDTO_8S);
  // CODE FOR PREPARING RFID READER

  Serial.begin(115200);   // Initialize serial communication
  SPI.begin();          // Initialize SPI bus
  mfrc522.PCD_Init();   // Initialize MFRC522 RFID reader
  Serial.println("Ready to scan RFID cards...");
  //----------------------------------------------------------------------------------


  // CODE FOR FIRST API ENDPOINT

  WiFi.mode(WIFI_STA); // Set the WiFi mode to station mode
  // Replace "PHONE_HOTSPOT_SSID" and "PHONE_HOTSPOT_PASSWORD" with your phone hotspot's SSID and password
  WiFi.begin("Ice Caffe", "icecaffe2019");
  while (WiFi.status() != WL_CONNECTED) { // Wait until the NodeMCU has connected to the phone hotspot
    delay(1000);
    Serial.println("Connecting to phone hotspot...");
  }
  Serial.println("Connected to phone hotspot");
  //----------------------------------------------------------------------------------
}

void loop() {

  wdt_reset();
  // Check if a new RFID card has been detected
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Get the RFID card's unique ID
    String cardID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardID += String(mfrc522.uid.uidByte[i], HEX);
    }
    cardID.toUpperCase();  // Convert the ID to uppercase letters
    Serial.println("RFID card detected: " + cardID);
    delay(1000);  // Wait for 1 second before checking for a new card
  

  //----------------------------------------------------------------------------------
   WiFiClient client;
   HTTPClient  http;

   http.begin(client,"http://192.168.1.19:7004/api/scan/" + cardID); // TODO: connect to local wifi

   int httpCode = http.GET();
   Serial.println(httpCode);

   if(httpCode==200){
     String response = http.getString();
     Serial.println(response);
   }
   else{
     Serial.println("CARD CANNOT BE AUTHORIZED!");
   }
  }
}
