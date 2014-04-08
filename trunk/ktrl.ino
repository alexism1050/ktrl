char foo; //trick for IDE.

#include "MIDI.h"

#define CENTIMETERSMIN 6 //min distance to the sensor 
#define CENTIMETERSMAX 29 //max distance to the sensor
#define NMEASURES 7

//exponential function approximation f(x)=a * (x/2)^b
#define FUNCTIONA  0.9847362291 //Function parameters
#define FUNCTIONB  -1.13405974 //Function parameters
float FUNCTIONMAXVALUE;
float FUNCTIONMINVALUE;

#define NSENSORS 8  
#define MIDISTEPS 127 
int MIDMIDISTEPS = MIDISTEPS / 2;

#define DEADZONEGENERAL 0.05 //% of deadzone to max and min value of sensors
#define DEADZONENOHAND 0.05 //% of memoryzone at the bottom of the sensors
#define DEADZONERELATIVE 0.05 //% of deathzone at the middle on relative mode

#define NTOPNBUTTONS 1 //number of the top buttons (Most important ones.)
#define CONFIGBUTTON 0

//sensors

int sensActiv[NSENSORS]; //used sensors
int sensPort[NSENSORS]; //arduino analog port of the sensor
float sensMidiSteps[NSENSORS]; //the steps for the midi on this sensor.
int sensRaw[NSENSORS]; //raw value readed
int sensRawMeasures[NSENSORS][NMEASURES];
float sensLinear[NSENSORS]; //raw to a linear value from 0.0 to 1.0
int sensRawRangMax[NSENSORS]; //ranges of the raw value
int sensRawRangMin[NSENSORS];
float sensLinDeadTop;
float sensLinDeadBot;
float sensLinDeadBotHand;
float sensLinDeadMidHand;
float sensLinDeadMidHandMax;
float sensLinDeadMidHandMin;
int sensMode[NSENSORS]; //mode of each sensor
int sensMidiPitch[NSENSORS];
int sensMidiNote[NSENSORS];
int sensMidiSend[NSENSORS]; //1 or 0 if this sensor need the midi signal to be sended

//buttons

int topButtonsValue[NTOPNBUTTONS];

// ------------ SHIFT OUT ---------
int latchPin = 19; 
int clockPin = 20; 
int dataPin = 21; 
byte leds = 0;
// ----------------------------

// ------------ BUTTONS ---------

 # define NSENSORS 8

# define NMODES 3

int groupButtonIN[NSENSORS];
int groupButtonValue[NSENSORS];

int modeButtonIN[NMODES];

int mode;
// ----------------------------


// ---------- BARGRAPH -----------

//D2-D3 
#define PORT_DataD2 PORTE
#define PORT_ClkD3 PORTE
#define DATA_PinD2 2 //DATA IN
#define CLK_PinD3 3 //CLK IN
#define BIT_DataD2 0x01<<4
#define BIT_ClkD3 0x01<<5


//D4-D5 
#define PORT_DataD4 PORTG
#define PORT_ClkD5 PORTE
#define DATA_PinD4 4 //DATA IN
#define CLK_PinD5 5 //CLK IN
#define BIT_DataD4 0x01<<5
#define BIT_ClkD5 0x01<<3

// D6- D7 
#define PORT_DataD6 PORTH
#define PORT_ClkD7 PORTH
#define DATA_PinD6 6 //DATA IN
#define CLK_PinD7 7 //CLK IN
#define BIT_DataD6 0x01<<3
#define BIT_ClkD7 0x01<<4

// D98- D9 
#define PORT_DataD8 PORTH
#define PORT_ClkD9 PORTH
#define DATA_PinD8 8 //DATA IN
#define CLK_PinD9 9 //CLK IN
#define BIT_DataD8 0x01<<5
#define BIT_ClkD9 0x01<<6

//D10-D11 
#define PORT_DataD10 PORTB
#define PORT_ClkD11 PORTB
#define DATA_PinD10 10 //DATA IN
#define CLK_PinD11 11 //CLK IN
#define BIT_DataD10 0x01<<4
#define BIT_ClkD11 0x01<<5


//D12-D13 
#define PORT_DataD12 PORTB
#define PORT_ClkD13 PORTB
#define DATA_PinD12 12 //DATA IN
#define CLK_PinD13 13 //CLK IN
#define BIT_DataD12 0x01<<6
#define BIT_ClkD13 0x01<<7


