#include <EEPROM.h>                         // EEPROM 

#include <avr/wdt.h> 
#define flow_sensor_pin   2                  // FLOW SENSOR PIN
#define relay_pin         12                 // RELAY PIN
int eeAddress = 0;
unsigned long relay_on_time = 0;
unsigned long max_time_limit = 600000;        // 600000 for 10 Minitues
int relay_status = 0;
int X;
int Y;
float TIME = 0;
float FREQUENCY = 0;
float WATER = 0;
float TOTAL = 0;
float LS = 0;

struct master_info{
  double max_water_limit;                         // Max Water Limit
  double consumed_water;                          // Total Consumed Water
  int emergency_status = 0;                       // 0/1 -> Valve Open/Closed
};
master_info mi;
void(* resetFunc) (void) = 0;      
void setup() {
  pinMode(relay_pin, OUTPUT);
  pinMode(flow_sensor_pin,INPUT);
  digitalWrite(relay_pin, LOW);
  Serial.begin(9600);
  EEPROM.get(eeAddress, mi);
//  max_water_limit = mi.max_water_limit;
//  consumed_water = mi.consumed_water;
//  emergency_status = mi.emergency_status;
  wdt_disable();
  delay(3000);
  wdt_enable(WDTO_8S);
}

void loop() {
  wdt_reset();
  delay(1000);
  X = pulseIn(flow_sensor_pin, HIGH);
  Y = pulseIn(flow_sensor_pin, LOW);
  TIME = X + Y;
  FREQUENCY = 1000000/TIME;
  WATER = 2*(FREQUENCY/7.5); // This is a calibrated line. 2 is multipled for calibration.
  LS = WATER/60;
  if(FREQUENCY >= 0){
    if(isinf(FREQUENCY)){
      Serial.println("Rain Stopped");
      Serial.println("0.00L/M");
      //Serial.println((String)consumed_water+"L");
      if(relay_status == 1){
        relay_status = 0;
        digitalWrite(relay_pin, LOW);        
      }      
    }
    else  {
      if(relay_status == 0){
        relay_status = 1;
        relay_on_time = millis();
        digitalWrite(relay_pin, HIGH);
        Serial.println("Rain Started");
        Serial.println("Relay On");
      }
      if(millis() - relay_on_time >= max_time_limit){
        digitalWrite(relay_pin, LOW);
      }
      Serial.println((String)WATER+"L/M");
      
      
    }
  }
}
//void saveCredentials() {
//  mi.max_water_limit = max_water_limit;
//  mi.emergency_status = emergency_status;
//  mi.consumed_water = consumed_water;
//  
//  int eeAddress = 0;
//  EEPROM.put(eeAddress, mi);
//  
//}
