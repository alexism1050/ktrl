#include "Bargraph_variables.h"
#include <SoftPWM.h>

char foo; //trick for IDE

#define NSENSORS 8 
#define MIDISTEPS 127 
#define DEADZONEGENERAL 0.05 //% of deadzone to max and min value of sensors
#define DEADZONEMEMORY 0.05 //% of memoryzone at the bottom of the sensors
#define DEADZONERELATIVE 0.05 //% of deathzone at the middle on relative mode

#define NTOPNBUTTONS 1 
#define CONFIGBUTTON 0



//A12-A13 : SOFTWARE PWM
#define PORT_DataA12 PORTK
#define PORT_ClkA13 PORTK
#define DATA_PinA12 12 //DATA IN
#define CLK_PinA13 13 //CLK IN
#define BIT_DataA12 0x01<<4
#define BIT_ClkA13 0x01<<5

//A14-A15 : SOFTWARE PWM
#define PORT_DataA14 PORTK
#define PORT_ClkA15 PORTK
#define DATA_PinA14 14 //DATA IN
#define CLK_PinA15 15 //CLK IN
#define BIT_DataA14 0x01<<6
#define BIT_ClkA15 0x01<<7

//D2-D3 : HARDWARE PWM
#define PORT_DataD2 PORTE
#define PORT_ClkD3 PORTE
#define DATA_PinD2 2 //DATA IN
#define CLK_PinD3 3 //CLK IN
#define BIT_DataD2 0x01<<4
#define BIT_ClkD3 0x01<<5


//D4-D5 : HARDWARE PWM
#define PORT_DataD4 PORTG
#define PORT_ClkD5 PORTE
#define DATA_PinD4 4 //DATA IN
#define CLK_PinD5 5 //CLK IN
#define BIT_DataD4 0x01<<5
#define BIT_ClkD5 0x01<<3

// D6- D7  : HARDWARE PWM
#define PORT_DataD6 PORTH
#define PORT_ClkD7 PORTH
#define DATA_PinD6 6 //DATA IN
#define CLK_PinD7 7 //CLK IN
#define BIT_DataD6 0x01<<3
#define BIT_ClkD7 0x01<<4

// D98- D9 : HARDWARE PWM 
#define PORT_DataD8 PORTH
#define PORT_ClkD9 PORTH
#define DATA_PinD8 8 //DATA IN
#define CLK_PinD9 9 //CLK IN
#define BIT_DataD8 0x01<<5
#define BIT_ClkD9 0x01<<6

//D10-D11 : HARDWARE PWM
#define PORT_DataD10 PORTB
#define PORT_ClkD11 PORTB
#define DATA_PinD10 10 //DATA IN
#define CLK_PinD11 11 //CLK IN
#define BIT_DataD10 0x01<<4
#define BIT_ClkD11 0x01<<5


//D12-D13 : HARDWARE PWM
#define PORT_DataD12 PORTB
#define PORT_ClkD13 PORTB
#define DATA_PinD12 12 //DATA IN
#define CLK_PinD13 13 //CLK IN
#define BIT_DataD12 0x01<<6
#define BIT_ClkD13 0x01<<7

#define CmdMode 0x0000 //Work on 8-bit mode
#define ON 0x00ff //8-bit 1 data
#define SHUT 0x0000 //8-bit 0 data

int sensSendMIDI_toLEDS[NSENSORS];


int MIDMIDISTEPS;
//sensors

int sensActiv[NSENSORS];  //used sensors
int sensPort[NSENSORS]; //arduino analog port of the sensor

int sensRaw[NSENSORS];  //raw value readed
int sensMidiPitch[NSENSORS];
int sensMidiNote[NSENSORS];
int sensSendMIDI[NSENSORS]; //1 or 0 if this sensor need the midi signal to be sended

int sensMidiSteps[NSENSORS];  //midi steps that the sensor will use

int sensRangMax[NSENSORS]; //ranges
int sensRangMin[NSENSORS]; 
int sensRangMem[NSENSORS]; //memory range at bottom.

int sensRangMidMem[NSENSORS]; //value at the mid of the range with the mem.

int sensRangMidMaxMem[NSENSORS]; 
int sensRangMidMinMem[NSENSORS]; 


int sensMode[NSENSORS]; //mode of each sensor

//buttons

int topButtonsValue[NTOPNBUTTONS]; 

