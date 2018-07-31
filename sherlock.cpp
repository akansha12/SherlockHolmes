#include <TinyGPS.h>
#include <SoftwareSerial.h>
SoftwareSerial SIM900(7, 8);

TinyGPS gps;  //Creates a new instance of the TinyGPS object
// LED & Buzzer pins
const int ledPin = 6;
const int buttonPin=9;
int fix=1;
// Alarm
int counter = 0;
// Size of the geo fence (in meters)
const float maxDistance = 100;
// Latitude & longitude for distance measurement
float initialLatitude;
float initialLongitude;
float latitude, longitude;

void setup()
{
  Serial.begin(9600);
  SIM900.begin(9600);
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

   digitalWrite(ledPin, LOW);
  // Init counter
  counter = millis();
  SIM900.println("Sherlock Holmes WELCOMES you,you know where to find me ");
  SIM900.println("");
}

void loop()
{
   if(fix==1)
  {
    bool newD=false;
    for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newD = true;  
    }
  }

  if (newD)      //If newData is true
  { 
     gps.f_get_position(&latitude, &longitude);
    initialLatitude = latitude;
    initialLongitude =longitude;
  }
    fix=0;
  } 

  SIM900.print("initial lat " );
  SIM900.println(initialLatitude);
  SIM900.print("initial lon ");
  SIM900.println(initialLongitude);

  
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (Serial.available())
    {
      char c = Serial.read();
      //Serial.print(c);
      if (gps.encode(c)) 
        newData = true;  
    }
  }

  if (newData)      //If newData is true
  { 
    
    float flat, flon;
    unsigned long age;
    gps.f_get_position(&flat, &flon, &age);   
    float distance = distanceCoordinates(flat, flon, initialLatitude, initialLongitude);
    SIM900.print("distance ");
    SIM900.println(distance);
      if (distance > maxDistance)
      {
     digitalWrite(ledPin, HIGH);
    SIM900.print("AT+CMGF=1\r"); 
    delay(400);
    SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number with country code
    delay(300);
    SIM900.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
    SIM900.print("Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    SIM900.print(" Longitude = ");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  
    delay(200);
    SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    SIM900.println();
      }
      else 
      {
        digitalWrite(ledPin, LOW);
        SIM900.println(" hi you are in your boundary  ");
        SIM900.println(" ");
      }
     
    if( digitalRead(buttonPin)==HIGH)
    {
      SIM900.println("Sherlock Its an emergency.....HELP! ");
      SIM900.print("AT+CMGF=1\r"); 
    delay(400);
    SIM900.println("AT + CMGS = \"+91xxxxxxxxxx\"");// recipient's mobile number with country code
    delay(300);
    SIM900.println("Find me here ");
    SIM900.print("http://maps.google.com/maps?&z=15&mrt=yp&t=k&q=");
    SIM900.print("Latitude = ");
    SIM900.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    SIM900.print(" Longitude = ");
    SIM900.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
  
    delay(200);
    SIM900.println((char)26); // End AT command with a ^Z, ASCII code 26
    delay(200);
    SIM900.println();
    }
   

 
  Serial.println(failed);
 // if (chars == 0)
   // Serial.println("** No characters received from GPS: check wiring **");
  }

}

// Calculate distance between two points
float distanceCoordinates(float flat1, float flon1, float flat2, float flon2) {

  // Variables
  float dist_calc=0;
  float dist_calc2=0;
  float diflat=0;
  float diflon=0;

  // Calculations
  diflat  = radians(flat2-flat1);
  flat1 = radians(flat1);
  flat2 = radians(flat2);
  diflon = radians((flon2)-(flon1));

  dist_calc = (sin(diflat/2.0)*sin(diflat/2.0));
  dist_calc2 = cos(flat1);
  dist_calc2*=cos(flat2);
  dist_calc2*=sin(diflon/2.0);
  dist_calc2*=sin(diflon/2.0);
  dist_calc +=dist_calc2;

  dist_calc=(2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));
  
  dist_calc*=6371000.0; //Converting to meters

  return dist_calc;
}

