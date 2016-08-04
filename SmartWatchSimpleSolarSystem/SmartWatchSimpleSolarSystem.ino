/*************************************************** 
  This is a example sketch demonstrating graphic drawing
  capabilities of the SSD1351 library for the 1.5" 
  and 1.27" 16-bit Color OLEDs with SSD1351 driver chip

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/1431
  ------> http://www.adafruit.com/products/1673
 
  If you're using a 1.27" OLED, change SSD1351HEIGHT in Adafruit_SSD1351.h
   to 96 instead of 128

  These displays use SPI to communicate, 4 or 5 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution

  The Adafruit GFX Graphics core library is also required
  https://github.com/adafruit/Adafruit-GFX-Library
  Be sure to install it!
 ****************************************************/

// You can use any (4 or) 5 pins 
#define sclk 3
#define mosi 4
#define dc   5
#define cs   6
#define rst  7

// Color definitions
#define BLACK           0x0000      /*   0,   0,   0 */
#define NAVY            0x000F      /*   0,   0, 128 */
#define DARKGREEN       0x03E0      /*   0, 128,   0 */
#define DARKCYAN        0x03EF      /*   0, 128, 128 */
#define MAROON          0x7800      /* 128,   0,   0 */
#define PURPLE          0x780F      /* 128,   0, 128 */
#define OLIVE           0x7BE0      /* 128, 128,   0 */
#define LIGHTGREY       0xC618      /* 192, 192, 192 */
#define DARKGREY        0x7BEF      /* 128, 128, 128 */
#define BLUE            0x001F      /*   0,   0, 255 */
#define GREEN          0x07E0      /*   0, 255,   0 */
#define CYAN            0x07FF      /*   0, 255, 255 */
#define RED             0xF800      /* 255,   0,   0 */
#define MAGENTA         0xF81F      /* 255,   0, 255 */
#define YELLOW          0xFFE0      /* 255, 255,   0 */
#define WHITE           0xFFFF      /* 255, 255, 255 */
#define ORANGE          0xFD20      /* 255, 165,   0 */
#define GREENYELLOw     0xAFE5      /* 173, 255,  47 */
#define PINK            0xF81F
#define TIME            0xF800
#define ORBIT           0x7800



#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <Timer.h>



int16_t hour = 0;
int16_t minute = 0;
int16_t second = 0;

//Navigation values 
bool isNotification = true;
bool isMenu = false; 
bool isHome = true;
bool isMessages = false; 
bool isTimer = false; 

int upPin = 8; 
int downPin = 9; 
int selectPin= 10; 

int down = 0;
int up = 0;
int select = 0;


int menuOption = 0;
int menuOptionCount = 4; 
int menuOptionsX[] = {20,20,20,20,100};
int menuOptionsY[] = {50,40,30,20,0};
String menuOptions [] = {"OPTION 4","TIMER","MESSAGES","HOME","MENU"};

int timerOption = 0; 
int timerOptionCount = 4;
int goalHour = 0;
int goalMinute = 0;
int goalSecond = 0; 
int timerHour = 0;
int timerMinute = 0; 
int timerSecond = 0; 
bool timerHit = false;
bool timerSet = false; 
String timerPosition [] = {"start","stop"}; 


// Option 1: use any pins but a little slower
Adafruit_SSD1351 tft = Adafruit_SSD1351(cs, dc, mosi, sclk, rst); 
Timer t; 

void setup() {
 tft.begin();
 Serial.begin(9600);
 drawHome();


  pinMode(downPin, INPUT_PULLUP);   
  pinMode(upPin, INPUT_PULLUP);
  pinMode(selectPin, INPUT_PULLUP);   
  t.every(1000, updateClock);
//  t.every(500,getTextMessage);
  updateClock();
    getTime();

//   getTextMessage();

}

void loop() {

  down = digitalRead(downPin);  // read input value
  up = digitalRead(upPin);  // read input value
  select = digitalRead(selectPin);  // read input value
  t.update();
  
  if (down == LOW) {   
    if(isMenu){
      menuDown();
    }

    if(isTimer){
      timerDown(); 
    }
  }

  if(up == LOW){
    if(isMenu){
      menuUp(); 
    }
    if(isTimer){
      timerUp(); 
    }
    
  }

  if(select == LOW){    
    if(!isMenu && !isTimer){
      drawMenuOptions();
    }
    else if(isMenu){
      menuSelect();
    }
    else if(isTimer){
      timerSelect(); 
    }
  }

  

}