//D14-D15
#define PORT_DataD14 PORTJ
#define PORT_ClkD15 PORTJ
#define DATA_PinD14 14 //DATA IN
#define CLK_PinD15 15//CLK IN
#define BIT_DataD14 0x01<<0
#define BIT_ClkD15 0x01<<1

//D16-D17
#define PORT_DataD16 PORTH
#define PORT_ClkD17 PORTH
#define DATA_PinD16 16 //DATA IN
#define CLK_PinD17 17 //CLK IN
#define BIT_DataD16 0x01<<0
#define BIT_ClkD17 0x01<<1


#define CmdMode 0x0000 //Work on 8-bit mode
#define ON 0x00ff //8-bit 1 data
#define SHUT 0x0000 //8-bit 0 data

int sensSendMIDI_toLEDS[NSENSORS];
// ----------------------------


//functions

void readTopButtons();
void rawToLinear(int currentSen);
void sendMidi(int currentSen);
void setLigths();
void normalizeMidiPitch(int currentSen);
void debug(int currentSen);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void setup() {
    MIDI.begin();
    FUNCTIONMAXVALUE = FUNCTIONA * pow((CENTIMETERSMIN / 2.0), FUNCTIONB);
    FUNCTIONMINVALUE = FUNCTIONA * pow((CENTIMETERSMAX / 2.0), FUNCTIONB);
    for (int currentSens = 0; currentSens < NSENSORS; currentSens++) {
        sensActiv[currentSens] = 1;
        //sensPort[currentSens] = currentSens;
        sensMidiSteps[currentSens] = 1;
        sensRaw[currentSens] = 0;
        sensLinear[currentSens] = 0;
        sensRawRangMax[currentSens] = 330;
        sensRawRangMin[currentSens] = 120;
        sensLinDeadTop = 1 - (1 * DEADZONEGENERAL);
        sensLinDeadBot = 1 * DEADZONEGENERAL;
        sensLinDeadBotHand = sensLinDeadBot + (1 * DEADZONENOHAND);
        sensLinDeadMidHand = mapFloat(0.5, 0.0, 1.0, sensLinDeadBotHand, 1.0);
        sensLinDeadMidHandMax = sensLinDeadMidHand + (1 * DEADZONERELATIVE);
        sensLinDeadMidHandMin = sensLinDeadMidHand - (1 * DEADZONERELATIVE);
        sensMode[currentSens] = 0;
        sensMidiPitch[currentSens] = 0;
        sensMidiNote[currentSens] = 0;
        sensMidiSend[currentSens] = 0;
    }
   
  
   sensRawRangMin[0] = 130;
   sensRawRangMin[3] = 125;
   sensRawRangMin[4] = 125;
   /*sensRawRangMin[1] = 96;
   sensRawRangMin[2] = 88;
   sensRawRangMin[3] = 87;
   sensRawRangMin[4] = 87;
   sensRawRangMin[5] = 87;
   sensRawRangMin[6] = 87;
   sensRawRangMin[7] = 87;
   
   sensRawRangMax[0] = 330;
   sensRawRangMax[1] = 390;
   sensRawRangMax[2] = 390;
   sensRawRangMax[3] = 390;
   sensRawRangMax[4] = 390;
   sensRawRangMax[5] = 390;
   sensRawRangMax[6] = 390;
   sensRawRangMax[7] = 390;*/
   
    sensPort[0] = A15;
    sensPort[1] = A14;
    sensPort[2] = A12;
    sensPort[3] = A13;
    sensPort[4] = A2;
    sensPort[5] = A3;
    sensPort[6] = A1;
    sensPort[7] = A0;
    
   
    /*sensActiv[0] = 0;
    sensActiv[1] = 0;
    sensActiv[2] = 0;
    sensActiv[3] = 0;
    sensActiv[4] = 0;
    sensActiv[5] = 0;
    sensActiv[6] = 0;
    sensActiv[7] = 0;*/
  
  
  
  // Declaration of pins as OUTPUTS
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
  pinMode(DATA_PinD14,OUTPUT); //Data pin
  pinMode(CLK_PinD15,OUTPUT); //CLK pin
  pinMode(DATA_PinD16,OUTPUT); //Data pin
  pinMode(CLK_PinD17,OUTPUT); //CLK pin
  
  // ------------ SHIFT OUT ----------
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  //leds = 255;
  updateShiftRegister();
  // ----------------------------
  
    // ------------ BUTTONS ----------

   groupButtonIN[0]=31;
  groupButtonIN[1]=33;
  groupButtonIN[2]=35;
  groupButtonIN[3]=37;
  groupButtonIN[4]=39;
  groupButtonIN[5]=41;
  groupButtonIN[6]=43;
  groupButtonIN[7]=45;
  
  modeButtonIN[0]=28;
  modeButtonIN[1]=26;
  modeButtonIN[2]=24;
 

  for(int i=0; i<NSENSORS;i++){
   
    groupButtonValue[i]=0;
    pinMode(groupButtonIN[i], INPUT);    // declare pushbutton as input  
  }
     mode=0;

  for(int i=0; i<NMODES;i++){
   
   pinMode(modeButtonIN[i], INPUT);    // declare pushbutton as input  
  }
    // ---------------------------- 
}

