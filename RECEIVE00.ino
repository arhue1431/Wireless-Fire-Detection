#include <Wire.h>
//#include <extEEPROM.h>
//#include <EEPROMAnything.h>
#include "SparkFunMPL3115A2.h"
#include <SPI.h>  
#include "RF24.h"
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <RF24_config.h>

// Smoke sensor 
#define         MQ_PIN                       (0)     
#define         RL_VALUE                     (5)     
#define         RO_CLEAN_AIR_FACTOR          (9.83)  
#define         CALIBARAION_SAMPLE_TIMES     (50)    
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   
#define         READ_SAMPLE_INTERVAL         (50)    
#define         READ_SAMPLE_TIMES            (5)      

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

#define         GAS_SMOKE                    (2)

float           SmokeCurve[3] ={2.3,0.53,-0.44};                                                        
float           Ro           =  10;                 

int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10,( ((log(rs_ro_ratio)-pcurve[1])/pcurve[2]) + pcurve[0])));
}

int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
     return MQGetPercentage(rs_ro_ratio,SmokeCurve);
 
  return 0;
}

float MQRead(int mq_pin)
{
  int i;
  float rs=0;
 
  for (i=0;i<READ_SAMPLE_TIMES;i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
 
  rs = rs/READ_SAMPLE_TIMES;
 
  return rs;  
}

float MQCalibration(int mq_pin)
{
  int i;
  float val=0;
 
  for (i=0;i<CALIBARAION_SAMPLE_TIMES;i++) {        
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val/CALIBARAION_SAMPLE_TIMES;
  val = val/RO_CLEAN_AIR_FACTOR;                        
 
  return val; 
}

float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE*(1023-raw_adc)/raw_adc));
}


//Humidiry and Temperature sensor DHT22
#define DHTPIN 5

#define DHTTYPE DHT22  
DHT dht(DHTPIN, DHTTYPE);
int chk;
float dht22_hum;  //Stores humidity value
float dht22_temp; //Stores temperature value


MPL3115A2 baro;

//NRF24L01 Set ups
//Receiver 
RF24 myRadio1 (2,4);
const byte addresses[6] = "00010";
//Receiver when node 10 fails
RF24 myRadio(7,8);
const byte address[6] = "00001";

//Send data using this struct
struct barometer
{
  float pascals=1;
  float tempC = 78.5;
  float humidity = 39;
  float smoke=0;
  float CO= 9;
  float LPG=2;
};

typedef struct barometer Barometer;
Barometer data;
barometer data2;
Barometer data1;
barometer data3;

void setup() 
{
  Serial.begin(9600);                   
  Wire.begin();                  
  baro.begin();
  dht.begin();
  baro.setModeBarometer();
  baro.setOversampleRate(7); 
  baro.enableEventFlags();
  
  myRadio.begin();
  myRadio.openReadingPipe(0,address);
  myRadio.setPALevel(RF24_PA_MIN);
  myRadio.startListening();

  myRadio1.begin();
  myRadio1.setPALevel(RF24_PA_MIN);
  myRadio1.openWritingPipe(address);
  myRadio1.stopListening();
}

void loop()  
{

  if ( myRadio.available()) 
  {
    
    while (myRadio.available())
    {

      //Read data
      myRadio.read(&data, sizeof(data));
      
      float pascals = data.pascals;
      float tempC=data.tempC;
      float humidity=data.humidity;
      float zero = 0;

      //if nothing is read from node 10, read from 11 directly
      //otherwise continue reading from node 10 
      
      if (pascals == zero && tempC == zero && humidity == zero){
        delay(1000);
        myRadio1.read(&data, sizeof(data));
        
      Serial.print("Received from 11; 10 failed");}
      else{
      Serial.print("Received from 10; Sensors from 11");
      }
      Serial.print("\nPressure:");
      Serial.print(data.pascals);
      Serial.print("\nTemperature:");
      Serial.print(data.tempC);
      Serial.print("\nHumidity:");
      Serial.println(data.humidity);
      Serial.print("\Smoke:");
      Serial.println(data.smoke);
      delay(1000);

      //continue reading from node 10 if availible
      if (pascals != zero && tempC != zero && humidity != zero){
          myRadio.read(&data2, sizeof(data2));
      Serial.print("Received from 10 Sensors");
      Serial.print("\nBarometer:");
      Serial.print(data2.pascals);
      Serial.print("\nTemperature:");
      Serial.print(data2.tempC);
      Serial.print("\nHumidity: ");
      Serial.print(data2.humidity);
      Serial.print(" %");
      Serial.print("\nSmoke:");
      Serial.print(data2.smoke);
      Serial.print("\nCO:");
      Serial.print(data2.CO);
      Serial.print("\nLPG:");
      Serial.println(data2.LPG);
      Serial.print("\n");
      }      


      delay(1000);


      data3.humidity = dht.readHumidity(); 
      data3.pascals = baro.readPressure();
      data3.tempC = dht.readTemperature(true);
      data3.smoke = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE); 


      Serial.print("On board Sensors");
      Serial.print("\nBarometer:");
      Serial.print(data3.pascals);
      Serial.print("\nTemperature:");
      Serial.print(data3.tempC);
      Serial.print("\nHumidity: ");
      Serial.print(data3.humidity);
      Serial.print(" %");
      Serial.print("\nSmoke:");
      Serial.print(data3.smoke);
      Serial.print("\n");

      delay(1000);
   
  }
  }


} 
