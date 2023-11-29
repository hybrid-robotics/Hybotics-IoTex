/*
*/

#include <Arduino.h>
#include <WiFi.h>
#include "Secrets.h"
#include "IoTeX-blockchain-client.h"
#include <UMS3.h>

#define DEFAULT_WIFI_WAIT_MS    1000
#define DEFAULT_BLINK_PERIOD_MS  100
#define DEFAULT_PIXEL_NUMBER       0

//  Initialize the UnexpectedMaker helper
UMS3 ums3;

#define DEFAULT_COLOR           BLUE
#define MAX_CONNECT_ATTEMPTS    5

constexpr const char ip[] = "gateway.iotexlab.io";
constexpr const char baseUrl[] = "iotexapi.APIService";
constexpr const int port = 10000;

//  Set the wallet address to check
const char accountStr[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";

//  Initialize color variables
uint32_t BLACK;
uint32_t RED;
uint32_t GREEN;
uint32_t BLUE;
uint32_t MAGENTA;
uint32_t YELLOW;

bool network_ok;
 
//  Create the IoTeX client connection
Connection<Api> connection(ip, port, baseUrl);

void umBlinkPixel(uint8_t color=DEFAULT_COLOR, uint16_t period_ms=DEFAULT_BLINK_PERIOD_MS, uint8_t nr_cycles=1) {    
  uint16_t count;
  
  for(count=0; count<nr_cycles; count++) {
    ums3.setPixelColor(color);
    delay(period_ms);
    ums3.setPixelColor(BLACK);
    delay(period_ms);        
  }  
}

bool initWiFi() {
  uint8_t attempts = 0;
  bool connected = true;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print(F("Attempting to connecting to WiFi .."));

  while ((WiFi.status() != WL_CONNECTED) and (attempts < MAX_CONNECT_ATTEMPTS)) {
    attempts++;

    Serial.print("Attempt #");
    Serial.print(attempts);
    Serial.println("..");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
       
    umBlinkPixel(YELLOW, DEFAULT_WIFI_WAIT_MS);  
  }

  if (attempts < MAX_CONNECT_ATTEMPTS) {
    Serial.print(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
  } else {
    connected = false;    
    Serial.print(F("\r\nUnable to connect to WiFi"));
  }

  return connected;  
}
 
void setup(void) {
  bool connected;

  Serial.begin(115200);

  ums3.begin();
  ums3.setPixelBrightness(255 / 3);

  //  Initialize the onboard NeoPixel        
  ums3.setPixelColor(BLACK); 

  //  Define colors
  BLACK     = ums3.color(0, 0, 0);
  RED       = ums3.color(127, 0, 0);
  GREEN     = ums3.color(0, 127, 0);
  BLUE      = ums3.color(0, 0, 127);
  MAGENTA   = ums3.color(127, 0, 127);
  YELLOW    = ums3.color(127, 127, 0);

  //  Connect to the wifi network
  network_ok = initWiFi();
}
 
void loop(void) {
  if (network_ok) {
    ums3.setPixelColor(GREEN);

    // Query the account metadata
    AccountMeta accountMeta;
    ResultCode result = connection.api.wallets.getAccount(accountStr, accountMeta);
  
    // Print the result
    Serial.print("Result: ");
    Serial.println(IotexHelpers.GetResultString(result));
  
    // If the query suceeded, print the account metadata
    if (result == ResultCode::SUCCESS) {
      Serial.print("Balance: ");
      Serial.println(accountMeta.balance);
      Serial.print(F("Nonce: "));
      Serial.println(accountMeta.nonce.c_str());
      Serial.print(F("PendingNonce: "));
      Serial.println(accountMeta.pendingNonce.c_str());
      Serial.print(F("NumActions: "));
      Serial.println(accountMeta.numActions.c_str());
      Serial.print(F("IsContract: "));
      Serial.println(accountMeta.isContract ? "\"true\"" : "\"false\"");
    }
  
    // Enable the onboard led if the balance is > 0.01 IOTX)
    Bignum b = Bignum(accountMeta.balance, NumericBase::Base10);
    Bignum zero = Bignum("0", NumericBase::Base10);

    if (b == zero) {
      ums3.setPixelColor(BLACK);
    } else {
      ums3.setPixelColor(MAGENTA);
    }
  } else {
    while (!network_ok) {
      umBlinkPixel(RED, DEFAULT_WIFI_WAIT_MS);  
    }
  }

  Serial.println("Program finished");
}
