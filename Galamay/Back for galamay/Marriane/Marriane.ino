#include <SoftwareSerial.h> 
#include <RTClib.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

RTC_DS1307 rtc;

const int ledPinUpdateSignal =  9;

String updateValue;

int openHour = 6;
int openMinute = 0;
int openMinute1 = openMinute + 1;

int closeHour = 18;
int closeMinute = 0;
int closeMinute1 = closeMinute + 1;

void setup()
{
    Serial.begin(9600);
    lcd.begin();

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
  DateTime now = rtc.now();
  
    if (Serial.available() > 0) {
        updateValue = Serial.readStringUntil('\n'); 
        
        int index = updateValue.indexOf(':');
        String hourSet = updateValue.substring(0, index);
        int hourSetInt = hourSet.toInt();
      
        String hourRemove = removeWord(updateValue, hourSet);
        String removeColon = hourRemove.substring(1);
        int minuteIndex = removeColon.indexOf(',');
        String minuteString = removeColon.substring(0, minuteIndex);
        int minuteInt = minuteString.toInt();

        String minuteRemove = removeWord(removeColon, minuteString);
        String removeComma1 = minuteRemove.substring(1);
        int openHourIndex = removeComma1.indexOf(':');
        String openHourString = removeComma1.substring(0, openHourIndex);
        int openHourInt = openHourString.toInt();

        String openHourRemove = removeWord(removeComma1, openHourString);
        String removeColon1 = openHourRemove.substring(1);
        int openMinuteIndex = removeColon1.indexOf(',');
        String openMinuteString = removeColon1.substring(0, openMinuteIndex);
        int openMinuteInt = openMinuteString.toInt();

        String openMinuteRemove = removeWord(removeColon1, openMinuteString);
        String removeComma2 = openMinuteRemove.substring(1);
        int closeHourIndex = removeComma2.indexOf(':');
        String closeHourString = removeComma2.substring(0, closeHourIndex);
        int closeHourInt = closeHourString.toInt();

        String closeHourRemove = removeWord(removeComma2, closeHourString);
        String removeColon2 = closeHourRemove.substring(1);
        int closeMinuteInt = removeColon2.toInt();

        digitalWrite(ledPinUpdateSignal, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Updating time");
        delay(2000);
        lcd.clear();
        digitalWrite(ledPinUpdateSignal, LOW);

        if(hourSetInt != 0 && minuteInt != 0){
          rtc.adjust(DateTime(now.hour(), now.minute(), now.second(), hourSetInt, minuteInt, 2));
          DateTime now = rtc.now();
        }

        if(openHourInt != 0){
          openHour = openHourInt;
          openMinute = openMinuteInt;
          openMinute1 = openMinuteInt + 1;
        }

        if(closeHourInt != 0){
          closeHour = closeHourInt;
          closeMinute = closeMinuteInt;
          closeMinute1 = closeMinuteInt + 1;
        }  
    }
    
    int hourNow = now.hour();
    int minutesNow = now.minute();
    int secondsNow = now.second();
     
    lcd.setCursor(0,0);
    lcd.print("Time now");
    lcd.setCursor(0,1);
    lcd.print(String(hourNow) + ":" + String(minutesNow) + ":" + String(secondsNow)); 
  
    if((hourNow >= openHour && minutesNow >= openMinute) && minutesNow < openMinute1){
       Serial.println("Open%");
    }
  
    if((hourNow >= closeHour && minutesNow >= closeMinute) && minutesNow < closeMinute1){
       Serial.println("Close%");
    }
}


String removeWord(String str, String word) {
  int index = str.indexOf(word);
  if (index == -1) return str;
  int len = word.length();
  return str.substring(0, index) + str.substring(index+len);
}
