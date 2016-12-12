class MPU925x: public I2CSensor {
  public:
    MPU925x(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){}
    virtual float readValue(int kind) {
      MPU9250 myIMU;
      if (kind==I2CSENSOR_VALUE_KIND_TEMP) {
        byte c = myIMU.readByte(this->i2cAddr, WHO_AM_I_MPU9250);
        myIMU.tempCount = myIMU.readTempData();  // Read the adc values
        // Temperature in degrees Centigrade
        myIMU.temperature = ((float) myIMU.tempCount) / 333.87 + 21.0;
        // Print temperature in degrees Centigrade
        Serial.print("Temperature is ");  Serial.print(myIMU.temperature, 1);
        Serial.println(" degrees C");
    
        return myIMU.temperature;        
      } else if (kind==I2CSENSOR_VALUE_KIND_ACC_TOTAL) {
         
         float acc2;
         
         myIMU.readAccelData(myIMU.accelCount);  // Read the x/y/z adc values
         myIMU.getAres();
         // Now we'll calculate the accleration value into actual g's
         // This depends on scale being set
         myIMU.ax = (float)myIMU.accelCount[0]*myIMU.aRes; // - accelBias[0];
         myIMU.ay = (float)myIMU.accelCount[1]*myIMU.aRes; // - accelBias[1];
         myIMU.az = (float)myIMU.accelCount[2]*myIMU.aRes; // - accelBias[2];

         acc2 =  myIMU.ax *  myIMU.ax;
         acc2 +=  myIMU.ay *  myIMU.ay;
         acc2 +=  myIMU.az *  myIMU.az;

         float acc = sqrt(acc2);
         Serial.print("Total acc: ");Serial.println(acc);
         return sqrt(acc2);

      } else {
        Serial.println("Unsupported 'kind' on MPU925x");
        return -1.0;
      }
    }
  private:
    
};

