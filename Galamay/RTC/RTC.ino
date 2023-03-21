 #include <SoftwareSerial.h> 
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

SoftwareSerial espSerial(6,7);
SoftwareSerial arduinoSerial(4,5);

RTC_DS1307 rtc;

const int buttonUpdate = 10;
int buttonStateUpdate = 0;

const int ledPinUpdateSignal =  9;

String updateValue;

int loopOnce = 0;

int oldOpenValue = 100, oldCloseValue = 100;

void setup()
{
    Serial.begin(9600);
    espSerial.begin(115200);
    arduinoSerial.begin(9600);
    
    lcd.begin();

    pinMode(buttonUpdate, INPUT);

    pinMode(ledPinUpdateSignal, OUTPUT);
    
    lcd.setBacklight((uint8_t)1);
    
    if (! rtc.begin()) {
      Serial.flush();
      while (1);
    }
    
    DateTime now = rtc.now();
    rtc.adjust(DateTime(now.hour(), now.minute(), now.second(), 0, 0, 0));       
}
    
void loop()
{  

  if(loopOnce == 0){
      espSerial.print("1%");
  }

  loopOnce++;

  
  buttonStateUpdate = digitalRead(buttonUpdate); // for manual

  Serial.println(buttonStateUpdate);


  if(buttonStateUpdate == HIGH){
      digitalWrite(ledPinUpdateSignal, HIGH);
      espSerial.print("1%");
      delay(2000);
      digitalWrite(ledPinUpdateSignal, LOW);
  }

  DateTime now = rtc.now();
  
    if (Serial.available() > 0) {
        updateValue = Serial.readStringUntil('%'); 
        
        int indexHour = updateValue.indexOf(':');
        String hourValue = updateValue.substring(0, indexHour);
        int hourValueInt = hourValue.toInt();
      
        String hourRemove = removeWord(updateValue, hourValue);
        String minuteValue = hourRemove.substring(1);
        int minuteValueInt = minuteValue.toInt();
        
        digitalWrite(ledPinUpdateSignal, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Updating time");
        delay(2000);
        lcd.clear();
        digitalWrite(ledPinUpdateSignal, LOW);

        if(hourValueInt != 0 && minuteValueInt != 0){
            rtc.adjust(DateTime(now.hour(), now.minute(), now.second(), hourValueInt, minuteValueInt, 0));
            DateTime now = rtc.now();
        }
    }
    
    int hourNow = now.hour();
    int minutesNow = now.minute();
    int secondsNow = now.second();
     
    lcd.setCursor(0,0);
    lcd.print("Time now");
    lcd.setCursor(0,1);
    String secondsString = String(secondsNow);
    int secondsNowLength = secondsString.length();
    if(secondsNowLength <= 1){
        lcd.print(String(hourNow) + ":" + String(minutesNow) + ":" + "0" + String(secondsNow)); 
    }

    else{
        lcd.print(String(hourNow) + ":" + String(minutesNow) + ":" + String(secondsNow)); 
    }

    //open
    if(minutesNow <= 30){
          oldCloseValue = 100;
          
          int openLoopCounter = 5;
          for(;;){
              if(openLoopCounter >= 30){
                  break;
              }
              
              if(minutesNow == openLoopCounter){
                  if(oldOpenValue == openLoopCounter){
                      Serial.println("Already Send Open Value");
                      break;
                  }
      
                  else{
                      oldOpenValue = openLoopCounter;
                      
                      Serial.println("Opening");
                      arduinoSerial.print("2%");
                      break;
                  }
                  
                  
              }
      
              else{
                  openLoopCounter = openLoopCounter + 5;
              }
          }
    }

    else if(minutesNow > 30){
        oldOpenValue = 100;
        
        //close
        int openLoopCounter1 = 30;
        for(;;){
            if(openLoopCounter1 >= 60){
                break;
            }
            
            if(minutesNow == openLoopCounter1){
                if(oldCloseValue == openLoopCounter1){
                      Serial.println("Already Send Close Value");
                      break;
                  }
      
                  else{
                      oldCloseValue = openLoopCounter1;
                      
                      Serial.println("Closing");
                      arduinoSerial.print("3%");
                      break;
                  }
            }
    
            else{
                openLoopCounter1 = openLoopCounter1 + 5;
            }
        }
    }
    


    
    

//    int hourIncrement = 6;
//    
//    for(int loopTime = 0; loopTime < 26; loopTime++){
//
//        if(loopTime >= 0 && loopTime <= 12){ 
//            if(hourNow >= hourIncrement && minutesNow <= 0){
//                Serial.println("Open%");
//                break;
//            }
//
//            else{
//                hourIncrement++;
//            }
//        }
//
//        if(loopTime >= 13 && loopTime <= 26){
//            if(loopTime >= 19){
//                hourNow = 0;
//
//                if(hourNow >= hourIncrement && minutesNow <= 0){
//                    Serial.println("Close%");
//                    break;
//                }
//    
//                else{
//                    hourIncrement++;
//                }
//            }
//
//            else{
//                if(hourNow >= hourIncrement && minutesNow <= 0){
//                    Serial.println("Close%");
//                    break;
//                }
//    
//                else{
//                    hourIncrement++;
//                }
//            } 
//        }
//    }
}


String removeWord(String str, String word) {
  int index = str.indexOf(word);
  if (index == -1) return str;
  int len = word.length();
  return str.substring(0, index) + str.substring(index+len);
}
