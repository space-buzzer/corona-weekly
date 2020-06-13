// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const char *daysOfTheWeek[7] = {
  "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};


RTC_DS3231 rtc;

void setup () {
  Serial.begin(9600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // If the RTC have lost power it will sets the RTC to the date & time this sketch was compiled in the following line
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }  

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  watchface_begin();

  Serial.println("-- *** --");
}

void watchface_begin() {
  // clear display and draw frame and tics
  display.clearDisplay();

  int spacing = 2;
  int rounding = 3;
  // drawRect params = x, y, w, h, color

  // fill the screen
  display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);

  // make holes for time numbers
  int y = spacing;
  
  int time_w = (SCREEN_WIDTH - 4*spacing) / 3;
  for (int i = 0; i < 3; i++) {
    display.fillRoundRect(
      spacing * (i+1) + time_w * i, y,
     (int)time_w, (SCREEN_HEIGHT - spacing*3) / 2,
     rounding, SSD1306_BLACK);
  }

  y += (SCREEN_HEIGHT - spacing*3) / 2;
  y += spacing;

  // day of week
  int date_h = (SCREEN_HEIGHT - 3*spacing) / 2;
  date_h = (date_h - spacing)/2;
  display.fillRoundRect(spacing, y,
    SCREEN_WIDTH - 2*spacing, date_h, rounding, SSD1306_BLACK);

  y += (date_h + spacing);
  int x = spacing;
  int date_w = (SCREEN_WIDTH - 3*spacing)/2;
  date_w = (date_w - spacing)/2;
  display.fillRoundRect(x, y, date_w , date_h, rounding, SSD1306_BLACK);
  x += (date_w + spacing);
  display.fillRoundRect(x, y, date_w , date_h, rounding, SSD1306_BLACK);
  x += (date_w + spacing);
  display.fillRoundRect(x, y, date_w*2 + spacing*2 , date_h, rounding, SSD1306_BLACK);

  display.setTextColor(WHITE, BLACK);
  
  display.display();  
}

void watchface_show_time(const DateTime& t) {
  // time
  draw_num(5, 6, t.hour(), 3);
  draw_num(48, 6, t.minute(), 3);
  draw_num(90, 6, t.second(), 3);

  int dow = t.dayOfTheWeek();

  // day of week
  // clear the previous text
  display.setTextSize(1);
  display.setCursor(24 ,35);
  display.print("        ");

  int16_t dow_x, dow_y;
  uint16_t dow_h, dow_w = 44;
  display.getTextBounds(daysOfTheWeek[dow], 0, 35, &dow_x, &dow_y, &dow_w, &dow_h);
  display.setCursor((SCREEN_WIDTH - (int)dow_w)/2, 35);
  display.print(daysOfTheWeek[dow]);


  // date
  draw_num(11, 51, t.day(), 1);
  draw_num(42, 51, t.month(), 1);
  draw_num(83, 51, 20, 1);
  draw_num(96, 51, t.year()-2000, 1);

  // add lines
//  display.drawFastHLine(0, SCREEN_HEIGHT/4, SCREEN_WIDTH, SSD1306_WHITE);
//  display.drawFastHLine(0, (SCREEN_HEIGHT/4) * 2.5 - 1, SCREEN_WIDTH, SSD1306_WHITE);
//  display.drawFastHLine(0, (SCREEN_HEIGHT/4) * 3.5 - 2, SCREEN_WIDTH, SSD1306_WHITE);
  
  display.display();
}

void draw_num(byte x, byte y, byte num, byte text_size) {
  char text[3];
  sprintf(text, "%02u", num);
  display.setCursor(x, y);
  display.setTextSize(text_size);
  display.print(text);
}


void loop () {
    DateTime foobar = rtc.now();
    watchface_show_time(foobar);

//    Serial.print("Temperature: ");
//    Serial.print(rtc.getTemperature());
//    Serial.println(" C");
//
//    Serial.println();
    delay(100);
}
