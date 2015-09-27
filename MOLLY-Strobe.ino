

const int buttonTempoPin = 2;
int buttonTempoState = 0;
int buttonTempoLastState = 0;

const int buttonModePin = 3;
int buttonModeState = 0;
int buttonModeLastState = 0;

const int relayPin = 4;

unsigned long tapTimeFirst;
unsigned long tapTimePrevious;
float tapCount = 0;
float tapBPM = 120;

unsigned long lastStrobeTime;
int strobeDuration = 1000;
int strobeState = LOW;
int lastStrobeState = LOW;


int currentMode = 0;
// 0 = Off
// 1 = On
// 2 = Strobe
// 3 = Strobe short

void setup(){
  pinMode(buttonTempoPin, INPUT);
  pinMode(buttonModePin, INPUT);
  pinMode(relayPin, OUTPUT);
  
  Serial.begin(9600);
}

void loop(){
  
  buttonTempoState = digitalRead(buttonTempoPin);
  buttonModeState = digitalRead(buttonModePin);
  
  if( buttonModeState != buttonModeLastState ){
    
    
    if( buttonModeState == HIGH ){
      
      currentMode++;
      
      if( currentMode > 3 ){
        currentMode = 0;
      }
      
      switchedMode();
    }
    
    buttonModeLastState = buttonModeState;
  }
  
  
  
  if( buttonTempoState != buttonTempoLastState ){
    
    if( buttonTempoState == HIGH ){
      tappedTempo();
    }
    
    buttonTempoLastState = buttonTempoState;
  }
  
  if( currentMode == 2 || currentMode == 3 ){
    strobe();
  }
  
}

void switchedMode(){
  if( currentMode == 0 ){
    digitalWrite(relayPin, LOW);
  }
  
  if( currentMode == 1 ){
    digitalWrite(relayPin, HIGH);
  }
}

void strobe(){
  unsigned long now = millis();
  
  if( now - lastStrobeTime >= strobeDuration && strobeState == LOW ){
    lastStrobeTime = now;
    strobeState = HIGH;
    doStrobe();
  }else if( now - lastStrobeTime >= strobeDuration && strobeState == HIGH ){
    lastStrobeTime = now;
    strobeState = LOW;
    doStrobe();
  }
  
}

void doStrobe(){
  if( lastStrobeState != strobeState ){
    digitalWrite(relayPin, strobeState);
    lastStrobeState = strobeState;
    
    if( strobeState == HIGH && currentMode == 3 ){
      delay(100);
      digitalWrite(relayPin, LOW);
      Serial.println("flash");
    }
  }
}

void tappedTempo(){
  
  unsigned long time = millis();
  
  if( ( time - tapTimePrevious ) > 3000 ){
    tapCount = 0;
  }
  
  if( tapCount == 0 ){
    tapTimeFirst = millis();
  }else{
    tapBPM = 60000 * tapCount / ( time - tapTimeFirst );
  }
  
  tapTimePrevious = time;
  tapCount++;
  
  strobeDuration = 60 / tapBPM * 1000;
  
  if( strobeDuration < 100 || strobeDuration > 10000 ){
    strobeDuration = 1000;
  }
  lastStrobeTime = 0;
}
