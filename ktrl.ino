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

int sensActiv[NSENSORS]; //used sensors
int sensPort[NSENSORS]; //arduino analog port of the sensor

int sensRaw[NSENSORS]; //raw value readed
int sensMidiPitch[NSENSORS];
int sensMidiNote[NSENSORS];
int sensSendMIDI[NSENSORS]; //1 or 0 if this sensor need the midi signal to be sended

int sensMidiSteps[NSENSORS]; //midi steps that the sensor will use

int sensRangMax[NSENSORS]; //ranges
int sensRangMaxDead[NSENSORS];

int sensRangMin[NSENSORS];
int sensRangMinDead[NSENSORS];

int sensRangMem[NSENSORS]; //memory range at bottom.
int sensRangMemDead[NSENSORS]; //memory range at bottom.

int sensRangMidMem[NSENSORS]; //value at the mid of the range with the memRange.
int sensRangMidMemMax[NSENSORS]; //range at the mid
int sensRangMidMemMin[NSENSORS];

int sensMode[NSENSORS]; //mode of each sensor

//buttons

int topButtonsValue[NTOPNBUTTONS];

//functions

void readTopButtons();

void rawToMidi(int currentSen);
void sendMidis();
void setLigths();

void normalizeMidiPitch(int currentSen);

void debug(int currentSen);

void setup() {
    Serial.begin(9600);
    for (int currentSens = 0; currentSens < NSENSORS; currentSens++) {

        MIDMIDISTEPS = MIDISTEPS / 2;

        sensActiv[currentSens] = 1;
        sensPort[currentSens] = currentSens;
        sensRaw[currentSens] = 0;
        sensMidiPitch[currentSens] = 0;
        sensSendMIDI[currentSens] = 0;
        sensMidiSteps[currentSens] = MIDISTEPS / MIDISTEPS;

        sensRangMax[currentSens] = 600;
        sensRangMem[currentSens] = 20;
        sensRangMin[currentSens] = 10;

        sensRangMaxDead[currentSens] = sensRangMax[currentSens] -((sensRangMax[currentSens] - sensRangMin[currentSens]) * DEADZONEGENERAL);
        sensRangMemDead[currentSens] = sensRangMem[currentSens] +((sensRangMax[currentSens] - sensRangMin[currentSens]) * DEADZONEMEMORY);
        sensRangMinDead[currentSens] = sensRangMin[currentSens] +((sensRangMax[currentSens] - sensRangMin[currentSens]) * DEADZONEGENERAL);

        sensRangMidMem[currentSens] = (sensRangMax[currentSens]-(sensRangMin[currentSens] + sensRangMem[currentSens])) / 2;
        sensRangMidMemMax[currentSens] = sensRangMidMem[currentSens] + (sensRangMidMem[currentSens] * DEADZONERELATIVE);
        sensRangMidMemMin[currentSens] = sensRangMidMem[currentSens] - (sensRangMidMem[currentSens] * DEADZONERELATIVE);

        sensMode[currentSens] = 0;
    }

    sensMode[0] = 0;
    sensActiv[1] = 0;
    sensActiv[2] = 0;
    sensActiv[3] = 0;
    sensActiv[4] = 0;
    sensActiv[5] = 0;
    sensActiv[6] = 0;
    sensActiv[7] = 0;
}