void loop() {
    
    readModeButtons();
    readGroupButtons();


        for (int currentMeasure = 0; currentMeasure < NMEASURES; currentMeasure++) {
            for (int currentSen = 0; currentSen < NSENSORS; currentSen++) {
                if (sensActiv[currentSen] == 1) {
                    sensRawMeasures[currentSen][currentMeasure] = analogRead(sensPort[currentSen]);
                    delayMicroseconds(2063);

                 
                }
            }
        }
        
        
        for (int currentSen = 0; currentSen < NSENSORS; currentSen++) {
            if (sensActiv[currentSen] == 1) {
                int done = 0;
                while (done != 1) {
                    done = 1;
                    for (int j = 0; j < (NMEASURES); j++) {
                        if (sensRawMeasures[j] > sensRawMeasures[(j + 1)]) {
                            int temp = sensRawMeasures[currentSen][(j + 1)];
                            sensRawMeasures[currentSen][(j + 1)] = sensRawMeasures[currentSen][j];
                            sensRawMeasures[currentSen][j] = temp;
                            done = 0;
                        }
                    }
                }
                sensRaw[currentSen] = (sensRawMeasures[currentSen][(NMEASURES / 2) + 1]+ sensRawMeasures[currentSen][(NMEASURES / 2)] +sensRawMeasures[currentSen][(NMEASURES / 2) - 1])/3;

                //sensRaw[currentSen] = analogRead(sensPort[currentSen]);
                rawToLinear(currentSen);
                linearToMidi(currentSen);
                sendMidi(currentSen);
                setLigths(currentSen); 
                sendFrontLEDS(currentSen);
                //delay(1);
                //debugMode();
                //debug(currentSen);
            }
        }
    
}


//functions

void sendMidi(int currentSen) {
  if(sensMidiSend[currentSen] == 1)
  {
    int CC = currentSen + 20;
    MIDI.sendControlChange(CC,sensMidiPitch[currentSen],1);
    sensMidiSend[currentSen] = 0;
  }
}

void readTopButtons() {
    for (int currentTopButton = 0; currentTopButton < NTOPNBUTTONS; currentTopButton++) {
    }
}

void linearToMidi(int currentSen) {
  
  
    if (mode == 0) { //absolute  
        if (sensLinear[currentSen] >= sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadBot) {
            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBot, sensLinDeadTop, 0.0, 1.0);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensMidiPitch[currentSen] != 0) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        }
    } else if (mode == 1) { //absolute with memory   
        if (sensLinear[currentSen] >= sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadBotHand) {
            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadTop, 0.0, 1.0);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadBot) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        }
    } else if (mode == 2) { //NEW mode
    
    
             for(int i=0; i<NSENSORS;i++){
       
       if(groupButtonValue[i]==1){ 
         sensMidiSend[i] = 1;
         sensMidiPitch[i]=0;       
       }
     } 
    
    
       /* if (sensLinear[currentSen] <= sensLinDeadMidHandMin && sensLinear[currentSen] >= sensLinDeadBotHand) { //--

            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadMidHandMin, 0, 0.5);

        } else if (sensLinear[currentSen] >= sensLinDeadMidHandMax) { //++

            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadMidHandMax, 1, 0.5, 1);

        } else {

        }*/
        
     mode=1;
    }
    
     if(groupButtonValue[currentSen]==0 && sensMidiPitch[currentSen]>0){
     for(int i=0; i<NSENSORS;i++){
       
       if(groupButtonValue[i]==0 && mode==1){
         
         sensMidiSend[i] = 1;
         sensMidiPitch[i]=sensMidiPitch[currentSen];       
       }
     } 
    }
 
}




