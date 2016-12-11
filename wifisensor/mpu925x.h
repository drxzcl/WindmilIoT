class MPU925x: public I2CSensor {
  public:
    MPU925x(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){}
    virtual float readValue(int kind) {
        MPU9250 myIMU;
        byte c = myIMU.readByte(this->i2cAddr, WHO_AM_I_MPU9250);
        myIMU.tempCount = myIMU.readTempData();  // Read the adc values
        // Temperature in degrees Centigrade
        myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
        // Print temperature in degrees Centigrade
        Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
        Serial.println(" degrees C");
    
        return myIMU.temperature;
    }
  private:
    
};