void loop() {
    readTopButtons();
    if (topButtonsValue[CONFIGBUTTON] == 1) { //configuration mode
    } else { //normal use mode
        for (int currentSen = 0; currentSen < NSENSORS; currentSen++) {
            if (sensActiv[currentSen] == 1) {
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

void readTopButtons() {
    for (int currentTopButton = 0; currentTopButton < NTOPNBUTTONS; currentTopButton++) {
    }
}

void rawToMidi(int currentSen) {
    if (sensMode[currentSen] == 0) { //absolute   
        if (sensRaw[currentSen] <= sensRangMaxDead[currentSen] && sensRaw[currentSen] >= sensRangMinDead[currentSen]) {
            sensMidiPitch[currentSen] = (sensRaw[currentSen] - sensRangMinDead[currentSen]) / ((sensRangMaxDead[currentSen] - sensRangMinDead[currentSen]) / (MIDISTEPS + 0.0));
        } else if (sensRaw[currentSen] > sensRangMaxDead[currentSen]) {
            sensMidiPitch[currentSen] = MIDISTEPS;
        } else {
            sensMidiPitch[currentSen] = 0;
        }
        normalizeMidiPitch(currentSen);
        sensSendMIDI[currentSen] = 1;
    } else if (sensMode[currentSen] == 1) { //absolute with memory
        if (sensRaw[currentSen] <= sensRangMaxDead[currentSen] && sensRaw[currentSen] >= sensRangMemDead[currentSen]) {
            sensMidiPitch[currentSen] = (sensRaw[currentSen]-sensRangMinDead[currentSen]) / ((sensRangMaxDead[currentSen] - sensRangMinDead[currentSen]) / (MIDISTEPS + 0.0))+0.0;
            sensSendMIDI[currentSen] = 1;
        } else if (sensRaw[currentSen] > sensRangMaxDead[currentSen]) {
            sensMidiPitch[currentSen] = MIDMIDISTEPS;
            sensSendMIDI[currentSen] = 1;
        } else if (sensRaw[currentSen] < sensRangMemDead[currentSen] && sensRaw[currentSen] >= sensRangMem[currentSen]) {
            sensMidiPitch[currentSen] = 0;
            sensSendMIDI[currentSen] = 1;
        } else {
            sensSendMIDI[currentSen] = 0;
        }
        normalizeMidiPitch(currentSen);

    } else if (sensMode[currentSen] == 2) { //relative mode
        if (sensRaw[currentSen] > sensRangMem[currentSen]) {
            //if(sensRaw[currentSen]>sensRangMidMaxMem[currentSen] || sensRaw[currentSen]<sensRangMidMinMem[currentSen]){
            int relativeAux = (sensRaw[currentSen]-(sensRangMin[currentSen] + sensRangMem[currentSen])) / 0;
            if (relativeAux > MIDMIDISTEPS) {
                sensMidiPitch[currentSen] += (relativeAux - MIDMIDISTEPS);
            } else if (relativeAux < MIDMIDISTEPS) {
                sensMidiPitch[currentSen] -= (MIDISTEPS - relativeAux);
            }
            normalizeMidiPitch(currentSen);
            sensSendMIDI[currentSen] = 1;

            // }

        }
    }
}

void sendMidis() {
}

void setLigths() {
}

void normalizeMidiPitch(int currentSen) {
    //TODO sensMidiPitch[currentSen]%sensMidiSteps[currentSen];	
    if (sensMidiPitch[currentSen] > MIDISTEPS) {
        sensMidiPitch[currentSen] = MIDISTEPS;
    } else if (sensMidiPitch[currentSen] < 0) {
        sensMidiPitch[currentSen] = 0;
    }

}

void debug(int currentSen) {

    Serial.begin(9600);
    Serial.print("SEN: ");
    Serial.print(currentSen);
    Serial.print(" RAW: \t");
    Serial.print(sensRaw[currentSen]);
    Serial.print("\tP: \t");
    Serial.print(sensMidiPitch[currentSen]);
    Serial.print("\tRMAX: \t");
    Serial.print(sensRangMax[currentSen]);
    Serial.print("\tRMAXDEAD: \t");
    Serial.print(sensRangMaxDead[currentSen]);
    Serial.print("\tRMIN: \t");
    Serial.print(sensRangMin[currentSen]);
    Serial.print("\tMINRANGEDEAD: \t");
    Serial.print(sensRangMinDead[currentSen]);

    Serial.println(" .");

}








































