char foo; //trick for IDE.

#define NSENSORS 8  
#define MIDISTEPS 127 
int MIDMIDISTEPS = (int) (MIDISTEPS / 2.0);

#define DEADZONEGENERAL 0.05 //% of deadzone to max and min value of sensors
#define DEADZONEMEMORY 0.05 //% of memoryzone at the bottom of the sensors
#define DEADZONERELATIVE 0.05 //% of deathzone at the middle on relative mode

#define NTOPNBUTTONS 1 //number of the top buttons (Most important ones.)
#define CONFIGBUTTON 0

//sensors

int sensActiv[NSENSORS]; //used sensors
int sensPort[NSENSORS]; //arduino analog port of the sensor

float sensMidiSteps[NSENSORS]; //the steps for the midi on this sensor.

int sensRaw[NSENSORS]; //raw value readed
float sensLinear[NSENSORS]; //raw to a linear value

int sensRawRangMax[NSENSORS]; //ranges of the raw value
int sensRawRangMin[NSENSORS];

float sensLinDeadTop;
float sensLinDeadBot;
float sensLinDeadBotMem;

float sensLinDeadMid;
float sensLinDeadMidMax;
float sensLinDeadMidMin;

int sensMode[NSENSORS]; //mode of each sensor

int sensMidiPitch[NSENSORS];
int sensMidiNote[NSENSORS];
int sensMidiSend[NSENSORS]; //1 or 0 if this sensor need the midi signal to be sended

//buttons

int topButtonsValue[NTOPNBUTTONS];

//functions

void readTopButtons();
void rawToLinear(int currentSen);
void sendMidis();
void setLigths();
void normalizeMidiPitch(int currentSen);

void debug(int currentSen);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void setup() {
    Serial.begin(9600);

    for (int currentSens = 0; currentSens < NSENSORS; currentSens++) {

        sensActiv[currentSens] = 1;
        sensPort[currentSens] = currentSens;
        sensMidiSteps[currentSens] = 1;

        sensRaw[currentSens] = 0;
        sensLinear[currentSens] = 0;

        sensRawRangMax[currentSens] = 650;
        sensRawRangMin[currentSens] = 40;

        sensLinDeadTop = 1 - (1 * DEADZONEGENERAL);
        sensLinDeadBot = 1 * DEADZONEGENERAL;
        sensLinDeadBotMem = sensLinDeadBot + (1 * DEADZONEMEMORY);

        //sensLinDeadMid = mapfloat();

        sensLinDeadMidMax = sensLinDeadMid + (sensLinDeadMid * DEADZONERELATIVE);
        sensLinDeadMidMin = sensLinDeadMid - (sensLinDeadMid * DEADZONERELATIVE);

        sensMode[currentSens] = 0;
        sensMidiPitch[currentSens] = 0;
        sensMidiNote[currentSens] = 0;
        sensMidiSend[currentSens] = 0;
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
                //rawToLinear(currentSen);
                linearToMidi(currentSen);
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

void linearToMidi(int currentSen) {
    if (sensMode[currentSen] == 0) { //absolute  
        if (sensLinear[currentSen] >= sensLinDeadBot && sensLinear[currentSen] <= sensLinDeadTop) {
            int inRange = mapFloat(sensLinear[currentSen], sensLinDeadBot, sensLinDeadTop, 0, 1);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
        } else if (sensLinear[currentSen] > sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
        } else {

        }
        sensMidiSend[currentSen] = 1;

    } else if (sensMode[currentSen] == 1) { //absolute with memory

        if (sensLinear[currentSen] >= sensLinDeadBotMem && sensLinear[currentSen] <= sensLinDeadTop) {
            int inRange = map(sensLinear[currentSen], sensLinDeadBotMem, sensLinDeadTop, 0, 1);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] < sensLinDeadBotMem && sensLinear[currentSen] > sensLinDeadBot) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else {

        }
    } else if (sensMode[currentSen] == 2) { //relative mode
        if (sensLinear[currentSen] <= sensLinDeadMidMin) { //--
            int inRange = map(sensLinear[currentSen], sensLinDeadBotMem, sensLinDeadMidMin, 0, 1);

            sensLinear[currentSen] - (sensLinDeadMidMin - sensLinear[currentSen]);
        } else if (sensLinear[currentSen] >= sensLinDeadMidMax) { //++
            sensLinear[currentSen] + (sensLinear[currentSen] - sensLinDeadMidMax);
        } else {

        }
    }
}

void sendMidis() {
}

void setLigths() {
}

void normalizeMidiPitch(int currentSen) {

}

void debug(int currentSen) {

    Serial.begin(9600);
    Serial.print("SEN: ");
    Serial.print(currentSen);
    Serial.print(" RAW: \t");
    Serial.print(sensRaw[currentSen]);
    Serial.print(" Linear: \t");
    Serial.print(sensLinear[currentSen]);

    Serial.println(" .");

}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
    return (float) (x - in_min) * (out_max - out_min) / (float) (in_max - in_min) + out_min;
}







