//auxiliars

float rawToPitchSteps[NSENSORS];  //raw to pitch step
float rawToPitchStepsMem[NSENSORS];  //raw to pitch step with memory range



// ------------ SHIFT OUT ----------

#define NUMBER_OF_LEDS 8
#define MAXIMUM_BRIGHTNESS 255
int latchPin = 45; // D45 = PWM
int clockPin = 46; // D6 = PWM
int dataPin = 47; // D47
int outputEnablePin = 44;//D44 = PWM (OE)
byte leds = 0;
// ----------------------------




//functions

void readTopButtons();

void rawToMidi(int currentSen);
void sendMidis();
void setLigths(int currentSen);

void normalizeMidiPitch(int currentSen);

void debug(int currentSen);

void send16bitDataD2D3(unsigned int dataD2);
void latchDataD2D3(void);
void sendLED(unsigned int LEDstate);


void setup()
{
  Serial.begin(9600);  

  for (int i = 0; i<NSENSORS; i++) {

    MIDMIDISTEPS = MIDISTEPS / 2;

    sensActiv[i]=1;

    sensPort[i]=i;    

    sensRaw[i]=0;

    sensMidiSteps[i]= MIDISTEPS/MIDISTEPS;

    sensRangMax[i]=500;
    sensRangMin[i]=50;
    sensRangMem[i]=50;

    sensRangMidMem[i]=(sensRangMax[i]-(sensRangMin[i]+sensRangMem[i]))/2;    

    sensRangMidMaxMem[i]+=sensRangMidMem[i]*DEADZONERELATIVE; 
    sensRangMidMinMem[i]-=sensRangMidMem[i]*DEADZONERELATIVE; 

    sensMode[i]=0;

    rawToPitchSteps[i] = (sensRangMax[i] - sensRangMin[i])/(MIDISTEPS+0.0);
    rawToPitchStepsMem[i] = (sensRangMax[i] - (sensRangMin[i]+sensRangMem[i]))/(MIDISTEPS+0.0);




  }

  sensMode[0]=1;
  sensActiv[0]=0;
  sensActiv[1]=0;
  sensActiv[2]=1;
  sensActiv[3]=0;
  sensActiv[4]=0;
  sensActiv[5]=0;
  sensActiv[6]=0;
  sensActiv[7]=0;


  // Initialize Software PWM library
  SoftPWMBegin();

  // Create and set pins PWM to 0 (off)
  SoftPWMSet(A12, 0);
  SoftPWMSet(A13, 0);
  SoftPWMSet(A14, 0);
  SoftPWMSet(A15, 0);
  // Set fade time for ALL PWM pins declared to 1 ms fade-up time, and 1 ms fade-down time
  SoftPWMSetFadeTime(ALL, 1, 1);



  // Declaration of pins as OUTPUTS

  // HARDWARE PWM
  pinMode(DATA_PinD2,OUTPUT); //Data pin
  pinMode(CLK_PinD3,OUTPUT); //CLK pin
  pinMode(DATA_PinD4,OUTPUT); //Data pin
  pinMode(CLK_PinD5,OUTPUT); //CLK pin
  pinMode(DATA_PinD6,OUTPUT); //Data pin
  pinMode(CLK_PinD7,OUTPUT); //CLK pin
  pinMode(DATA_PinD8,OUTPUT); //Data pin
  pinMode(CLK_PinD9,OUTPUT); //CLK pin
  pinMode(DATA_PinD10,OUTPUT); //Data pin
  pinMode(CLK_PinD11,OUTPUT); //CLK pin
  pinMode(DATA_PinD12,OUTPUT); //Data pin
  pinMode(CLK_PinD13,OUTPUT); //CLK pin
 
 
  // SOFTWARE PWM
    pinMode(DATA_PinA12,OUTPUT); //Data pin
  pinMode(CLK_PinA13,OUTPUT); //CLK pin
  pinMode(DATA_PinA14,OUTPUT); //Data pin
  pinMode(CLK_PinA15,OUTPUT); //CLK pin

  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  pinMode(A6,INPUT);
  pinMode(A7,INPUT);
  
  // ------------ SHIFT OUT ----------
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(outputEnablePin, OUTPUT); 
  leds = 255;
updateShiftRegister();
  // ----------------------------
  
  
}

