#include <esp_now.h>
#include <WiFi.h>

#define trigPin 12  
#define echoPin 13 
#define sLeftPin 25
#define sRightPin 27
//----------------------------------------

uint8_t broadcastAddress[] = {0x78, 0x21, 0x84, 0xE4, 0xA5, 0x2C}; //--> REPLACE WITH THE MAC Address of your receiver / ESP32 Receiver.

//----------------------------------------Variables to accommodate the data to be sent.
int send_sLeft_val;
int send_sRight_val;
int send_distance_val;
//----------------------------------------

String success; //--> Variable to store if sending data was successful.

//----------------------------------------Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
    int sLeft;
    int sRight;
    long distance;
} struct_message;

struct_message send_Data; //--> Create a struct_message to send data.
//----------------------------------------

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
  Serial.println(">>>>>");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ VOID SETUP
void setup() {
  Serial.begin(115200);

  pinMode(sLeftPin,INPUT);
  pinMode(sRightPin,INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  WiFi.mode(WIFI_STA); //--> Set device as a Wi-Fi Station.

  //----------------------------------------Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //----------------------------------------
  
  //----------------------------------------Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //----------------------------------------
  
  //----------------------------------------Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  //----------------------------------------
  
  //----------------------------------------Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  //----------------------------------------
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ VOID LOOP
void loop() {
  long duration, dis;
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  dis = (duration/2) / 29.1;


  //----------------------------------------Set values to send
  send_sLeft_val = digitalRead(sLeftPin);
  send_sRight_val = digitalRead(sRightPin);
  send_distance_val = dis;
  send_Data.sLeft = send_sLeft_val;
  send_Data.sRight = send_sRight_val;
  send_Data.distance = send_distance_val;
  //----------------------------------------
  
  Serial.println("Receive Data: ");
  Serial.println(send_sLeft_val);
  Serial.println(send_sRight_val);
  Serial.println(send_distance_val);
  Serial.println("<<<<<");
  Serial.println();
  Serial.print(">>>>> ");
  Serial.println("Send data");

  //----------------------------------------Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  //----------------------------------------
  
  delay(50);
}