void normalizeMidiPitch(int currentSen) {

}

void debug(int currentSen) {

   /* Serial.begin(9600);
    Serial.print("SEN: ");
    Serial.print(currentSen);
    Serial.print(" RAW: \t");
    Serial.print(sensRaw[currentSen]);
    Serial.print(" Linear: \t");
    Serial.print(sensLinear[currentSen]);

    Serial.println(" ");*/

}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {

    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

}

void rawToLinear(int currentSen) {

    //exponential function approximation f(x) = a * (x/2)^b  // y = a * (x/2)^b
    //to get x from a y value  // x=(((2^b)*y)/a)^(1/b)
    //FUNCTIONMAXVALUE = FUNCTIONA * pow((CENTIMETERSMIN / 2.0), FUNCTIONB);

    float inRangeRaw = mapFloat(sensRaw[currentSen], sensRawRangMin[currentSen], sensRawRangMax[currentSen], FUNCTIONMINVALUE, FUNCTIONMAXVALUE);
    double distance = pow((((pow(2, FUNCTIONB)) * inRangeRaw) / FUNCTIONA), (1 / FUNCTIONB));
    float linearValue = mapFloat(distance, CENTIMETERSMIN, CENTIMETERSMAX, 1.0, 0.0);
    linearValue = constrain(linearValue, 0.0, 1.0);
    sensLinear[currentSen] = linearValue;

    /*Serial.begin(9600);
    Serial.print("SEN: ");
    Serial.print(currentSen);
    Serial.print(" RAW: \t");
    Serial.print(sensRaw[currentSen]);
    Serial.print(" inra: \t");
    Serial.print(inRangeRaw);
    Serial.print(" distance: \t");
    Serial.print(distance);
    Serial.print(" Linear: \t");
    Serial.print(sensLinear[currentSen]);
    Serial.print(" Midi: \t");
    Serial.print(sensMidiPitch[currentSen]);
    Serial.println(" ");*/
}



void setLigths(int currentSen){

    
       sensSendMIDI_toLEDS[currentSen]=convert_to_led(sensMidiPitch[currentSen]);
      
      // sensSendMIDI_toLEDS[0]=0;
      
      switch(currentSen){
      
      case 0:   
      
      // BARGRAPH 0
      /*while(sensSendMIDI_toLEDS[0]<=1023)
      {*/
          send16bitDataD2D3(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD2D3(); //make it come into effect
      
      /*   send16bitDataD4D5(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD4D5(); //make it come into effect
      
          send16bitDataD6D7(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD6D7(); //make it come into effect
      
          send16bitDataD8D9(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD8D9(); //make it come into effect
      
          send16bitDataD10D11(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD10D11(); //make it come into effect
      
          send16bitDataD12D13(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD12D13(); //make it come into effect
      
          send16bitDataD14D15(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          //  send16bitData(CmdMode); //set second LED Bar mode,if you do not use two LED Bar work together(connect one by one),you can delete this line.
          //  sendLED(i); //send second LED Bar data,if you do not use two LED Bar work together(connect one by one),you can delete this line.
          latchDataD14D15(); //make it come into effect
      
          send16bitDataD16D17(CmdMode); //set first LED Bar mode
          sendLED(sensSendMIDI_toLEDS[0]); //send first LED Bar data
          latchDataD16D17(); //make it come into effect
      sensSendMIDI_toLEDS[0]=(sensSendMIDI_toLEDS[0]*2)+1;
      delay(500);
      }*/
      break;
    
    case 1:
        send16bitDataD4D5(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[1]); //send first LED Bar data
        latchDataD4D5(); //make it come into effect
    break;
    
    case 2:
        send16bitDataD6D7(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[2]); //send first LED Bar data
        latchDataD6D7(); //make it come into effect
    break;
    
    case 3:
        send16bitDataD8D9(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[3]); //send first LED Bar data
        latchDataD8D9(); //make it come into effect
    break;
    
    case 4:
        send16bitDataD10D11(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[4]); //send first LED Bar data
        latchDataD10D11(); //make it come into effect
    break;
    
    case 5:
        send16bitDataD12D13(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[5]); //send first LED Bar data
        latchDataD12D13(); //make it come into effect
    break;
    
    case 6:
        send16bitDataD14D15(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[6]); //send first LED Bar data
        //  send16bitData(CmdMode); //set second LED Bar mode,if you do not use two LED Bar work together(connect one by one),you can delete this line.
        //  sendLED(i); //send second LED Bar data,if you do not use two LED Bar work together(connect one by one),you can delete this line.
        latchDataD14D15(); //make it come into effect
    break;
    
    case 7:
        send16bitDataD16D17(CmdMode); //set first LED Bar mode
        sendLED(sensSendMIDI_toLEDS[7]); //send first LED Bar data
        latchDataD16D17(); //make it come into effect
    break;
    
    default :
    break;
    
    }
      //i=i*2+1; // increment i in order to get the leds ON one by one
    

}





//Send 16_bit data on D14 D15
void send16bitDataD14D15(unsigned int dataD14)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD14&0x8000)
    {
      PORT_DataD14 |= BIT_DataD14;
    }
    else
    {
      PORT_DataD14 &=~ BIT_DataD14;
    }
    PORT_ClkD15 ^= BIT_ClkD15;
    dataD14 <<= 1;
  }
}

