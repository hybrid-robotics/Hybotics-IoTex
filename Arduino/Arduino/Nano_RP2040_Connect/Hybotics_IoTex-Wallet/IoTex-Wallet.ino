#include <Arduino.h>
#include <WiFi.h>
#include "Secrets.h"
#include "IoTeX-blockchain-client.h"
 
#define ONBOARD_LED  2
 
constexpr const char ip[] = "gateway.iotexlab.io";
constexpr const char baseUrl[] = "iotexapi.APIService";
constexpr const int port = 10000;

// Set the wallet address to check
const char accountStr[] = "io1xkx7y9ygsa3dlmvzzyvv8zm6hd6rmskh4dawyu";
 
// Create the IoTeX client connection
Connection<Api> connection(ip, port, baseUrl);
 
void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(F("Connecting to WiFi .."));

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
 
    Serial.print(F("\r\nConnected. IP: "));
    Serial.println(WiFi.localIP());
}
 
void setup(void) {
    Serial.begin(115200);
 
    // Connect to the wifi network
    initWiFi();
 
    // Configure the LED pin
    pinMode(ONBOARD_LED, OUTPUT);
}
 
void loop(void) {
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
    digitalWrite(ONBOARD_LED, LOW);
  else
    digitalWrite(ONBOARD_LED, HIGH);
 
  Serial.println("Program finished");
 
  while (true) {
    delay(1000);
  }
}