//#define ESPink42_V2     //for version v2.4 and earlier
#define ESPink42_V3     //for version v3.0 and above

#ifdef ESPink42_V2
  // SD
  #define MOSI  23
  #define MISO  12
  #define SCK   18
  #define SD_CS 27
  //Display
  //MOSI/SDI    23
  //CLK/SCK     18
  //SS/CS       5
  #define DC    17 
  #define RST   16  
  #define BUSY  4 
  #define POWER 2
  #define SDA   21
  #define SCL   22
  #define BAT   34
  #define DISPLAY_LED 26      // Display backlight pin
#elif defined(ESPink42_V3)
  #define MOSI  11            // same for Display and SD
  #define MISO  13            // same for Display and SD
  #define SCK   12            // same for Display and SD
  #define SD_CS 17
  //D_CS        10
  #define DC    48 
  #define RST   45  
  #define BUSY  38 
  #define POWER 47
  #define SDA   42
  #define SCL   2
  #define BAT   9
  #define DISPLAY_LED 1      // Display backlight pin
#endif