void loop()
{ 


  
  readTopButtons();
  if(topButtonsValue[CONFIGBUTTON]==1){ //config mode
  }
  else { //normal use mode
    for (int currentSen = 0; currentSen<NSENSORS; currentSen++) {
      if(sensActiv[currentSen]==1){
        sensRaw[currentSen] = analogRead(sensPort[currentSen]);
        rawToMidi(currentSen);        
        setLigths(currentSen);  
        sendFrontLEDS(sensMidiPitch[currentSen]*2);
        sendMidis();
                
        delay(100);  
        debug(currentSen);  
      }
    }
  }
  

}


//functions

void readTopButtons(){
  for (int currentTopButton = 0; currentTopButton<NTOPNBUTTONS; currentTopButton++) {
  }
}

void rawToMidi(int currentSen){

  if(sensMode[currentSen]==0){ //absolute         
    sensMidiPitch[currentSen] = (sensRaw[currentSen]-sensRangMin[currentSen]) / rawToPitchSteps[currentSen];    
    normalizeMidiPitch(currentSen);  
    sensSendMIDI[currentSen] = 1;
  }
  else if(sensMode[currentSen]==1){ //absolute with memory
    if (sensRaw[currentSen] >= sensRangMem[currentSen]){
      sensMidiPitch[currentSen] = (sensRaw[currentSen]-(sensRangMin[currentSen]+sensRangMem[currentSen])) / rawToPitchStepsMem[currentSen];
      normalizeMidiPitch(currentSen);      
      sensSendMIDI[currentSen] = 1;
    }    
  }
  else if(sensMode[currentSen]==2){ //relative mode
    if (sensRaw[currentSen] > sensRangMem[currentSen]){
      if(sensRaw[currentSen]>sensRangMidMaxMem[currentSen] || sensRaw[currentSen]<sensRangMidMinMem[currentSen]){
        int relativeAux = (sensRaw[currentSen]-(sensRangMin[currentSen]+sensRangMem[currentSen])) / rawToPitchStepsMem[currentSen];
        if (relativeAux > MIDMIDISTEPS){ 
          sensMidiPitch[currentSen] += abs((MIDISTEPS - relativeAux) - MIDMIDISTEPS) ;
        }
        else if (relativeAux < MIDMIDISTEPS){ 
          sensMidiPitch[currentSen] -= abs((MIDISTEPS - relativeAux) - MIDMIDISTEPS) ;
        }
        normalizeMidiPitch(currentSen);
        sensSendMIDI[currentSen] = 1;

      }

    }
  }
}
void sendMidis(){
}




