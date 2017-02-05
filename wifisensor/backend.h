/*
 * 
 * 
 */

 
class Backend {
  public:
    Backend(){ }    
    virtual void begin(String id) = 0;
    virtual void addValue(String name, float value) = 0;  
    virtual String generateRequest() = 0;  
    virtual String host() = 0;
};

struct SensorStream {
  Backend* backend;
  const int numReadings;
  SensorReading readings[12];   
};

class IoTBackend: public Backend {
  public:
    IoTBackend(String host, String secret){
      this->m_host = host;
      this->m_secret = secret;
    }
    virtual void begin(String id)
    {
      m_url = "/publish?secret="+m_secret+"&id=" + id;      
    }
    virtual void addValue(String name, float value)
    {
      m_url = m_url + "&" + name + "=" + value;
    }  
    virtual String generateRequest(){
      String request = String("GET ") + m_url + " HTTP/1.1\r\n" +
               "Host: " + m_host + "\r\n" +
               "User-Agent: WinmilIoT\r\n" +
               "Connection: close\r\n\r\n";
      return request;  
    }
    virtual String host() { return m_host;}
  private:
    String m_url;
    String m_host;    
    String m_secret;
};

class PhantBackend: public Backend {
  public:
    PhantBackend(String host, String pubkey, String privkey): m_host(host), m_pubkey(pubkey), m_privkey(privkey), m_phant(host, pubkey, privkey)
    {      
    }
    virtual void begin(String id)
    {
      m_phant = Phant(m_host, m_pubkey, m_privkey); 
      m_phant.add("who", id);
    }
    virtual void addValue(String name, float value)
    {
      m_phant.add(name, value);
    }  
    virtual String generateRequest(){
      return m_phant.post();
    }
    virtual String host() { return m_host;}
  private:
    String m_host;    
    String m_pubkey;
    String m_privkey;
    Phant m_phant;
};




