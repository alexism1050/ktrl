char foo; //trick for IDE.

#define NMEASURES 500

int sensRawMeasures[NMEASURES];
int sensPort[8];

void setup() {
  Serial.begin(9600);
  sensPort[0] = A15;
  sensPort[1] = A14;
  sensPort[2] = A12;
  sensPort[3] = A13;
  sensPort[4] = A2;
  sensPort[5] = A3;
  sensPort[6] = A1;
  sensPort[7] = A0;
}

void loop(){

  for (int currentMeasure = 0; currentMeasure < NMEASURES; currentMeasure++) {    
    sensRawMeasures[currentMeasure] = analogRead(sensPort[0]);
    //Serial.println(sensRawMeasures[currentMeasure]);
    delay(25);
  }



  int done = 0;
  while (done != 1) {
    done = 1;
    for (int j = 0; j < (NMEASURES); j++) {
      if (sensRawMeasures[j] > sensRawMeasures[(j + 1)]) {
        int temp = sensRawMeasures[(j + 1)];
        sensRawMeasures[(j + 1)] = sensRawMeasures[j];
        sensRawMeasures[j] = temp;
        done = 0;
      }
    }
  }



  Serial.println("MODA");
  Serial.println(sensRawMeasures[(NMEASURES/2)-1]);

  delay(100);
}

