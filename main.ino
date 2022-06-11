#include <LiquidCrystal.h>
#include <Keypad.h>
LiquidCrystal lcd(0,1,2,3,4,5);
int tempPin = A5;   // the output pin of LM35
int fan = 10;       // the pin where fan is
int buzzer = 13;    // buzzer pin
int tempMin = 25;   // the temperature to start the fan
int tempMax = 50;   // the maximum temperature when fan is at 100%
int pir = 11;		// pir sensor pin
int flagPersonInside = 0;
int fanSpeed;
int fanLCD;
int temp;
int sensor = 0;

const byte rows = 4;
const byte cols = 4;
#define passLength 5
char pass[passLength] = "3214";
char enteredPass[passLength];
int passReadCount = 0;
char customKey;
int flagCorrectPass = 0;
#define wrongPassLimit 3
int remPassCount;
int wrongPassCount = 0;
int time = -1;

char hexaKeys[rows][cols] = 
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

byte rowPins[rows] = {A0,A1,A2,A3};
byte colPins[cols] = {A4,8,7,6};   

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, rows, cols);

void setup()
{
	
  	pinMode(fan, OUTPUT);
  	pinMode(pir, INPUT);
	pinMode(buzzer, OUTPUT);
	pinMode(tempPin, INPUT);
	lcd.begin(16,2);  
	lcd.print("Welcome Ma'am!");
	lcd.setCursor(0,1);
	lcd.print("Initializing...");
	delay(2000);
	lcd.clear();
}
 
void loop() 
{  
	
  	updateRoomStatus();
  
	if (flagCorrectPass == 1)
    {
        temp = readTemp() - 50;     
      	if((temp >= tempMin) && (temp <= tempMax)) 
        {  
            fanSpeed = map(temp, tempMin, tempMax, 32, 255); // the actual speed of fan
            fanLCD = map(temp, tempMin, tempMax, 0, 100);  // speed of fan to display on LCD
	        analogWrite(fan, fanSpeed);  // spin the fan at the fanSpeed speed

          	lcd.clear();
            lcd.print("Temperature:");
            lcd.print(temp);
            lcd.print("C");
            lcd.setCursor(0,1);
            lcd.print("Fan Speed:");
            lcd.print(fanLCD); // display the fan speed
            lcd.print("%");
            delay(1000);
        } 

        else if(temp < tempMin)
        {
            fanSpeed = 0; 
            fanLCD = 0; 
            digitalWrite(fan, LOW);
            lcd.clear();
            lcd.print("Temp is normal");
            lcd.setCursor(0,1);
            lcd.print("Fan not required");
            delay(1000);
        } 

        else if(temp > tempMax) 
        {
            fanLCD = 0;
            fanSpeed = 0; 
            digitalWrite(fan, LOW);
            lcd.clear();
            lcd.print("Temp is high");
            lcd.setCursor(0,1);
            lcd.print("Use AC...");
        } 
    }
  	
    else if (flagPersonInside == 1)
    {
        if (time == -1)
          	time = millis()/1000;
      	if ((millis()/1000 == time+60) || (wrongPassCount == wrongPassLimit))
        {
          	lcd.clear();
          	lcd.print("BREACH!");
          	digitalWrite(buzzer, HIGH);
          	delay(5000);
            digitalWrite(buzzer, LOW); 
          	lcd.clear();
          	exit(0);
        }
      
      	lcd.setCursor(0,0);
      	lcd.print("Enter Password:");

        customKey = customKeypad.getKey();
        if (customKey)
        {
            enteredPass[passReadCount] = customKey; 
            lcd.setCursor(passReadCount,1); 
            lcd.print(enteredPass[passReadCount]); 
            passReadCount++; 
        }
      
      	if(passReadCount == passLength-1)
        {
            delay(1000);
          	lcd.clear();

            if(!strcmp(enteredPass, pass))
            {
              	lcd.print("Correct");
              	delay(1000);
              	flagCorrectPass = 1;
            }
            else
            {
              	lcd.print("Incorrect");
            	wrongPassCount++;
              	lcd.setCursor(0,1);
              	remPassCount = wrongPassLimit - wrongPassCount;
            	lcd.print(remPassCount);
              	lcd.print(" Attemps Left");
              	delay(1000);
            }
            passReadCount = 0;
            delay(1000);
          	lcd.clear();
  		}
    }
  	
  	else
    {
      	lcd.clear();
        lcd.print("Room Empty...");
      	delay(1000);
    }
  
  	delay(500);
}

int readTemp()
{  
	temp = analogRead(tempPin);
	return temp * 0.48828125;
}
          
void updateRoomStatus() 
{
    if (digitalRead(pir)==HIGH) 
    {
        if (flagPersonInside == 0)
        {
          	flagPersonInside = 1;
          	//flagCorrectPass = 0;
        }
      else if (flagPersonInside==1){
      		flagPersonInside = 0;
      }
       
      	flagCorrectPass = 0;
      	wrongPassCount = 0;
      	time = -1;
      	lcd.clear();
    }
}