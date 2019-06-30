#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 5 on the Arduino 
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
int relay = 8;
float Celcius=0;
//counter variable for counting more than one second
int count =0;
void initTimer(void)
{
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 3035;            // preload timer 65536-16MHz/256*1Hz
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
  interrupts();             // enable all interrupts
}

void setup(void)
{
   // start serial port 
  Serial.begin(9600);
   // Start up the library 
  pinMode(relay,OUTPUT);
  sensors.begin();
  sensors.setResolution(9);
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  //call timer initialization
  initTimer();
}



void loop(void)
{ 
  digitalWrite(relay,HIGH);
  sensors.setResolution(9);
  sensors.requestTemperaturesByIndex(0); // Send the command to get temperatures
  Celcius = sensors.getTempCByIndex(0); // on récupère les temps. par index de capteurs
  //sensors.requestTemperatures(); // Send the command to get temperature reading
  //Celcius=sensors.getTempCByIndex(0);// Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
  Serial.print(" current temp = ");
  Celcius+=0.5;
  Serial.println(Celcius);
  delay(100);   //hardware delay "software is fast"
}

//Timer Interrupt function called every 1 sec
ISR(TIMER1_OVF_vect){
  count++;
  //if 1.5 min passed
  if (count == 90){ //90 sec
      Serial.print("Your Temp = ");
      Celcius = Celcius+0.5;
      Serial.println(Celcius); //for exact temp "Calibration"
      count = 0; 
    }
}


  /*
  >> My Results 
  -> give the exact temp after 4 min
  -> I round the value then it will give the reading in 2 min
  -> This was at 12-bit resolution
  -> On 9-bit resolution the exact reached after 2 min
  -> it can round in 1 min by adding 1 or 1.5 "starting from normal degree" 
  -> after 2 min reach by 0.5 degree variation "mentioned in datasheet"
  -> stariting from 32 need < 1min to reach exact
  -> In cold "winter" starting with low temp make delay
  -> In hot "summer" starting with high temp make fast reach to our body temp
  */
