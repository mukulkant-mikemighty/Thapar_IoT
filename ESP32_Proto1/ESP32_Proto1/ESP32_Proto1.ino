#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <SPI.h>
#include <MFRC522.h>



#define RXp2 16
#define TXp2 17

#define SS_PIN  21  /*Slave Select Pin*/
#define RST_PIN 22  /*Reset Pin for RC522*/
#define LED_G   12   /*Pin 8 for LED*/
// Set your Board ID (ESP32 Sender #1 = BOARD_ID 1, ESP32 Sender #2 = BOARD_ID 2, etc)
#define BOARD_ID 2


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

/*
#include <HTTPClient.h>
//#include <WiFi.h>
#include "ESPAsyncWebSrv.h"
#include <Arduino_JSON.h>
*/

////const char* wifi_network_ssid     = "Team MIKE";   /*Replace with Your own network SSID*/
////const char* wifi_network_password =  "invincibles"; /*Replace with Your own network PASSWORD*/
/*
JSONVar board;

AsyncWebServer server(80);
AsyncEventSource events("/events");

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Copies the sender mac address to a string
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["board_id"] = incomingReadings.board_id;
  board["per_unit_weight"] = incomingReadings.per_unit_weight;
  board["reading_id"] = incomingReadings.reading_id;
  board["rfid_id"] = incomingReadings.rfid_id;
  String jsonString = JSON.stringify(board);
  events.send(jsonString.c_str(), "new_readings", millis());
  
  Serial.printf("Board ID %i: %u bytes\n", incomingReadings.board_id, len);
  Serial.printf("per_unit_weight value: %4.2f \n", incomingReadings.per_unit_weight);
  Serial.println("rfid_id value: ");
  //Serial.println(incomingReadings.rfid_id.);
  for(byte i=0;i< 4 ;i++){
    Serial.printf("%x",incomingReadings.rfid_id[i]);
  }
  Serial.printf("\nreading_id value: %d \n", incomingReadings.reading_id);
  Serial.println();
}









const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP-NOW DASHBOARD</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p {  font-size: 1.2rem;}
    body {  margin: 0;}
    .topnav { overflow: hidden; background-color: #2f4468; color: white; font-size: 1.7rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); }
    .reading { font-size: 2.8rem; }
    .packet { color: #bebebe; }
    .card.Packet_info { color: #fd7e14; }
    .card.rfid_id { color: #1b78e2; }
  </style>
</head>
<body>
  <div class="topnav">
    <h3>ESP-NOW DASHBOARD</h3>
  </div>
  <div class="content">
    <div class="cards">
      <div class="card Packet_info">
        <h4>  <i class="fa-regular fa-weight-scale"></i>   BOARD #1</h4>
        <p><span class="reading">PACKET INFO<span id="b1"></span> </span></p>
        <p class="packet">Reading ID: <span id="rb1"></span></p>
      </div>
      <div class="card rfid_id"> 
        <h4><i class="fa-solid fa-nfc-signal"></i> BOARD #1</h4>
        <p><span class="reading">RFID ID:<span id="rfid1"></span></span></p>
        <p class="packet">Reading ID: <span id="rb1"></span></p>
      </div>



      <div class="card Packet_info">
        <h4><i class="fa-regular fa-weight-scale"></i> BOARD #2</h4>
        <p><span class="reading">PACKET INFO<span id="b2"></span> </span></p>
        <p class="packet">Reading ID: <span id="rb2"></span></p>
      </div>
      
      <div class="card rfid_id">
        <h4><i class="fa-solid fa-nfc-signal"></i> BOARD #2</h4>
        <p><span class="reading">RFID ID:<span id="rfid2"></span></span></p>
        <p class="packet">Reading ID: <span id="rb2"></span></p>
      </div>



    </div>
  </div>
<script>
if (!!window.EventSource) {
 var source = new EventSource('/events');
 
 source.addEventListener('open', function(e) {
  console.log("Events Connected");
 }, false);
 source.addEventListener('error', function(e) {
  if (e.target.readyState != EventSource.OPEN) {
    console.log("Events Disconnected");
  }
 }, false);
 
 source.addEventListener('message', function(e) {
  console.log("message", e.data);
 }, false);
 
 source.addEventListener('new_readings', function(e) {
  console.log("new_readings", e.data);
  var obj = JSON.parse(e.data);
  document.getElementById("b"+obj.id).innerHTML = obj.Packet_info;
  document.getElementById("rb"+obj.id).innerHTML = obj.reading_id;
  document.getElementById("rfid"+obj.id).innerHTML = obj.rfid_id;
  
 }, false);
}
</script>
</body>
</html>)rawliteral";



*/










////////
////////


/*/////
//MAC Address of the receiver 
uint8_t broadcastAddress[] = {0xB8, 0xD6, 0x1A, 0x47, 0x63, 0x04};//change

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

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 10000;        // Interval at which to publish sensor readings

unsigned int reading_id = 0;

// Insert your SSID
constexpr char WIFI_SSID[] = "Team MIKE";
constexpr char WIFI_PASSWORD[] ="invincibles";
int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}
*//////

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

// callback when data is sent
/*void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
*/

void setup()
{
  Serial.begin(9600);   /*Serial Communication begin*/
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);
  SPI.begin();          /*SPI communication initialized*/
  mfrc522.PCD_Init();   /*RFID sensor initialized*/
  pinMode(LED_G, OUTPUT);  /*LED Pin set as output*/
  //pinMode(2,INPUT);
  ////////
  ////////
  /*
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
  int32_t channel = getWiFiChannel(WIFI_SSID);

  //WiFi.printDiag(Serial); // Uncomment to verify channel number before
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Uncomment to verify channel change after

  //Init ESP-NOW
  /*if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  //Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;
  
  //Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  */
  ////////
  ////////
  
  Serial.println("Put your card to the reader...");
  Serial.println();

}
void loop()
{
  //////
  //////
  /*
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;
    //Set values to send
    myData.board_id = BOARD_ID;
    //mfrc522.PICC_ReadCardSerial();
    readCard();
    reading_id++;
    myData.reading_id=reading_id; 
    //Send message via ESP-NOW
    /*esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Sent with success");
    }
    else {
      Serial.println("Error sending the data");
    }
  }
  */
  ////////
  ////////
  myData.board_id = BOARD_ID;
  //mfrc522.PICC_ReadCardSerial();
  /*
  Serial.printf("Weight:");
  Serial.print(Serial2.readString());
  Serial.println(" ");
  readCard();
  */
  Serial.print("Weight: "+Serial2.readString());
  //Serial.print();
  Serial.println(" ");
  readCard();
  //Serial.printf(" "+readCard());
  
  Serial.println(" ");
  float moisture= analogRead(2);
  Serial.println(moisture);

  reading_id++;
  myData.reading_id=reading_id;

}