//Send 16_bit data on D16 D17
void send16bitDataD16D17(unsigned int dataD16)
{
  for(unsigned char i=0;i<16;i++)
  {
    if(dataD16&0x8000)
    {
      PORT_DataD16 |= BIT_DataD16;
    }
    else
    {
      PORT_DataD16 &=~ BIT_DataD16;
    }
    PORT_ClkD17^= BIT_ClkD17;
    dataD16 <<= 1;
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
void latchDataD14D15(void)
{
  PORT_DataD14 &=~ BIT_DataD14;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD14 ^= BIT_DataD14;
  } 
} 

//latch routine for MY9221 data exchange
void latchDataD16D17(void)
{
  PORT_DataD16 &=~ BIT_DataD16;
  delayMicroseconds(10);

  for(unsigned char i=0;i<8;i++)
  {
    PORT_DataD16 ^= BIT_DataD16;
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
      send16bitDataD2D3(ON);
      send16bitDataD4D5(ON);
      send16bitDataD6D7(ON);
      send16bitDataD8D9(ON);
      send16bitDataD10D11(ON);
      send16bitDataD12D13(ON);      
      send16bitDataD14D15(ON);
      send16bitDataD16D17(ON);
    }
    else 
    {
      send16bitDataD2D3(SHUT);     
      send16bitDataD4D5(SHUT);
      send16bitDataD6D7(SHUT);
      send16bitDataD8D9(SHUT);
      send16bitDataD10D11(SHUT);
      send16bitDataD12D13(SHUT);
      send16bitDataD14D15(SHUT);
      send16bitDataD16D17(SHUT);
 
    }
     if(i!=11) // I don't know the use
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




  // ------------ SHIFT OUT ----------


void sendFrontLEDS(int currentSen)
{
  
  
  // To use if we want to control leds one by one
  // infos http://learn.adafruit.com/adafruit-arduino-lesson-4-eight-leds/brightness-control
  
  if(sensMidiPitch[currentSen]>0){
    bitSet(leds, currentSen);
    updateShiftRegister();
    //delay(500);
  }
  else{
     bitClear(leds, currentSen);
    updateShiftRegister();
  }

  

}


 
// not used if all the leds are controlled at the same time 
void updateShiftRegister()
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, leds);
   digitalWrite(latchPin, HIGH);
}


  // ----------------------------






  // ------------ BUTTONS ----------


void debugGroup(int num){
 /*
    Serial.print("Button ");
    Serial.print(num); 
    Serial.print("\t");
    Serial.print(groupButtonValue[num]);
      Serial.println("");
     //delay(1);*/

}

void debugMode(){
/*
    Serial.print("Mode ");
    Serial.print("\t"); 
    Serial.print(mode);
    Serial.print("\n");
    
    delay(50);*/
}

void readGroupButtons(){

 for(int i=0;i<NSENSORS;i++){
  
   groupButtonValue[i]=digitalRead(groupButtonIN[i]);
// debugGroup(i);
  }
}

void readModeButtons(){
  
 for(int i=0;i<NMODES;i++){
  
   if(digitalRead(modeButtonIN[i])){

     switch(i){
      
       case 0:
             mode=0;
         break;
        case 1:
             mode=1;
         break;     
       case 2:
             mode=2;
         break;
       default:
             mode=0;
         break;       
     }
      
   }
  }
  
}


  // ----------------------------






































