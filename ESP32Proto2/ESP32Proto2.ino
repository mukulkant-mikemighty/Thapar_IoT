//#include <esp_now.h>
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
#define BOARD_ID  2
#define Moisture_PIN 2


#define WIFI_SSID "mikemighty002"/////////////////////////do
#define WIFI_PASSWORD "wewillrockyou020"/////////////////do
#define API_KEY "AIzaSyCYQbeu1lp0lBBBwaTL1sBf7Y78a8S9Hiw"///////do
#define DATABASE_URL "https://farmer-2c012-default-rtdb.firebaseio.com/"/////do//https://farmer-2c012-default-rtdb.firebaseio.com/





//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;
int i=0;
MFRC522 mfrc522(SS_PIN, RST_PIN);   /*Create MFRC522 initialized*/

//Structure example to send data
//Must match the receiver structure
typedef struct struct_message {
  int board_id;
  int weight;
  int moisture;
  unsigned int reading_id;
  long int rfid_id;
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
  char rfid_id[9]; 
  //String content="";
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) return;
  
  for (byte i=0; i < mfrc522.uid.size; i++) {
    //Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    rfid_id[i] = mfrc522.uid.uidByte[i];
    Serial.printf("%x",mfrc522.uid.uidByte[i]);
  }
  Serial.print("\n");
  for(byte i=0; i < mfrc522.uid.size; i++){
    Serial.printf("%x",rfid_id[i]);
  }
  //delay(2000);
  //return rfid_id;
  
  int result=0;
  for (int i=0; i<mfrc522.uid.size; i++) {
        if (rfid_id[i]>=48 && rfid_id[i]<=57)
        {
            result += (rfid_id[i]-48)*pow(16,mfrc522.uid.size-i-1);
        } else if (rfid_id[i]>=65 && rfid_id[i]<=70) {
            result += (rfid_id[i]-55)*pow(16,mfrc522.uid.size-i-1);
        } else if (rfid_id[i]>=97 && rfid_id[i]<=102) {
            result += (rfid_id[i]-87)*pow(16,mfrc522.uid.size-i-1);
        }
    }
    myData.rfid_id=result;
}
/*
char path_rfid(){
  char path_rfid=("Farmer/userDataRecords%d/RFID",i);
  i++;
  return path_rfid;
} 
*/

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
  auth.user.email = "akathuria_be20@thapar.edu";
  auth.user.password = "btsjungkook";
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  //int p=0;
  //char path_rfid="";


  
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
  //myData.board_id = myData.board_id.c_str();
  Serial.print("Weight: "+Serial2.readString());
  myData.weight=(int)(Serial2.readString().toFloat() * 1000);
  //delay(2000);
  //Serial.print();
  Serial.println(" ");
  readCard();
  //long int RFID=myData.rfid_id;
  //Serial.printf(" "+readCard());
  
  //Serial.println(" ");

  int moisture=0;
  moisture= analogRead(Moisture_PIN);
  Serial.println(moisture);

  int mappedValue = map(moisture, 0, 4095, 0, 255);
  mappedValue = constrain(mappedValue, 60, 20);

  Serial.println(mappedValue);
  myData.moisture=mappedValue;

  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    

    


  }
  //const char rfid1[i]=path_rfid();
  
  if (Firebase.RTDB.setInt(&fbdo, "Farmer/userDataRecords/RFID" , myData.rfid_id)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }
  

    if (Firebase.RTDB.setInt(&fbdo, "Farmer/userDataRecords/weight", myData.weight)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }


    if (Firebase.RTDB.setInt(&fbdo, "Farmer/userDataRecords/moisture", myData.moisture)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }

  reading_id++;
  myData.reading_id=reading_id;

  if (Firebase.RTDB.setInt(&fbdo, "Farmer/userDataRecords/reading_id", myData.reading_id)) {
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
    }
    else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
    }




    






  delay(1500);
}
