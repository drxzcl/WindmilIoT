// Include the ESP8266 WiFi library. (Works a lot like the
// Arduino WiFi library.)
#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>

// Sparkfun MPU9250 library
#include <MPU9250.h>
#include <Wire.h>

// https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library/archive/master.zip
#include <LSM9DS1_Types.h>
#include <SparkFunLSM9DS1.h>
#include <LSM9DS1_Registers.h>

#include <CapacitiveSensor.h>


#include "i2csensor.h"
#include "mpu925x.h"
#include "tmp102.h"
#include "lsm9ds1.h"
#include "backend.h"


/*
    Passwords and keys are stored in config.h. You need to make your own.
    Copy config.h.example to config.h and edit to taste.
*/
#include "config.h"

ADC_MODE(ADC_VCC); // Use the ADC to measure the voltage.

/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 500;
unsigned long lastPost = 0;


/////////////////////
// Pin Definitions //
/////////////////////
#if defined THING
const int LED_PIN = 5;  // Thing's onboard, green LED
#elif defined NODEMCU
const int LED_PIN = D0; // NodeMCU's onboard, blue LED
#else
#error Missing Board define in config.h!
#endif


void setup()
{
  initHardware(); // Setup input/output I/O pins
  connectWiFi(); // Connect to WiFi
  digitalWrite(LED_PIN, LOW); // LED on to indicate connect success

  Wire.begin(); // start the I2C library

  // Initialize the sensors
  for (int i = 0; i < numSensors; i++) {
    sensors[i]->begin();
  }
}

void loop()
{
  // This conditional will execute every lastPost milliseconds
  if ((lastPost + postRate <= millis()) || lastPost == 0)
  {
    Serial.println("Posting to backend!");
    if (postToBackend())
    {
      lastPost = millis();
      Serial.println("Post Suceeded!");
    }
    else // If the Phant post failed
    {
      delay(500); // Short delay, then try again
      Serial.println("Post failed, will try again.");
    }
  }

  delay(5000); //wait 5 seconds before printing our next set of readings.
}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(LED_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void initHardware()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);     // Set LED as output
  digitalWrite(LED_PIN, HIGH);  // LED off
}


int postToBackend()
{
  // LED turns on when we enter, it'll go off when we
  // successfully post.
  digitalWrite(LED_PIN, LOW);


  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to the SSID:
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID =  WiFiSSID + ("-" + macID); // Parentheses force correct operator

  for (int i = 0; i < numStreams; i++) {
    // Add the field/value pairs defined by our stream:
    streams[i].backend->begin(postedID);

    for (int j = 0; j < streams[i].numReadings; j++) {
      SensorReading r = streams[i].readings[j];
      streams[i].backend->addValue(r.name, r.sensor->readValue(r.kind));
    }

    streams[i].backend->send(true);    
  }

  // Before we exit, turn the LED off.
  digitalWrite(LED_PIN, HIGH);
  return 1; // Return success
}
