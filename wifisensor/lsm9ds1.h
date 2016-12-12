class LSM9DS: public I2CSensor {
  public:
    LSM9DS(int i2cAddr_acc, int i2cAddr_mag):I2CSensor(i2cAddr_acc){
      this->i2cAddr_mag = i2cAddr_mag;
      }
    virtual void begin(){
      // Before initializing the IMU, there are a few settings
      // we may need to adjust. Use the settings struct to set
      // the device's communication mode and addresses:
      imu.settings.device.commInterface = IMU_MODE_I2C;
      imu.settings.device.mAddress = i2cAddr_mag;
      imu.settings.device.agAddress = i2cAddr;
      // The above lines will only take effect AFTER calling
      // imu.begin(), which verifies communication with the IMU
      // and turns it on.
      if (!imu.begin())
      {
        Serial.println("Failed to communicate with LSM9DS1.");
        Serial.println("Double-check wiring.");
        Serial.println("Default settings in this sketch will " \
                      "work for an out of the box LSM9DS1 " \
                      "Breakout, but may need to be modified " \
                      "if the board jumpers are.");
        while (1)
          ;
      }
    }
    
    virtual float readValue(int kind) {
      if (kind==I2CSENSOR_VALUE_KIND_TEMP) {
        return 0.0f;
      } else if (kind==I2CSENSOR_VALUE_KIND_ACC_TOTAL) {
        float acc2;
        float comp;
        acc2 = 0;
        imu.readAccel();  
      
        Serial.print(imu.calcAccel(imu.az), 2);
        Serial.print ("\r\n");
      
        Serial.print(imu.az);
        Serial.print ("\r\n");
        
        //acc = imu.calcAccel(imu.ax);
        comp = imu.calcAccel(imu.ax);
        acc2 += comp*comp;
      
        comp = imu.calcAccel(imu.ay);
        acc2+= comp*comp;
      
        comp = imu.calcAccel(imu.az);
        acc2 += comp*comp;
      
        return sqrt(acc2);
      } else {
        Serial.println("Unsupported 'kind' on LSM9DS1");
        return -1.0;
      }
    }
  private:
    LSM9DS1 imu;
    int i2cAddr_mag;
};

