#include <Adafruit_GFX.h>    
#include <Adafruit_ST7789.h> 
#include <SPI.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "Free_Fonts.h"                                                                  

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

   #define WHITE       0xFFFF
   #define BLACK       0x0000
   #define BLUE        0x001F
   #define RED         0xF800
   #define GREEN       0x07E0
   #define CYAN        0x07FF
   #define MAGENTA     0xF81F
   #define YELLOW      0xFFE0
   #define GREY        0x2108 
   #define SCALE0      0x0000                                                                                              
   #define SCALE1      0x0000                                                               
   #define TEXT_COLOR  0xFFFF                                                                

   #define RED2RED     0
   #define GREEN2GREEN 1
   #define BLUE2BLUE   2
   #define BLUE2RED    3
   #define GREEN2RED   4
   #define RED2GREEN   5

   #define DEG2RAD 0.0174532925                                                            
   
   const char* ssid =            "XXXXXXXX";                              
   const char* password =        "XXXXXXXXXXXXX";                                
   String openWeatherMapApiKey = "XXXXXXXXXXXXXXXXXXXXXXXXX";                        
   String city =                 "Newton";
   String countryCode =          "US";                                            
   String jsonDocument (1024);                                                         

   unsigned long lastTime = 0;                                                             
   unsigned long timerDelay;                                                               
                                                        
   float temp_01=10;
   float hum_01;
   int   hum_02; 

   uint32_t runTime = -99999;                                                              
   int   reading = 10;                                                                      
   int   tesmod = 0;
   int   rGaugePos_x = 55;
   int   rGaugePos_y = 18;

   int   ringmeterRadius = 50;                                                              
   char* ringlabel[] = {"","*F","%","mBar"};                                                
   float tempRainbowgauge;  
   int   t = 40;                                                                            

   float windDir_01;

void setup() {
   Serial.begin (9600);
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);
  tft.init(135, 240);  
  tft.setRotation(3);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize (2);
   WiFi.begin (ssid, password);
   tft.setTextColor (ST77XX_RED);
   tft.print ("DASH");
   tft.setTextColor (ST77XX_YELLOW);
   tft.print ("9");
   tft.setTextColor (ST77XX_BLUE);
   tft.print ("COMPUTING");
   tft.println ();
   tft.println ();
   tft.setTextColor (ST77XX_WHITE);
   tft.println ("WIFI Connecting");
     while(WiFi.status() != WL_CONNECTED) {
     delay (500);
     tft.print (".");
     tft.print ("."); 
   }
   tft.println (); 
   tft.print ("SSID: ");
   tft.print (ssid);
   tft.println ();    
   tft.println ();
   tft.print ("IP address: "); 
   tft.println ();
   tft.setTextColor (ST77XX_GREEN);
   tft.println (WiFi.localIP());
   delay(2000);
   tft.fillScreen(ST77XX_BLACK);
   drawAllWindowFrames ();                                                               
   tft.setTextColor (YELLOW);
   tft.setFont(FF1);
   tft.setTextSize (1);
   tft.setCursor (14,20);   
   tft.print ("    Newton, MA");
   tft.setCursor (125,20);                                                                 
   tft.print (char(247));                                                                  
   tft.setTextSize (2);
   timerDelay = 1000;                                                                       
   doTheHardWork ();                                                                        
   drawAllWindowFrames ();                                                                  
   rainbowScaleMeter ();                                                                    

}

void loop() {
   timerDelay = 600000;  
   doTheHardWork ();
   rainbowScaleMeter(); 
}

