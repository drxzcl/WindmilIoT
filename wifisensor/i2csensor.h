/*
 * 
 * 
 */

const int I2CSENSOR_VALUE_KIND_TEMP = 1;
const int I2CSENSOR_VALUE_KIND_ACC_TOTAL = 2;
 
class I2CSensor {
  public:
    I2CSensor(int i2cAddr){
      this->i2cAddr = i2cAddr;
    }
    I2CSensor(const I2CSensor& that) = delete;
    
    virtual void begin() = 0;
    virtual float readValue(int kind) = 0;

  protected:
    int i2cAddr;    
  
};

class ZeroSensor: public I2CSensor {
  public:
    ZeroSensor(int i2cAddr):I2CSensor(i2cAddr){ }
    virtual void begin(){}
    virtual float readValue(int kind) {
        return 0.0f;
    }
  private:
    
};

class CapSensor: public I2CSensor {
  public:
    CapSensor(int i2cAddr):I2CSensor(i2cAddr){ 
        capsensor.set_CS_AutocaL_Millis(0xFFFFFFFF);
      }
    virtual void begin(){}
    virtual float readValue(int kind) {
        return capsensor.capacitiveSensorRaw(60);
    }
  private:
  CapacitiveSensor capsensor = CapacitiveSensor(D5,D6);
  
    
};

struct SensorReading {
    I2CSensor* sensor;
    int kind;
    String name;
};