void drawTimer(){
  
   toggleNav(false, false, false, true);
   
   tft.fillRect(0,0,128,128,BLACK);

   tft.setCursor(15,32);
   tft.setTextColor(BLACK,WHITE); 
   tft.setTextSize(2);
   
   tft.print("00");
   tft.setTextColor(WHITE,BLACK);  
   tft.print(":00:00");

   tft.setCursor(32,58);
   tft.setTextColor(GREEN,BLACK);
   tft.setTextSize(2);
   tft.print("Start");

    tft.setCursor(40,80);
   tft.setTextColor(RED,BLACK);
   tft.setTextSize(2);
   tft.print("Stop");
   
   tft.setTextColor(WHITE,BLACK); 
   tft.setTextSize(1); 
   tft.setCursor(100,0);
   tft.print("MENU"); 
}


void printTimerHour(){
     tft.setTextSize(2); 
     tft.setCursor(15,32); 

      if(goalHour < 10){
         tft.print(0);
         tft.print(goalHour);
      }
      else{
        tft.print(goalHour);    
      }       
}

void printTimerMinute (){
     tft.setTextSize(2);
     tft.setCursor(49,32); 

      if(goalMinute < 10){
         tft.print(0);
         tft.print(goalMinute);
      }
      else{
        tft.print(goalMinute);    
      }        
}

void printTimerSecond (){
     tft.setTextSize(2);
     tft.setCursor(89,32); 

      if(goalSecond < 10){
         tft.print(0);
         tft.print(goalSecond);
      }
      else{
        tft.print(goalSecond);    
      }        
}

void drawTimerStart(){
 
     tft.setCursor(32,58);
     tft.setTextSize(2);
     tft.print("Start");
  
}

void drawTimerStop(){
  
   tft.setCursor(40,80);
   tft.setTextSize(2);
   tft.print("Stop");
}

void drawMenuOption(){

   tft.setTextSize(1); 
   tft.setCursor(100,0);
   tft.print("MENU"); 
}


void  timerDown(){
  delay(100);
  
  if(timerOption == 0){  
    if(goalHour != 0){
      goalHour = goalHour - 1; 
    }
    else{
      goalHour = 99; 
    }
     tft.setTextColor(BLACK,WHITE);
     printTimerHour();
  }
  else if(timerOption == 1){
      if(goalMinute != 0){
        goalMinute = goalMinute - 1; 
      }
      else
      {
        goalMinute = 60;
      }
       tft.setTextColor(BLACK,WHITE);
       printTimerMinute();  
  }
  else if(timerOption == 2){
      if(goalSecond != 0){
      goalSecond = goalSecond - 1; 
      }
      else{
        goalSecond = 60;
      }
       tft.setTextColor(BLACK,WHITE);
       printTimerSecond();  
  }
  else if(timerOption == 3){  
    tft.setTextColor(GREEN,BLACK);
    drawTimerStart();

    tft.setTextColor(BLACK, RED);
    drawTimerStop();

    timerOption = 4;
   }
  else if(timerOption == 4){

   tft.setTextColor(RED,BLACK);
   drawTimerStop();

   tft.setTextColor(BLACK,WHITE); 
   drawMenuOption();

   timerOption = 5;
   }
   else{
     tft.setTextColor(WHITE,BLACK); 
     drawMenuOption();

     tft.setTextColor(BLACK,WHITE);
     printTimerHour();

     timerOption = 0;
   }
  
 
}

void timerUp(){
  delay(100);
    
  if(timerOption == 0){  
    if(goalHour < 99){
      goalHour = goalHour + 1; 
    }
    else{
      goalHour = 0; 
    }
     tft.setTextColor(BLACK,WHITE);
     printTimerHour();
  }
  else if(timerOption == 1){
      if(goalMinute < 60){
        goalMinute = goalMinute + 1; 
      }
      else{
        goalMinute = 0; 
      }
       tft.setTextColor(BLACK,WHITE);
       printTimerMinute();
  }
  else if(timerOption == 2){
      if(goalSecond <  60){
        goalSecond = goalSecond + 1; 
      }
      else{
        goalSecond = 0;
      }
       tft.setTextColor(BLACK,WHITE);
       printTimerSecond(); 
  }
}