void doTheHardWork (){
  if ((millis () - lastTime) > timerDelay) 
      {
      if(WiFi.status ()== WL_CONNECTED)
         {
         String serverPath = "http://api.openweathermap.org/data/2.5/weather?id=4945283&appid=" + openWeatherMapApiKey;

         jsonDocument = httpGETRequest(serverPath.c_str ());
         JSONVar myObject = JSON.parse (jsonDocument);                                      
         double temp_01 = (myObject ["main"]["temp"]); 
         double hum_01  = (myObject ["main"]["humidity"]); 
         double main_01  = (myObject ["weather"][0]["main"]);        
  
      if (JSON.typeof (myObject) == "undefined")
         {
         Serial.println ("Parsing input failed!");
         return;
         }

      int Kelvin = myObject["main"]["temp"];
      int FTemp = (((Kelvin - 273.15) * 9) / 5) + 32;

      int KelvinMIN = myObject["main"]["temp_min"];
      int FTemp_MIN = (((KelvinMIN - 273.15) * 9) / 5) + 32;

      int KelvinMAX = myObject["main"]["temp_max"];
      int FTemp_MAX = (((KelvinMAX - 273.15) * 9) / 5) + 32;

      String SFTemp = String(FTemp);
      String SFTemp_MIN = String(FTemp_MIN);
      String SFTemp_MAX = String(FTemp_MAX);

   tft.setTextSize (1); 
    tft.setCursor (0,42);
    tft.print ("Humidity");
    tft.setCursor (20,57);
    tft.print (myObject["main"]["humidity"]);
    tft.setCursor (40,57);
    tft.print ("%");
    tft.setCursor (188,42);
    tft.print ("Sky");
    tft.setCursor (175,57);
  String weatherMain = String((const char*) myObject["weather"][0]["main"]);
  weatherMain.replace("\"", ""); // remove all occurrences of the quotation mark
  tft.print(weatherMain);
/*
   tft.setCursor (15,115);          
   tft.print("Low"); 
   tft.setCursor (20,130);          
   tft.print(SFTemp_MIN); 
   tft.setCursor (190,115);
   tft.print("High"); 
   tft.setCursor (197,130);                                                                  
   tft.print(SFTemp_MAX);
*/
   tft.setCursor (10,130);
   tft.print("Low " + SFTemp_MIN);           
   tft.setCursor (160,130);
   tft.print(SFTemp_MAX + " High"); 
                                                                // 50 is top rainbow scale temperature 
         
   Serial.println ("*******************************************");  
   Serial.print ("JSON object = ");
   Serial.println (myObject);
   Serial.println ("*******************************************");  
   
   Serial.println ("extracted from JSON object:");        
   Serial.print ("Temperature:    ");    
   Serial.print (myObject["main"]["temp"]);
   Serial.println (" *Kelvin");       
   Serial.print ("Pressure:       ");
   Serial.print (myObject["main"]["pressure"]);
   Serial.println (" mB"); 
   Serial.print ("Humidity:       ");
   Serial.print(myObject["main"]["humidity"]);
   Serial.println (" %");  
   Serial.print ("Wind Speed:     ");
   Serial.print (myObject["wind"]["speed"]);
   Serial.println (" m/s");  
   Serial.print("Wind Direction: ");
   Serial.print (myObject["wind"]["deg"]);
   Serial.println (" degrees"); 

   double windDir = (myObject ["wind"]["deg"]);
   windDir_01 = windDir; 

   temp_01 = (temp_01-273) * 9/5 + 32;                    
   tempRainbowgauge = temp_01;
   hum_02 = hum_01;

   rainbowScaleMeter ();
   tempRainbowgauge = map(temp_01, 0, 100, 0, 360);   
  

    }
    else {
      Serial.println( "WiFi Disconnected");
    }
    lastTime = millis ();
  }  
}

void drawAllWindowFrames (){ 

   tft.drawRoundRect  (  0,   2, 234, 25, 4, GREEN);                                       
}

String httpGETRequest(const char* serverName) {

  HTTPClient http;
  http.begin(serverName);                                                                   
  int httpResponseCode = http.GET();                                                       
  String payload = "{}"; 
  if (httpResponseCode>0)
     {
     Serial.print ("HTTP Response code: ");
     Serial.println (httpResponseCode);
     payload = http.getString ();
     }
  else {
    Serial.print( "Error code: ");
    Serial.println (httpResponseCode);
  }
  http.end ();                                                                              
  return payload;
}

void rainbowScaleMeter() {
  
  float maxTemp = 100; // change this to the maximum temperature for your application
  float minTemp = 0;   // change this to the minimum temperature for your application
  float range = maxTemp - minTemp;
  
  // Calculate the angle that corresponds to the current temperature
  float tempAngle = (t - minTemp) / range * 240.0 - 120.0;
  if (tempAngle < -120.0) tempAngle = -120.0;
  if (tempAngle > 120.0) tempAngle = 120.0;

  // Calculate the position of the blue circle on the rainbow ring
  float tempX = cos((tempAngle + 120.0) / 180.0 * PI) * ringmeterRadius + rGaugePos_x;
  float tempY = sin((tempAngle + 120.0) / 180.0 * PI) * ringmeterRadius + rGaugePos_y;

  // Draw the blue ball -Change location realtive to rainbow circle int x =, y =
  float angle = (t-20) * DEG2RAD;
  int x = rGaugePos_x + (ringmeterRadius + 20) * cos(angle);
  int y = rGaugePos_y + (ringmeterRadius + 70) * sin(angle);
  tft.fillCircle(x, y, 5, BLUE);

     if (millis () - runTime >= 100)                                                                                            
     {                                    
      runTime = millis ();
      if( tesmod==0)
        {
         reading =  99;
        }
      if( tesmod==1)
        { 
         reading = tempRainbowgauge*2;                                                          
        } 
      int xpos = 65, ypos = 105, gap = 10; 

     ringMeter (reading,0,100, (rGaugePos_x+15),(rGaugePos_y+17),ringmeterRadius,ringlabel[0],GREEN2RED);    
     tesmod = 1;
     }   
}




