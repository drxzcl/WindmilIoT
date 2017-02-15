class TMP102: public I2CSensor {
  public:
    TMP102(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){}
    virtual float readValue(int kind) {
        //TMP102
      byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers
      int val; /* an int is capable of storing two bytes, this is where we "chuck" the two bytes together. */
      float temp; /* We then need to multiply our two bytes by a scaling factor, mentioned in the datasheet. */

      /* Reset the register pointer (by default it is ready to read temperatures)
        You can alter it to a writeable register and alter some of the configuration -
        the sensor is capable of alerting you if the temperature is above or below a specified threshold. */

      Wire.beginTransmission(i2cAddr); //Say hi to the sensor.
      Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
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
  private:

};

/**
  One-shot version.
  **/
class TMP102_OS: public I2CSensor {
  public:
    TMP102_OS(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){
      // When initializing, set it to shutdown mode.
      byte firstbyte, secondbyte;

      // Now shut it down!
      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x01);
      Wire.write(0b00000001);
      //Wire.write(0b01100000);
      Wire.endTransmission();

      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x01);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      firstbyte      = (Wire.read());
      secondbyte      = (Wire.read());
      Serial.println(firstbyte,BIN);
      Serial.println(secondbyte,BIN);


    }

    byte getStatus1() {
      byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers
      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x01);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      firstbyte      = (Wire.read());
      secondbyte      = (Wire.read());
      return firstbyte;
    }

    void printStatus() {
      byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers
      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x01);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      firstbyte      = (Wire.read());
      secondbyte      = (Wire.read());
      Serial.println(firstbyte,BIN);
      Serial.println(secondbyte,BIN);
    }

    virtual float readValue(int kind) {
      //TMP102
      byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers

      // request conversion!
      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x1);
      Wire.write(0b10000001); // Send one-shot!
      Wire.endTransmission();
      while ((getStatus1() & 0b10000000) == 0) {
        delay(5); // 26 ms for a conversion, according to the datasheet
      }
      Serial.println();
      int val; /* an int is capable of storing two bytes, this is where we "chuck" the two bytes together. */
      float temp; /* We then need to multiply our two bytes by a scaling factor, mentioned in the datasheet. */

      /* Reset the register pointer (by default it is ready to read temperatures)
        You can alter it to a writeable register and alter some of the configuration -
        the sensor is capable of alerting you if the temperature is above or below a specified threshold. */

      Wire.beginTransmission(i2cAddr); //Say hi to the sensor.
      Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();


      firstbyte      = (Wire.read());
      secondbyte      = (Wire.read());
      val = ((firstbyte) << 4);
      val |= (secondbyte >> 4);

      temp = val*0.0625;

      if (val == 4095) {
        temp = 0.0f;
      }

      return temp;

    }
  private:

};
