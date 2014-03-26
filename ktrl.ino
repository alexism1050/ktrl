char foo; //trick for IDE.

#define CENTIMETERSMIN 6 //min distance to the sensor 
#define CENTIMETERSMAX 28 //max distance to the sensor

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
float sensLinear[NSENSORS]; //raw to a linear value from 0.0 to 1.0

int sensRawRangMax[NSENSORS]; //ranges of the raw value
int sensRawRangMin[NSENSORS];

//float sensRawFunctionMax[NSENSORS]; //ranges of the function value
//float sensRawFunctionMin[NSENSORS];

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

//functions

void readTopButtons();
void rawToLinear(int currentSen);
void sendMidi(int currentSen);
void setLigths();
void normalizeMidiPitch(int currentSen);

void debug(int currentSen);
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max);

void setup() {
    Serial.begin(9600);

    FUNCTIONMAXVALUE = FUNCTIONA * pow((CENTIMETERSMIN / 2.0), FUNCTIONB);
    FUNCTIONMINVALUE = FUNCTIONA * pow((CENTIMETERSMAX / 2.0), FUNCTIONB);

    for (int currentSens = 0; currentSens < NSENSORS; currentSens++) {

        sensActiv[currentSens] = 1;
        sensPort[currentSens] = currentSens;
        sensMidiSteps[currentSens] = 1;

        sensRaw[currentSens] = 0;
        sensLinear[currentSens] = 0;

        sensRawRangMax[currentSens] = 390;
        sensRawRangMin[currentSens] = 92;

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
                rawToLinear(currentSen);
                linearToMidi(currentSen);
                setLigths();
                sendMidi();
                delay(10);
                //debug(currentSen);
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
        /*
        if (sensLinear[currentSen] >= sensLinDeadBotHand && sensLinear[currentSen] <= sensLinDeadTop) {
            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadTop, 0, 1);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] < sensLinDeadBotHand && sensLinear[currentSen] >= sensLinDeadBot) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else if (sensMidiPitch[currentSen] != 0) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else {
            //do nothing
        }
         */

        if (sensLinear[currentSen] >= sensLinDeadBotHand && sensLinear[currentSen] <= sensLinDeadTop) {
            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadTop, 0, 1);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] < sensLinDeadBotHand && sensLinear[currentSen] >= sensLinDeadBot) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else if (sensMidiPitch[currentSen] != 0) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else {
            //do nothing
        }

    } else if (sensMode[currentSen] == 1) { //absolute with memory

        if (sensLinear[currentSen] >= sensLinDeadBotHand && sensLinear[currentSen] <= sensLinDeadTop) {
            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadTop, 0, 1);
            sensMidiPitch[currentSen] = inRange * MIDISTEPS;
            normalizeMidiPitch(currentSen);
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] > sensLinDeadTop) {
            sensMidiPitch[currentSen] = MIDISTEPS;
            sensMidiSend[currentSen] = 1;
        } else if (sensLinear[currentSen] < sensLinDeadBotHand && sensLinear[currentSen] >= sensLinDeadBot) {
            sensMidiPitch[currentSen] = 0;
            sensMidiSend[currentSen] = 1;
        } else {
            //do nothing
        }
    } else if (sensMode[currentSen] == 2) { //relative mode
        if (sensLinear[currentSen] <= sensLinDeadMidHandMin && sensLinear[currentSen] >= sensLinDeadBotHand) { //--

            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadBotHand, sensLinDeadMidHandMin, 0, 0.5);

        } else if (sensLinear[currentSen] >= sensLinDeadMidHandMax) { //++

            float inRange = mapFloat(sensLinear[currentSen], sensLinDeadMidHandMax, 1, 0.5, 1);

        } else {

        }
    }
}

void sendMidi(int currentSen) {

    sensMidiSend[currentSen] = 0;
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

    Serial.println(" ");

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
    constrain(linearValue, 0.0, 1.0);
    sensLinear[currentSen] = linearValue;

    Serial.begin(9600);
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
    Serial.println(" ");
}







