int ringMeter(int value,int vmin,int vmax,int x,int y,int r, char *units, byte scheme){
   x += r; y += r;                                                                          
   int w = r / 3;                                                                           
   int angle = 150;                                                                         
   int v = map (value, vmin, vmax, -angle, angle);                                          
   byte seg = 6;                                                                            
   byte inc = 18;                                                                             
   int colour = BLACK;
 
   for (int i = -angle+inc/2; i < angle-inc/2; i += inc)                                    
     {           
      float sx = cos((i - 90) * DEG2RAD);                                                   
      float sy = sin((i - 90) * DEG2RAD);
      uint16_t x0 = sx * (r - w) + x;
      uint16_t y0 = sy * (r - w) + y;
      uint16_t x1 = sx * r + x;
      uint16_t y1 = sy * r + y;
    
      float sx2 = cos((i + seg - 90) * DEG2RAD);                                            
      float sy2 = sin((i + seg - 90) * DEG2RAD);
      int x2 = sx2 * (r - w) + x;
      int y2 = sy2 * (r - w) + y;
      int x3 = sx2 * r + x;
      int y3 = sy2 * r + y;

      if (i < v) 
         {                                                                               
          switch (scheme)
             {
              case 0: colour = RED; break;                                                  
              case 1: colour = GREEN; break;                                                
              case 2: colour = BLUE; break;                                                 
              case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break;               
              case 4: colour = rainbow(map(i, -angle, angle, 70, 127)); break;              
              case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break;              
              default: colour = BLUE; break;                                                
             }
              //tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
              tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
             }
      else                                                                                  
             {
              tft.fillTriangle(250, 250, 250, 250, 250, 250, SCALE1);                              
              //tft.fillTriangle(x1, y1, x2, y2, x3, y3, SCALE0);                             
             }
             }
   tft.setTextSize (1);
   if (tempRainbowgauge<-9.9) t = 91;
   if (tempRainbowgauge>-9.9) t = 100; 
   if (tempRainbowgauge > 0 ) t = 122;
   if (tempRainbowgauge >9.9) t = 107;
   tft.setCursor (96,90);
   tft.setTextColor (GREEN);     
   tft.print (tempRainbowgauge,1);
   tft.setCursor (140,84);   
   tft.print("o");
}

unsigned int rainbow (byte value) {                                                         
   byte red = 0;                                                                            
   byte green = 0;                                                                          
   byte blue = 0;                                                                           
   byte quadrant = value / 32;

   if (quadrant == 0)
     {
      blue = 31;
      green = 2 * (value % 32);
      red = 0;
     }
   if (quadrant == 1)
     {
      blue = 31 - (value % 32);
      green = 63;
      red = 0;
     }
   if (quadrant == 2)
     {
      blue = 0;
      green = 63;
      red = value % 32;
     }
   if (quadrant == 3)
     {
      blue = 0;
      green = 63 - 2 * (value % 32);
      red = 31;
     }
   return (red << 11) + (green << 5) + blue;
}

float sineWave(int phase) {
  
   return sin(phase * 0.0174532925);
}

int fillSegment (int x, int y, int startAngle, int subAngle, int r, unsigned int colour)
{
  float sx = cos((startAngle - 90) * DEG2RAD);                                              
  float sy = sin((startAngle - 90) * DEG2RAD);
  uint16_t x1 = sx * r + x;
  uint16_t y1 = sy * r + y;

  for (int i = startAngle; i < startAngle + subAngle; i++)                                  
     {    
     int x2 = cos((i + 1 - 90) * DEG2RAD) * r + x;                                          
     int y2 = sin((i + 1 - 90) * DEG2RAD) * r + y;
     tft.fillTriangle(x1, y1, x2, y2, x, y, colour);
     x1 = x2;                                                                               
     y1 = y2;
     }
}