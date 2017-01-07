// TCS230 sensor calibration and color readings
//
// Input and output using the Serial console.
//
#include <MD_TCS230.h>
#include <FreqCount.h>

#define  BLACK_CAL 0
#define WHITE_CAL 1
#define READ_VAL  2

// Pin definitions
#define  S2_OUT  12
#define  S3_OUT  13
#define  OE_OUT   8    // LOW = ENABLED 
int led = 7;

MD_TCS230 CS(S2_OUT, S3_OUT, OE_OUT);

void setup() 
{
  pinMode(led, OUTPUT);    
  digitalWrite(led, 0); 
  Serial.begin(57600);
  Serial.print(F("\n[TCS230 Calibrator Example]"));

  CS.begin();
}

char getChar()
// blocking wait for an input character from the input stream
{
  while (Serial.available() == 0)
    ;
  return(toupper(Serial.read()));
}

void clearInput()
// clear all characters from the serial input
{
  while (Serial.read() != -1)
    ;
}

uint8_t fsmReadValue(uint8_t state, uint8_t valType, uint8_t maxReads)
// Finite State Machine for reading a value from the sensor
// Current FSM state is passed in and returned
// Type of value being read is passed in
{
  static  uint8_t selChannel;
  static  uint8_t readCount;
  static  sensorData  sd;

  switch(state)
  {
  case 0: // Prompt for the user to start
               digitalWrite(led,0);
    Serial.print(F("\n\nReading value for "));
    switch(valType)
    {
    case BLACK_CAL: Serial.print(F("BLACK calibration")); break;
    case WHITE_CAL: Serial.print(F("WHITE calibration")); break;
    case READ_VAL:  Serial.print(F("DATA"));        break;
    default:    Serial.print(F("??"));          break;
    }
    
    Serial.print(F("\nPress any key to start ..."));
    state++;
    break;

  case 1: // Wait for user input
    getChar();
    clearInput();
    state++;
    break;

  case 2: // start the reading process
                digitalWrite(led,1);
    CS.read();
    state++;
    break;

  case 3: // wait for a read to complete
    if (CS.available()) 
    {
      sensorData  sd;
      colorData rgb;

      switch(valType)
      {
      case BLACK_CAL: 
        CS.getRaw(&sd); 
        CS.setDarkCal(&sd);   
        break;

      case WHITE_CAL: 
        CS.getRaw(&sd); 
        CS.setWhiteCal(&sd);  
        break;

      case READ_VAL:  
        CS.getRGB(&rgb);
        Serial.print(F("\nRGB is ["));
        Serial.print(rgb.value[TCS230_RGB_R]);
        Serial.print(F(","));
        Serial.print(rgb.value[TCS230_RGB_G]);
        Serial.print(F(","));
        Serial.print(rgb.value[TCS230_RGB_B]);
        Serial.print(F("]"));

         float k; 
      //  k=rgb2hsv(rgb.value[TCS230_RGB_R],rgb.value[TCS230_RGB_G],rgb.value[TCS230_RGB_B]);
       k=  convert(rgb.value[TCS230_RGB_R],rgb.value[TCS230_RGB_G],rgb.value[TCS230_RGB_B]);
         Serial.println(k);
        break;
      }
      state++;
                                digitalWrite(led,0);
    }
    break;

  default:  // reset fsm
    state = 0;
    break;
  }

  return(state);
}


void loop() 
{
  static uint8_t  runState = 0;   
  static uint8_t  readState = 0;

  switch(runState)
  {
  case 0: // calibrate black
    readState = fsmReadValue(readState, BLACK_CAL, 2);
    if (readState == 0) runState++;
    break;

  case 1: // calibrate white
    readState = fsmReadValue(readState, WHITE_CAL, 2);
    if (readState == 0) runState++;
    break;

  case 2: // read color
    readState = fsmReadValue(readState, READ_VAL, 1);
    break;

  default:
    runState = 0; // start again if we get here as something is wrong
  }
}


float convert(float r,float g,float b) 
{
  float fmax,fmin,l,s,d;
  float h;
 
  r=r/255;
  g=g/255;
  b=b/255;
   fmax=max(r,max(g,b));
   fmin=min(r,min(g,b));
   Serial.print("r:");  Serial.print(r); Serial.print("g:");  Serial.print(g); Serial.print("b:");  Serial.print(b);
  Serial.print("\nmax:");  Serial.print(fmax);
    Serial.print("\nmin:");  Serial.print(fmin);
   l=(fmin+fmax)/2;    
    Serial.print("\nl:");  Serial.print(l);
    if(l<0.5){
      s=(fmax-fmin)/(fmax+fmin);
    }
    else{
        s=(fmax-fmin)/(2.0-fmax+fmin);
    }
       Serial.print("\ns:");  Serial.print(s);
       
    if(fmax==r)
    {  Serial.print("\n if R");
        float t2;   float t1;
     t1=g-b;
     Serial.print("\nfmax:");  Serial.print(fmax);
     Serial.print("\nfmin:");  Serial.print(fmin);
     t2=fmax-fmin;
      Serial.print("\nt1:");  Serial.print(t1);
        Serial.print("\nt2:");  Serial.print(t2);
      h=t1/t2;
          Serial.print("\nsh:");  Serial.print(h);
    }
      else if(fmax==g)
    {
      Serial.print("\n if G");
          h=2+((b-r)/(fmax-fmin));
    }
     else if(fmax==b)
    {
      Serial.print("\n if B");
          h=4+((r-g)/(fmax-fmin));
    }
       Serial.print("\n h:");  Serial.print(h);
    h=h*60;   
    if(h<0){
      h=+360;
      }
    Serial.print("\n h*60:");  Serial.print(h);
    Serial.print("\n\n");



if((0<=h && h<=30) || (300<=h && h<=365))
{
  //red

   Serial.println("Red");
return 1; 
}
else if(30<h&&h<=60)
{
   Serial.println("Yell");
  //yel
return 2; 
}
else if(60<h&&h<170)
{
  Serial.println("Gred");
  //gre
return 3; 
}
else if(170<h&&h<270)
{
   Serial.println("blue");
  //blue
return 4; 
}
else{
   Serial.println("NNNNNNNNNNNNNNNN");
  return 0;
}  
}