void setLigths(int currentSen){


  sensSendMIDI_toLEDS[currentSen]=convert_to_led(sensMidiPitch[currentSen]);



  if(currentSen==0){
    // BARGRAPH 0
    send16bitDataA12A13(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
    //  send16bitData(CmdMode); //set second LED Bar mode,if you do not use two LED Bar work together(connect one by one),you can delete this line.
    //  sendLED(i); //send second LED Bar data,if you do not use two LED Bar work together(connect one by one),you can delete this line.
    latchDataA12A13(); //make it come into effect

  }
  else if(currentSen==1){


    // BARGRAPH 1
    send16bitDataA14A15(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[1]); //send first LED Bar data
    latchDataA14A15(); //make it come into effect

  }
   else if(currentSen==2){
    // BARGRAPH 2
    send16bitDataD2D3(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[2]); //send first LED Bar data
    latchDataD2D3(); //make it come into effect
  }
  else if(currentSen==3){

    // BARGRAPH 3
    send16bitDataD4D5(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[3]); //send first LED Bar data
    latchDataD4D5(); //make it come into effect
  }

  else if(currentSen==4){

    // BARGRAPH 4
    send16bitDataD6D7(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[4]); //send first LED Bar data
    latchDataD6D7(); //make it come into effect

  }

  else if(currentSen==5){


    // BARGRAPH 5
    send16bitDataD8D9(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[5]); //send first LED Bar data
    latchDataD8D9(); //make it come into effect

  }
  else if(currentSen==6){

    // BARGRAPH 6
    send16bitDataD10D11(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[6]); //send first LED Bar data
    latchDataD10D11(); //make it come into effect
  }

  else if(currentSen==7){ 

    // BARGRAPH 7
    send16bitDataD12D13(CmdMode); //set first LED Bar mode
    sendLED(sensSendMIDI_toLEDS[7]); //send first LED Bar data
    latchDataD12D13(); //make it come into effect
  }
  //i=i*2+1; // increment i in order to get the leds ON one by one




}

void normalizeMidiPitch(int currentSen){	
  //TODO sensMidiPitch[currentSen]%sensMidiSteps[currentSen];	
  if ( sensMidiPitch[currentSen] > MIDISTEPS){
    sensMidiPitch[currentSen] = MIDISTEPS;
  }
  else if ( sensMidiPitch[currentSen] < 0){
    sensMidiPitch[currentSen] = 0;
  }

}

void debug(int currentSen){

  Serial.begin(9600);
  Serial.print("SEN: ");
  Serial.print(currentSen);
  Serial.print(" RAW: \t");
  Serial.print(sensRaw[currentSen]);
  Serial.print("\tP: \t");
  Serial.print(sensMidiPitch[currentSen]);
  Serial.println(" .");
}






  // ------------ SHIFT OUT ----------


void sendFrontLEDS(byte brightness)
{

  // To use if we want to control leds one by one
  // infos http://learn.adafruit.com/adafruit-arduino-lesson-4-eight-leds/brightness-control

/* setBrightness(MAXIMUM_BRIGHTNESS);
  leds = 0;
  updateShiftRegister();
  
for (int i = 0; i < NUMBER_OF_LEDS; i++)
  {
    bitSet(leds, i);
    updateShiftRegister();
    delay(500);
  }
  */
  
  /*
//  for controlling all leds at the same time
  for (byte b = MAXIMUM_BRIGHTNESS; b > 0; b--)
  { 
    setBrightness(b);
    delay(20);
  }
*/

    setBrightness(brightness);

}


void setBrightness(byte brightness) // 0 to 255
{
  analogWrite(outputEnablePin, 255-brightness);
} 
 
// not used if all the leds are controlled at the same time 
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}


  // ----------------------------






//Send 16_bit data on A12 A13
void send16bitDataA12A13(unsigned int dataA12)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataA12&0x8000)
    {
      PORT_DataA12 |= BIT_DataA12;
    }
    else
    {
      PORT_DataA12 &=~ BIT_DataA12;
    }
    PORT_ClkA13 ^= BIT_ClkA13;
    dataA12 <<= 1;
  }
}

//Send 16_bit data on A14 A15
void send16bitDataA14A15(unsigned int dataA14)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataA14&0x8000)
    {
      PORT_DataA14 |= BIT_DataA14;
    }
    else
    {
      PORT_DataA14 &=~ BIT_DataA14;
    }
    PORT_ClkA15^= BIT_ClkA15;
    dataA14 <<= 1;
  }
}



//Send 16_bit data on D2 D3
void send16bitDataD2D3(unsigned int dataD2)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD2&0x8000)
    {
      PORT_DataD2 |= BIT_DataD2;
    }
    else
    {
      PORT_DataD2 &=~ BIT_DataD2;
    }
    PORT_ClkD3 ^= BIT_ClkD3;
    dataD2 <<= 1;
  }
}

//Send 16_bit data on D4 D5
void send16bitDataD4D5(unsigned int dataD4)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD4&0x8000)
    {
      PORT_DataD4 |= BIT_DataD4;
    }
    else
    {
      PORT_DataD4 &=~ BIT_DataD4;
    }
    PORT_ClkD5 ^= BIT_ClkD5;
    dataD4 <<= 1;
  }
}

//Send 16_bit data on D6 D7
void send16bitDataD6D7(unsigned int dataD6)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD6&0x8000)
    {
      PORT_DataD6 |= BIT_DataD6;
    }
    else
    {
      PORT_DataD6 &=~ BIT_DataD6;
    }
    PORT_ClkD7 ^= BIT_ClkD7;
    dataD6 <<= 1;
  }
}

//Send 16_bit data on D8 D9
void send16bitDataD8D9(unsigned int dataD8)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD8&0x8000)
    {
      PORT_DataD8 |= BIT_DataD8;
    }
    else
    {
      PORT_DataD8 &=~ BIT_DataD8;
    }
    PORT_ClkD9 ^= BIT_ClkD9;
    dataD8 <<= 1;
  }
}