void timerSelect(){
  if(timerOption == 0){
    
     //select minutes
     tft.setTextColor(WHITE,BLACK);
     printTimerHour();
     
     tft.setTextColor(BLACK,WHITE);
     printTimerMinute(); 
     timerOption = 1; 
  }
  else if(timerOption == 1){
    // select seconds

     tft.setTextColor(WHITE,BLACK);
     printTimerMinute(); 

     tft.setTextColor(BLACK,WHITE);
     printTimerSecond(); 
     timerOption = 2;
  }
  else if(timerOption == 2){
//   select start
     tft.setTextColor(WHITE,BLACK);
     printTimerSecond(); 

     tft.setTextColor(BLACK,GREEN);
     drawTimerStart();    
     timerOption = 3;
  }
  else if(timerOption == 3){
    //start timer
   tft.setTextColor(GREEN,BLACK);
   drawTimerStart();

   // select stop button
   tft.setTextColor(BLACK,RED);
   drawTimerStop();
   
   timerOption = 4;
  }
  else if(timerOption == 4){
    //stop timer
    tft.setTextColor(RED,BLACK);
    drawTimerStop();

   tft.setTextColor(BLACK,WHITE); 
   drawMenuOption();
   
    timerOption = 5; 
  }
  else{
    //draw menu
    drawMenuOptions();
    timerOption = 0; 
  }
}

void menuUp(){
   tft.setTextSize(1); 

     
  if(menuOption < menuOptionCount){
    tft.setTextSize(1); 

   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.setTextColor(WHITE,BLACK); 
   tft.print(menuOptions[menuOption]); 

    menuOption +=1;
    
   tft.setTextColor(BLACK,WHITE); 
   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.print(menuOptions[menuOption]); 

   
  }
  else
  {
     
   tft.setCursor(menuOptionsX[menuOptionCount],menuOptionsY[menuOptionCount]);
   tft.setTextColor(WHITE,BLACK); 
   tft.print(menuOptions[menuOption]); 

   menuOption =0;
  
   tft.setTextColor(BLACK,WHITE); 
   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.print(menuOptions[menuOption]); 
  }

     Serial.println(menuOption);
}

void menuDown(){
    tft.setTextSize(1); 
   
  if(menuOption >=0){
    tft.setTextSize(1); 

   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.setTextColor(WHITE,BLACK); 
   tft.print(menuOptions[menuOption]); 

   menuOption -=1;
   
   tft.setTextColor(BLACK,WHITE); 
   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.print(menuOptions[menuOption]); 
  }
  else
  {
     
   tft.setCursor(menuOptionsX[menuOptionCount],menuOptionsY[menuOptionCount]);
   tft.setTextColor(WHITE,BLACK); 
   tft.print(menuOptions[menuOption]); 

   menuOption =4;
  
   tft.setTextColor(BLACK,WHITE); 
   tft.setCursor(menuOptionsX[menuOption],menuOptionsY[menuOption]);
   tft.print(menuOptions[menuOption]); 
  }
}

void menuSelect(){
  Serial.println(menuOption);
  switch(menuOption){
    case 0:
//      drawOption4();
      break;  
    case 1:
      drawTimer();
      break;  
    case 2:
      drawMessages();
      break;  
    case 3:
      drawHome();
      break;
    default:
      drawMenuOptions();
      break;
  }
}

void toggleNav(bool homeScreen, bool menuScreen, bool messagesScreen, bool timerScreen){

  //add getTime here to update clock
  
  isHome = homeScreen; 
  isMenu = menuScreen; 
  isMessages = messagesScreen; 
  isTimer = timerScreen; 

  menuOption = 0;

   tft.setTextColor(BLACK,WHITE); 
   tft.setTextSize(1); 
   tft.setCursor(100,0);
   tft.print("MENU"); 
}

