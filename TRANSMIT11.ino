
#include <Wire.h>
#include "SparkFunMPL3115A2.h"
#include <SPI.h>  
#include "RF24.h"
#include <RF24_config.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

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

//NRF24L01 Transmitter

RF24 transmit(7, 8);
const byte addresses[6] = "00010";

//Humidity Sensor DHT22
#define DHTPIN 4
#define DHTTYPE DHT22 
DHT dht(DHTPIN, DHTTYPE); //initialize DHT sensor for 16mhz

//DHT22 Variable 
int chk;
float dht22_hum;  //Stores humidity value
float dht22_temp; //Stores temperature value


struct barometer
{
  float pascals=1;
  float tempC = 78.5;
  float humidity;
  float smoke = 0;
};

typedef struct barometer Barometer;
Barometer data;

MPL3115A2 baro;

void setup() {
  //from transmitter_1 code
  
  Serial.begin(9600);
  transmit.begin();
  transmit.openWritingPipe(addresses);
  transmit.setPALevel(RF24_PA_MIN);
  transmit.stopListening();

  Wire.begin();  
  baro.begin();
  dht.begin();
  
  baro.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  baro.setOversampleRate(7); // Set Oversample to the recommended 128
  baro.enableEventFlags(); // Enable all three pressure and temp event flags 

}

void loop() {

//Read on board data
  data.humidity = dht.readHumidity(); 
  data.pascals = baro.readPressure();
  data.tempC = dht.readTemperature();
  data.smoke = MQGetGasPercentage(MQRead(MQ_PIN)/Ro,GAS_SMOKE);

//transmit data to second modules 
  transmit.write(&data, sizeof(data)); 

//print values 
  Serial.print("\nBarometer:");
  Serial.print(data.pascals);
  Serial.print("\nTemperature:");
  Serial.print(data.tempC);
  Serial.print("\nHumidity: ");
  Serial.print(data.humidity);
  Serial.print(" %");
  Serial.print("\nSmoke:");
  Serial.println(data.smoke);
  delay(1000);
}
