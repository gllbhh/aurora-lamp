/*
ESP32 Space Weather Station code
Developed by Aleksi Myllymäki, Gleb Bulygin, and Megumi Iwata
for Mobile Fab Lab Oulu
 */


// include libraries used in the project
#include <WiFi.h>               
#include <HTTPClient.h>
#include <Adafruit_NeoPixel.h>


// all constants used in the project

#define LED_PIN 26  //pin on ESP32
#define LED_NUM 4   //number of LEDs

//WiFi credentials
const char* ssid = "WiFi_name";  //replace "WiFi_name" with your WIFi name (const char* ssid = "myWiFi";)
const char* password = "WiFi_password";  //replace "WiFi_password" with your WIFi name (password = "myPassword";)


// coordinates of the place where you want to check Aurora probability
// https://www.latlong.net/
// !We need only INTEGER part!
// Oulu
// latitude: 65
// longitude: 25

const int latitude = 65;  //latitude rounded to the nearest integer
const int longitude = 25; //longitude rounded to the nearest integer. Can NOT be negative! 0 - 359
const int ntresh = 50; //notification threshold in %. 50 means change animation if probability of Aurora is >= 50%


//declare tasks
//readAurora will be runing on core 0
//runNeopixel will be runing on core 1

TaskHandle_t readAurora;
TaskHandle_t runNeopixel;


// queue is used to pass data between tasks
QueueHandle_t queue;



// create object "strip" to run Neopixels
// visit GitHub page for additional information
// https://github.com/adafruit/Adafruit_NeoPixel
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);



/*
  Void setup is a function that you create at the top of each program. 
  Inside the curly brackets is the code that you want to run one time as soon as the program starts running. 
  You set things like pinMode in this section.
*/
void setup() {

  //open a Serial port on 115200 bps (bit-per-second).
  Serial.begin(115200); 

  
  //setting up a FIFO (First In First Out) queue of 2 elements
  queue = xQueueCreate(2, sizeof(int));
  if (queue == NULL) {
    Serial.println("Error creating the queue");
  }

  
  //create a task that will be executed in the readAuroraCode() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    readAuroraCode,   /* Task function. */
    "readAurora",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &readAurora,      /* Task handle to keep track of created task */
    0);          /* pin task to core 0 */
  delay(500);


  //create a task that will be executed in the runNeopixelCode() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    runNeopixelCode,   /* Task function. */
    "runNeopixel",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &runNeopixel,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);

  //end of void setup()
}


/* readAuroraCode
 * Once:
 * connect to WiFi using defined credentials
 * On repeat:
 * Get Aurora probability from the server
 * Sent the value to the queue
 * wait 30 sec
*/ 
void readAuroraCode( void * pvParameters ) {
  //connect to WiFi usig WiFi credentials 
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to the WiFi network");
  //repeat forever
  
  for (;;) {
    Serial.print("readAurora running on core ");
    Serial.println(xPortGetCoreID());
    // get aurora probability from https://services.swpc.noaa.gov/json/ovation_aurora_latest.json
    int auroraState = getAurora(longitude, latitude); 

    //put the value to the end of the queue
    xQueueSend(queue, &auroraState, portMAX_DELAY);

    // send longitude, latitude and aurora probability to serial port
    // used mostly for debugging reasons
    Serial.print("Longitude: ");
    Serial.print(longitude);
    Serial.print(", ");
    Serial.print("Latitude: ");
    Serial.print(latitude);
    Serial.print(", Aurora: ");
    Serial.println(auroraState);
    // send "readAurora running on core 0" to the serial port
    // used mostly for debugging reasons
    
    delay(30000); //wait 30 seconds
  }
  //end of readAuroraCode
}


/*runNeopixelCode
 * get Aurora probability from the queue
 * if it is > notification threshold
 * run animation 1
 * else
 * run animation 2
 * 
 * If ESP has failed to connect to the internet, 
 * probability will always be 0%.
 * In that case animation 1 will run on repeat
 */
void runNeopixelCode( void * pvParameters ) {
  //repeat forever
  for (;;) {
    // send "runNeopixel running on core 1" to the serial port
    // used mostly for debugging reasons
    Serial.print("runNeopixel running on core ");
    Serial.println(xPortGetCoreID());

    static int auroraState;

    //read probability of Aurora to auroraState variable
    xQueueReceive(queue, &auroraState, 0);
    
    Serial.print("Aurora probability: ");
    Serial.print(auroraState);
    Serial.println("%");   

    //if Aurora probability < Notification treshold
    //run animation 1
    if (auroraState < ntresh){
        rainbowCycle(20);
        Serial.println("Animation 1");
    }
    //else run animation 2
    else {
         breathe(10);
         Serial.println("Animation 2");
    }
    
  }
  //end of runNeopixelCode
}



void loop() {
  /*
   * Usually, the main code is located here. 
   * In our case, we only create tasks and the "void loop()" is empty.
   * But you can't delete it. Without it, Arduino will fail to compile the sketch.
   */
}


//getAurora gets Aurora probabilyty in %
// https://forum.arduino.cc/t/parse-http-data-from-website/576407/3
int getAurora(int longitude, int latitude) {
  WiFiClient *pStream;
  HTTPClient http;
  http.begin("https://services.swpc.noaa.gov/json/ovation_aurora_latest.json");
  
  if (http.GET() == HTTP_CODE_OK) {
    pStream = http.getStreamPtr();
    char s[16];
    
    snprintf(s, sizeof(s), "[%i, %i, ", longitude, latitude);
    
    pStream->find(s);
    char aur = pStream->read();
    char aur2 = pStream->read();
    int aurora;
    
    if (aur2 == ']'){
     aurora = aur - '0';
    }
    else{
     aurora = 10 * (aur - '0') + aur2 - '0';
    }
    
    return aurora;
  }
    else {
    Serial.println("Error while getting the data");
    int aurora = 0;
    return aurora;
  }
}


//******* LED ANIMATIONS *******

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void breathe(int wait) {
    for (int j = -45; j < 225; j++){                   // 360 degrees of an imaginary circle.
    float angle = radians(j);                          // Converts degrees to radians.
    int col = (255 / 2) + (255 / 2) * sin(angle);      // Generates points on a sign wave.
    for (int i=0; i < LED_NUM; i++) {
        strip.setPixelColor(i, 0, col, 0);
      }
    strip.show();
    delay(wait);                                       // Delay between each point of sine wave.
  }
}
