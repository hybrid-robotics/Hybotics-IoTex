#include <Arduino.h>
#include <WiFi.h>
#include <UMS3.h>
#include "Secrets.h"
#include "IoTeX-blockchain-client.h"

#define DEFAULT_BLINK_TIME_MS      500
#define DEFAULT_ATTEMPT_TIME_MS   1000
#define DEFAULT_NUMBER_ATTEMPTS     10
 
constexpr const char ip[] = "gateway.iotexlab.io";
constexpr const char baseUrl[] = "iotexapi.APIService";
constexpr const int port = 10000;

// Set the wallet address to check
const char accountStr[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
 
// Create the IoTeX client connection
Connection<Api> connection(ip, port, baseUrl);
UMS3 ums3;

const uint32_t RED      = ums3.color(127, 0, 0);
const uint32_t GREEN    = ums3.color(0, 127, 127);
const uint32_t BLUE     = ums3.color(0, 0, 127);
const uint32_t YELLOW   = ums3.color(127, 127, 0);
const uint32_t MAGENTA  = ums3.color(127, 0, 127);
const uint32_t BLACK    = ums3.color(0, 0, 0);

bool wifi_connected;

#define DEFAULT_PIXEL_COLOR       BLUE

void umBlinkPixel(uint32_t color=DEFAULT_PIXEL_COLOR, uint16_t blink_time_ms=DEFAULT_BLINK_TIME_MS, uint16_t nr_cycles=1) {
  uint8_t cycle_count=0;

  for (cycle_count=0; cycle_count<nr_cycles; cycle_count++) {
    ums3.setPixelColor(color);
    delay(blink_time_ms);
    ums3.setPixelColor(BLACK);
    delay(blink_time_ms);
  }
}

bool initWiFi(char* ssid, char* password, uint8_t blink_time_ms=DEFAULT_BLINK_TIME_MS, uint8_t nr_attempts=DEFAULT_NUMBER_ATTEMPTS, uint8_t attempt_time_ms=DEFAULT_ATTEMPT_TIME_MS)
{
  bool connected = true;
  uint8_t attempts = 0;
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to WiFi .."));

  while ((WiFi.status() != WL_CONNECTED) and attempts<nr_attempts) {
    attempts++;

    Serial.print("Attempt #");
    Serial.println(attempts);
    umBlinkPixel(YELLOW);
    delay(attempt_time_ms);
  }

  connected = (attempts < nr_attempts);

  if (connected) {
    Serial.print(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("Unable to connect to WiFi!"));
  }

  return connected;
}
 
void setup(void) {
  Serial.begin(115200);

  ums3.begin();
  ums3.setPixelPower(true); 
   
  //  Connect to the wifi network
  wifi_connected = initWiFi(WIFI_SSID, WIFI_PASSWORD);
}
 
void loop(void) {
  if (wifi_connected) {
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
  
    if (b == zero)
      ums3.setPixelColor(BLACK);
    else
      ums3.setPixelColor(MAGENTA);
  } else {
    while (true) {
      umBlinkPixel(RED);
    }    
  }
     
  Serial.println("Program finished");
 
  }
