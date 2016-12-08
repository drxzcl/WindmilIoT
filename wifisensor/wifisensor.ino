// Include the ESP8266 WiFi library. (Works a lot like the
// Arduino WiFi library.)
#include <ESP8266WiFi.h>
// Include the SparkFun Phant library.
#include <Phant.h>

// Sparkfun MPU9250 library
#include <MPU9250.h> 
#include <Wire.h>

/*
 *  Passwords and keys are stored in config.h. You need to make your own.
 *  Copy config.h.example to config.h and edit to taste.
 */
#include "config.h"

/////////////////////
// Temp sensor configuration
/////////////////////
#define TMP102_I2C_ADDRESS 72 

#if defined TEMP_TMP102
#define getTemp getTemp102
#elif defined TEMP_MPU925x
#define getTemp getTempMPU925x 
#else 
#error Missing TEMP_ define in config.h!
#endif


/////////////////
// Post Timing //
/////////////////
const unsigned long postRate = 6000;
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
  
  //TMP102
  Wire.begin(); // start the I2C library
}

void loop() 
{
  // This conditional will execute every lastPost milliseconds
  // (assuming the Phant post succeeded).
  if ((lastPost + postRate <= millis()) || lastPost == 0)
  {
    Serial.println("Posting to Phant!");
    if (postToPhant())
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
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);     // Set LED as output
  digitalWrite(LED_PIN, HIGH);  // LED off
}


float getTempMPU925x() {
  MPU9250 myIMU;
  Wire.begin();
  byte c = myIMU.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
  //Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
        myIMU.tempCount = myIMU.readTempData();  // Read the adc values
        // Temperature in degrees Centigrade
        myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
        // Print temperature in degrees Centigrade
        Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
        Serial.println(" degrees C");
  
  
  return myIMU.temperature;
  
}



//TMP102
float getTemp102(){
  byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers
  int val; /* an int is capable of storing two bytes, this is where we "chuck" the two bytes together. */
  float temp; /* We then need to multiply our two bytes by a scaling factor, mentioned in the datasheet. */ 
 
  /* Reset the register pointer (by default it is ready to read temperatures)
You can alter it to a writeable register and alter some of the configuration -
the sensor is capable of alerting you if the temperature is above or below a specified threshold. */
 
  Wire.beginTransmission(TMP102_I2C_ADDRESS); //Say hi to the sensor.
  Wire.write(0x00);
  Wire.endTransmission();
  Wire.requestFrom(TMP102_I2C_ADDRESS, 2);
  Wire.endTransmission();
 
 
    firstbyte      = (Wire.read());
    /*read the TMP102 datasheet - here we read one byte from
    each of the temperature registers on the TMP102*/
    secondbyte      = (Wire.read());
    /*The first byte contains the most significant bits, and
    the second the less significant */
    val = ((firstbyte) << 4);  
    /* MSB */
    val |= (secondbyte >> 4);    
    /* LSB is ORed into the second 4 bits of our byte.*/

    temp = val*0.0625;

    if (val == 4095) {
      // We have no sensor, so we read the max value. Make it 0 instead.    
      temp = 0.0f;
    }
 
  Serial.print("Converted temp is ");
  Serial.print("\t");
  Serial.println(temp);
  Serial.println();

  return temp;
}

int postToPhant()
{
  // LED turns on when we enter, it'll go off when we 
  // successfully post.
  digitalWrite(LED_PIN, LOW);

  // Declare an object from the Phant library - phant
  Phant phant(PhantHost, PublicKey, PrivateKey);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to the SSID:
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.macAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String postedID =  WiFiSSID + ("-" + macID); // Parentheses force correct operator

  // Add the field/value pairs defined by our stream:
  phant.add("who", postedID);
  phant.add("temp",getTemp());

  // Now connect to data.sparkfun.com, and post our data:
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(PhantHost, httpPort)) 
  {
    // If we fail to connect, return 0.
    return 0;
  }
  // If we successfully connected, print our Phant post:
  client.print(phant.post());
  Serial.println(phant.post());


  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    //Serial.print(line); // Trying to avoid using serial
  }

  // Before we exit, turn the LED off.
  digitalWrite(LED_PIN, HIGH);

  return 1; // Return success
}
