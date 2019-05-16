#include <Wire.h>
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

//Humidity Sensor (DHT22) set up
#define DHTPIN 5
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE); 

int chk;
float dht22_hum;  //Stores hum

//NRF24L01 set up
RF24 transmit (2, 4);
const byte addresses[6] = "00001";
//byte addresses[][6] = {"0"};

RF24 receive (7, 8);
const byte addresses1[6] = "00010";

// Data struct to send data through
struct barometer
{
  float pascals=1;
  float tempC = 78.5;
  float humidity;
  float smoke = 0;
};

typedef struct barometer Barometer;
Barometer data;
barometer data2;

MPL3115A2 baro;

void setup() {
  Serial.begin(9600);                 
  Wire.begin();                   
  dht.begin();
  baro.begin();
  
  baro.setModeBarometer(); 
  baro.setOversampleRate(7);
  baro.enableEventFlags();
  
//transimtter
  transmit.begin();  
  transmit.setPALevel(RF24_PA_MIN);
  transmit.openWritingPipe(addresses);
  transmit.stopListening();
  delay(1000);


// receiver
  receive.begin(); 
  receive.openReadingPipe(0,addresses1);
  receive.setPALevel(RF24_PA_MIN);
  receive.startListening();
  delay(1000);
}

void loop() {

//Receive data from first node in system
  receive.read(&data, sizeof(data));
//Collect data from onboard sensors
  data2.humidity = dht.readHumidity(); 
  data2.pascals = baro.readPressure();
  data2.tempC = dht.readTemperature(true);
  data2.smoke = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);
  delay(1000);

//Transmit both data sets to last node
  transmit.write(&data, sizeof(data));
  Serial.print("Start");
  delay(100);
  transmit.write(&data2, sizeof(data2));

  Serial.print("Received from 11");
  Serial.print("\nBarometer:");
  Serial.print(data.pascals);
  Serial.print("\nTemperature:");
  Serial.print(data.tempC);
  Serial.print("\nHumidity: ");
  Serial.print(data.humidity);
  Serial.print(" %");
  Serial.print("\nSmoke:");
  Serial.print(data.smoke);
  Serial.print("\n");

  Serial.print("On board sensors");
  Serial.print("\nBarometer:");
  Serial.print(data2.pascals);
  Serial.print("\nTemperature:");
  Serial.print(data2.tempC);
  Serial.print("\nHumidity: ");
  Serial.print(data2.humidity);
  Serial.print(" %");
  Serial.print("\nSmoke:");
  Serial.print(data2.smoke);
  Serial.print("\n");

 }