void drawHome(){

     tft.fillRect(0,0,128,128,BLACK);

     toggleNav(true, false, false, false);


    tft.drawCircle(64,64,14,ORBIT);
    tft.drawCircle(64,64,19,ORBIT);
    tft.drawCircle(64,64,25,ORBIT);
    tft.drawCircle(64,64,32,ORBIT);
    tft.drawCircle(64,64,38,ORBIT);
    tft.drawCircle(64,64,45,ORBIT);
    tft.drawCircle(64,64,51,ORBIT);
    tft.drawCircle(64,64,57,ORBIT);

    //sun
    tft.fillCircle(64,64,11,YELLOW);

    //mercury
    tft.fillCircle(64,78,2,OLIVE);
    //venus
    tft.fillCircle(64,64-19,4,ORANGE);
    //earth
    tft.fillCircle(64+15,64-20,4,BLUE);
    //mars
    tft.fillCircle(64+8,64+31,3,RED);
    //jupiter
    tft.fillCircle(64-34,64+16,6,LIGHTGREY);
    //saturn
    tft.fillCircle(64+42,64+16,7,GREENYELLOw);
    //uranus
    tft.fillCircle(64+50,64-10,6,NAVY);
    //neptune
    tft.fillCircle(63-35,63-45,6,DARKCYAN);

   tft.setTextColor(BLACK,WHITE); 
   tft.setTextSize(1); 
   tft.setCursor(100,0);
   tft.print("MENU");   
}


void drawMenuOptions(){

    
   tft.fillRect(0,0,128,128,BLACK);
   
    toggleNav(false, true, false, false);
    menuOption = 4;



   tft.setTextColor(WHITE,BLACK);
   tft.setCursor(20,20);
   tft.print("HOME"); 

//   tft.setTextColor(WHITE,BLACK);
   tft.setCursor(20,30);
   tft.print("MESSAGES"); 
   
   tft.setCursor(20,40);
   tft.print("TIMER");

   tft.setCursor(20,50);
   tft.print("OPTION 4");     
}


void drawMessages(){

    tft.fillRect(0,0,128,128,GREEN);

    toggleNav(false, false, true, false);


   tft.setTextColor(WHITE,BLACK); 
   tft.setTextSize(1); 
   tft.setCursor(100,0);
   tft.print("MENU"); 
}


void updateClock(){
   second+=1; 
 
  tft.setTextColor(WHITE,BLACK);  
  
  if(second ==60){
    second=0;
    minute+=1;  
  }

 if(minute ==60){
    minute = 0;
    hour+=1;
  }

  if(hour >24){
    hour = 1;   
  }

 tft.setCursor(15,110);
 tft.setTextSize(2);
 
 if(hour < 10){
  tft.print("0"); 
 }
 tft.print(hour);
 tft.print(":");

 if(minute < 10){
  tft.print("0"); 
 }
 tft.print(minute);
 tft.print(":");

 if(second < 10){
  tft.print("0"); 
 }
 tft.print(second); 

if(isNotification){

    int isBlack = second % 2;
    
    if(isBlack == 0){
      tft.setTextColor(BLACK,BLACK); 
    }
    else
    {
      tft.setTextColor(WHITE,BLACK);
    }
    
    tft.setCursor(0,0);
    tft.setTextSize(1);
    tft.print("MESSAGE");
  }
}



void getTime(){
  while(Serial.available() > 0){

    String input = Serial.readStringUntil('\n');
    Serial.read();
    
    int barIndexOne = input.indexOf('|');
    int barIndexTwo = input.indexOf('|',barIndexOne+1); 
  
    String currentTime = input.substring(barIndexOne+1,barIndexTwo);    

    int hourIndex = currentTime.indexOf(':'); 
    int minuteIndex = currentTime.indexOf(hourIndex+1,':'); 
    int secondIndex = currentTime.indexOf(minuteIndex+1,':'); 

    
    hour = currentTime.substring(0,hourIndex).toInt();
    minute = currentTime.substring(hourIndex+1,minuteIndex).toInt();
    second = currentTime.substring(minuteIndex+1, secondIndex).toInt(); 

  }
}

void getTextMessage(){
 while(Serial.available() > 0){

    String input = Serial.readStringUntil('\n');
    Serial.read();
    
    int barIndexOne = input.indexOf('|');
    int barIndexTwo = input.indexOf('|',barIndexOne+1); 
    int barIndexThree = input.indexOf('|',barIndexTwo+1); 
  
    String textMessage = input.substring(barIndexThree+1, input.length());    
    
    tft.setTextColor(WHITE,BLACK); 
    tft.setCursor(64,64);

    tft.println(input); 

  }
  
}



