char foo; //trick for IDE.

#define NSENSORS 8 
#define MIDISTEPS 127 
#define DEADZONEGENERAL 0.05 //% of deadzone to max and min value of sensors
#define DEADZONEMEMORY 0.05 //% of memoryzone at the bottom of the sensors
#define DEADZONERELATIVE 0.05 //% of deathzone at the middle on relative mode

#define NTOPNBUTTONS 1 
#define CONFIGBUTTON 0

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

int sensRangMidMaxMem[NSENSORS]; //range at the mid
int sensRangMidMinMem[NSENSORS]; 


int sensMode[NSENSORS]; //mode of each sensor

//buttons

int topButtonsValue[NTOPNBUTTONS]; 

//auxiliars

float rawToPitchSteps[NSENSORS];  //raw to pitch step
float rawToPitchStepsMem[NSENSORS];  //raw to pitch step with memory range

//functions

void readTopButtons();

void rawToMidi(int currentSen);
void sendMidis();
void setLigths();

void normalizeMidiPitch(int currentSen);

void debug(int currentSen);

void setup()
{
  Serial.begin(9600);  
  for (int i = 0; i<NSENSORS; i++) {

    MIDMIDISTEPS = MIDISTEPS / 2;

    sensActiv[i]=1;

    sensPort[i]=i;    

    sensRaw[i]=0;

    sensMidiSteps[i]= MIDISTEPS/MIDISTEPS;

    sensRangMax[i]=600;
    sensRangMin[i]=20;
    sensRangMem[i]=20;

    sensRangMidMem[i]=(sensRangMax[i]-(sensRangMin[i]+sensRangMem[i]))/2;    

    sensRangMidMaxMem[i]=sensRangMidMem[i] + (sensRangMidMem[i]*DEADZONERELATIVE); 
    sensRangMidMinMem[i]=sensRangMidMem[i] - (sensRangMidMem[i]*DEADZONERELATIVE); 

    sensMode[i]=0;

    rawToPitchSteps[i] = (sensRangMax[i] - sensRangMin[i])/(MIDISTEPS+0.0);
    rawToPitchStepsMem[i] = (sensRangMax[i] - (sensRangMin[i]+sensRangMem[i]))/(MIDISTEPS+0.0);

  }

  sensMode[0]=2;
  sensActiv[1]=0;
  sensActiv[2]=0;
  sensActiv[3]=0;
  sensActiv[4]=0;
  sensActiv[5]=0;
  sensActiv[6]=0;
  sensActiv[7]=0;

}

void loop()
{ 
  readTopButtons();
  if(topButtonsValue[CONFIGBUTTON]==1){ //configuration mode
  }
  else { //normal use mode
    for (int currentSen = 0; currentSen<NSENSORS; currentSen++) {
      if(sensActiv[currentSen]==1){
        sensRaw[currentSen] = analogRead(sensPort[currentSen]);
        rawToMidi(currentSen);        
        setLigths();  
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
      //if(sensRaw[currentSen]>sensRangMidMaxMem[currentSen] || sensRaw[currentSen]<sensRangMidMinMem[currentSen]){
      int relativeAux = (sensRaw[currentSen]-(sensRangMin[currentSen]+sensRangMem[currentSen])) / rawToPitchStepsMem[currentSen];
      if (relativeAux > MIDMIDISTEPS){ 
        sensMidiPitch[currentSen] += (relativeAux - MIDMIDISTEPS);
      }
      else if (relativeAux < MIDMIDISTEPS){ 
        sensMidiPitch[currentSen] -= (MIDISTEPS -  relativeAux);
      }
      normalizeMidiPitch(currentSen);
      sensSendMIDI[currentSen] = 1;

      // }

    }
  }
}
void sendMidis(){
}
void setLigths(){
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








