//Send 16_bit data on D10 D11
void send16bitDataD10D11(unsigned int dataD10)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD10&0x8000)
    {
      PORT_DataD10 |= BIT_DataD10;
    }
    else
    {
      PORT_DataD10 &=~ BIT_DataD10;
    }
    PORT_ClkD11 ^= BIT_ClkD11;
    dataD10 <<= 1;
  }
}

//Send 16_bit data on D12 D13
void send16bitDataD12D13(unsigned int dataD12)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD12&0x8000)
    {
      PORT_DataD12 |= BIT_DataD12;
    }
    else
    {
      PORT_DataD12 &=~ BIT_DataD12;
    }
    PORT_ClkD13 ^= BIT_ClkD13;
    dataD12 <<= 1;
  }
}



//latch routine for MY9221 data exchange
void latchDataA12A13(void)
{
  PORT_DataA12 &=~ BIT_DataA12;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataA12 ^= BIT_DataA12;
  } 
} 

//latch routine for MY9221 data exchange
void latchDataA14A15(void)
{
  PORT_DataA14 &=~ BIT_DataA14;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataA14 ^= BIT_DataA14;
  } 
} 



//latch routine for MY9221 data exchange
void latchDataD2D3(void)
{
  PORT_DataD2 &=~ BIT_DataD2;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD2 ^= BIT_DataD2;
  } 
} 


//latch routine for MY9221 data exchange
void latchDataD4D5(void)
{
  PORT_DataD4 &=~ BIT_DataD4;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD4 ^= BIT_DataD4;
  } 
} 

//latch routine for MY9221 data exchange
void latchDataD6D7(void)
{
  PORT_DataD6 &=~ BIT_DataD6;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD6 ^= BIT_DataD6;
  } 
}

//latch routine for MY9221 data exchange
void latchDataD8D9(void)
{
  PORT_DataD8 &=~ BIT_DataD8;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD8 ^= BIT_DataD8;
  } 
} 

//latch routine for MY9221 data exchange
void latchDataD10D11(void)
{
  PORT_DataD10 &=~ BIT_DataD10;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD10 ^= BIT_DataD10;
  } 
} 

//latch routine for MY9221 data exchange
void latchDataD12D13(void)
{
  PORT_DataD12 &=~ BIT_DataD12;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD12 ^= BIT_DataD12;
  } 
}




//Send 12 road led brightness data
// Use for sending data to the led
// goes from 0x0000 (all leds OFF) to 0x03ff (all leds ON)

void sendLED(unsigned int LEDstate)
{
  unsigned char i;
  for(i=0;i<12;i++)
  {
    if(LEDstate&0x0001)
    {
      send16bitDataA12A13(ON);
      send16bitDataA14A15(ON);
      send16bitDataD2D3(ON);
      send16bitDataD4D5(ON);
      send16bitDataD6D7(ON);
      send16bitDataD8D9(ON);
      send16bitDataD10D11(ON);
      send16bitDataD12D13(ON);
    }
    else 
    {
      send16bitDataA12A13(SHUT);
      send16bitDataA14A15(SHUT);
      send16bitDataD2D3(SHUT);
      send16bitDataD4D5(SHUT);
      send16bitDataD6D7(SHUT);
      send16bitDataD8D9(SHUT);
      send16bitDataD10D11(SHUT);
      send16bitDataD12D13(SHUT);
    }
    // if(i!=11) // I don't know the use
    LEDstate=LEDstate>>1;
  }
}

// convert the value 0-127 to 0-1023 for the leds
int convert_to_led(unsigned int received) // received value by the sensors
{

  unsigned int sent=0x0000; // value to send to the leds

  if (received==0)
  {
    sent=0x00001;
      return sent;

  }
  else if(received>=1 && received<=15){

    sent=0x00002;
      return sent;

  }
  else if(received>=16 && received<=31){

    sent=0x00006;
      return sent;

  }
  else if(received>=32 && received<=47){

    sent=0x0000e;
      return sent;

  }
  else if(received>=48 && received<=63){

    sent=0x0001e;
      return sent;

  }
  else if(received>=64 && received<=79){

    sent=0x0003e;
      return sent;

  }
  else if(received>=80 && received<=95){

    sent=0x0007e;
      return sent;

  }     
  else if(received>=96 && received<=111){

    sent=0x000fe;
      return sent;

  }
  else if(received>=112 && received<127){

    sent=0x001fe;
      return sent;

  }
  else if(received==127){

    sent=0x003fe;
      return sent;

  }


}


























