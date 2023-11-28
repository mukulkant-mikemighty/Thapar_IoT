#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>

#include <Arduino.h>
#include <Firebase_ESP_Client.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"


#define RXp2 16
#define TXp2 17

#define SS_PIN  21  /*Slave Select Pin*/
#define RST_PIN 22  /*Reset Pin for RC522*/
#define LED_G   12   /*Pin 8 for LED*/
// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2


#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"/////////////////////////do
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"/////////////////do
#define API_KEY "REPLACE_WITH_YOUR_FIREBASE_PROJECT_API_KEY"///////do
#define DATABASE_URL "REPLACE_WITH_YOUR_FIREBASE_DATABASE_URL"/////do





//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);   /*Create MFRC522 initialized*/

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int board_id;
  float per_unit_weight;
  unsigned int reading_id;
  char rfid_id[9];
  //String rfid_id;
  //String rfid_id;
} struct_message;
//Create a struct_message called myData
struct_message myData;

unsigned int reading_id = 0;

/////////
/////////



void readCard() {
  /*Show UID for Card/Tag on serial monitor*/
  Serial.print("UID tag: ");
  char content[9]; 
  //String content="";
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;
  
  for (byte i=0; i < mfrc522.uid.size; i++) {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    myData.rfid_id[i] = mfrc522.uid.uidByte[i];
    Serial.printf("%x",mfrc522.uid.uidByte[i]);
  }
  Serial.print("\n");
  for(byte i=0; i < mfrc522.uid.size; i++){
    Serial.printf("%x",myData.rfid_id[i]);
  }
}



void setup()
{
  Serial.begin(9600);   /*Serial Communication begin*/
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  
  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  
  
  
  SPI.begin();          /*SPI communication initialized*/
  mfrc522.PCD_Init();   /*RFID sensor initialized*/
  pinMode(LED_G, OUTPUT);  /*LED Pin set as output*/
  
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  
  
  
  
  
  
  ////////
  ////////
  
  Serial.println("Put your card to the reader...");
  Serial.println();

}
void loop()
{
  
  ////////
  ////////
  myData.board_id = BOARD_ID;
  
  Serial.print("Weight: "+Serial2.readString());
  //Serial.print();
  Serial.println(" ");
  readCard();
  //Serial.printf(" "+readCard());
  
  Serial.println(" ");
  float moisture= analogRead(2);
  Serial.println(moisture);

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    if (Firebase.RTDB.setInt(&fbdo, "test/board_id", BOARD_ID)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setString(&fbdo, "test/weight", Serial2.readString())) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }


    if (Firebase.RTDB.setFloat(&fbdo, "test/moisture", moisture)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }


  }

  reading_id++;
  myData.reading_id=reading_id;

  if (Firebase.RTDB.setInt(&fbdo, "test/reading_id", myData.reading_id)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }

}
