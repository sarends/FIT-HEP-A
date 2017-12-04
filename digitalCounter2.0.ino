// Last modified 2017/12/04 by Sarah Arends
// counts high voltage signals from input pin, performs averages periodically

const int  input = 2;    // pin where input is attached

int counter = 0;         // total number of counts
float currentstate = 0;  // current state of input signal
float laststate = 0;     // previous state of the signal
float rateInst, rateAvg;
float lastTime, currentTime;
int countInterval = 10;  // determines frequency of statistics calculations

void setup() {
  
  // initialize the pin as a input
  pinMode(input, INPUT);
  
  // initialize serial communication
  Serial.begin(9600);

}

void loop() {

  // print and store value from input
  currentstate = digitalRead(input);
  Serial.println(currentstate);
  
  // runs if state has changed to high
  if ( (currentstate != laststate) && (currentstate == HIGH) ) {
     
      counter++;

      // runs statistics at periodic interval
      if (counter %countInterval == 0) {

        currentTime = millis();
        
        //Serial.print(counter + " counts, ");
        
        rateAvg =  float(counter) / currentTime;
        //Serial.print(rateAvg*1000 + " Hz (avg), ");
        
        rateInst = float(countInterval) / (currentTime-lastTime);
        //Serial.println(rateInst*1000 + " Hz (inst), ");

        lastTime = currentTime;
        
      }
    }
  
  // store previous state
  laststate = currentstate;
  
  // reset counter once at maximum capacity
  if (counter == 32000) {
    counter=0;
    Serial.println(" RESET ");
  }

}
