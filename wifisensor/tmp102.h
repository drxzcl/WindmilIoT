class TMP102: public I2CSensor {
  public:
    TMP102(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){}
    virtual float readValue(int kind) {
      int val; 
      float temp; 

      // Read the temperature register (0x00). We're in 12-bit mode.
      Wire.beginTransmission(i2cAddr); 
      Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      val = (Wire.read() << 4);
      val |= (Wire.read() >> 4);

      temp = val*0.0625;

      if (val == 4095) {
        temp = 0.0f;
      }

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
      
      // Set the TMP102 to shutdown mode.
      Wire.beginTransmission(i2cAddr); // Request config registers
      Wire.write(0x01);
      Wire.write(0b00000001);
      Wire.endTransmission();

    }

    // Read the first byte of the status register. It's all we need.
    byte getStatus1() {
      byte result;
      Wire.beginTransmission(i2cAddr);
      Wire.write(0x01);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      result = Wire.read();
      Wire.read();
      return result;
    }

    virtual float readValue(int kind) {
      //TMP102
      byte firstbyte, secondbyte; //these are the bytes we read from the TMP102 temperature registers

      // Request a conversion by setting the MSB bit (OS) to 1
      Wire.beginTransmission(i2cAddr); 
      Wire.write(0x1); // Config register
      Wire.write(0b10000001); // Send one-shot!
      Wire.endTransmission();
      // OS will remain low until conversion is ready.
      while ((getStatus1() & 0b10000000) == 0) {
        delay(5); // 26 ms for a conversion, according to the datasheet
      }
      int val; 
      float temp; 

      // Read the temperature register (0x00). We're in 12-bit mode.
      Wire.beginTransmission(i2cAddr); 
      Wire.write(0x00);
      Wire.endTransmission();
      Wire.requestFrom(i2cAddr, 2);
      Wire.endTransmission();
      val = (Wire.read() << 4);
      val |= (Wire.read() >> 4);

      temp = val*0.0625;

      if (val == 4095) {
        temp = 0.0f;
      }

      return temp;

    }
  private:

};
