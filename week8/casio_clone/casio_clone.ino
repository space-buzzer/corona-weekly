#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>


// *** Configuration ***
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)

#define button1 3
#define button2 4
#define buzzer 3
// *** ***


#define US_FORMAT 0

#if US_FORMAT
static const char TIME_FMT[] = "hh:mm:ss";
// x locations for day, month, year
static const int DATE_LOC[] = {98, 80, 116, 80};
#endif
#if !US_FORMAT
// x locations for day, month, year
static const int DATE_LOC[] = {80, 98, 116, 80};
static const char TIME_FMT[] = "hh:mm:ss";
#endif


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RTC_DS3231 rtc;

char temperature[] = " 00.00";

void setup() {
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  //pinMode(buzzer, OUTPUT);

//  digitalWrite(buzzer, HIGH);
//  delay(1000);
  
  Serial.begin(115200);
  Serial.println("--- ** ---");

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // no need to set the RTC, since we can do it from the Arduino

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 display allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // show the adafruit logo
  display.display();
  delay(200);

  watchface_begin();
}

void display_day(int dow) {
  switch(dow) {
    case 0: draw_text(0, 0, "Sunday   ", 1); break;
    case 1: draw_text(0, 0, "Monday   ", 1); break;
    case 2: draw_text(0, 0, "Tuesday  ", 1); break;
    case 3: draw_text(0, 0, "Wednesday", 1); break;
    case 4: draw_text(0, 0, "Thursday ", 1); break;
    case 5: draw_text(0, 0, "Friday   ", 1); break;
    default: draw_text(0, 0,"Saturday ", 1);
  }
}

void watchface_begin() {
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  draw_text(DATE_LOC[3], 0, "  /  /", 1);
  draw_text(0, 55, "Temperature", 1);
  display.drawRect(117, 55, 3, 3, WHITE);
  draw_text(122, 55, "C", 1);  

  display.display();
}

void watchface_display(const DateTime& t, float tempc) {
  if (tempc < 0) {
    tempc = abs(tempc);
    temperature[0] = '-';
  } else {
    temperature[0] = ' ';
  }

  int temp1 = int(floor(tempc));
  int temp2 = tempc * 100  - temp1 * 100;
  snprintf(temperature+1, 6, "%d.%d", temp1, temp2);
  draw_text(80, 55, temperature, 1);

  char fmt[9];

  // day, month, year
  snprintf(fmt, 3, "%02u", t.day());
  draw_text(DATE_LOC[0], 0, fmt, 1);
  snprintf(fmt, 3, "%02u", t.month());
  draw_text(DATE_LOC[1], 0, fmt, 1);
  snprintf(fmt, 3, "%02u", t.year());
  draw_text(DATE_LOC[2], 0, fmt, 1);

  strncpy(fmt, TIME_FMT, 9);
  t.toString(fmt);  
  draw_text(20, 24, fmt, 2);
  
  
  // TODO: alarm code goes here
  // if: string comparison
}

void blink_parameter() {
   byte j = 0;
   while (j < 10 && digitalRead(button1) && digitalRead(button2)) {
    j++;
    delay(25);
   }
}

byte edit(int& index, byte x_pos, byte y_pos, byte parameter, int text_size = 1) {
  char text[3];
  byte param_orig = parameter;
  snprintf(text, 3, "%02u", parameter);
  // wait until button1 is released
  while (!digitalRead(button1)) {}
  while (true) {
    // if button2 is pressed:
    while (!digitalRead(button2)) {
      parameter++;
      // day
      if (index == 0 && parameter > 31)
        parameter = 1;
      // month
      if (index == 1 && parameter > 12)
        parameter = 1;
      // year
      if (index == 2 && parameter > 99)
        parameter = 0;
      // hour 
      if (index == 3 && parameter > 23)
        parameter = 0;
      // minute
      if (index == 4 && parameter > 59)
        parameter = 0;

      snprintf(text, 3, "%02u", parameter);
      draw_text(x_pos, y_pos, text, text_size);
      delay(200);
    }
    draw_text(x_pos, y_pos, "  ", text_size);
    blink_parameter();
    draw_text(x_pos, y_pos, text, text_size);
    blink_parameter();
    if (!digitalRead(button1)) {
      index++;
      return parameter;
    }
  }
}

void draw_text(byte x_pos, byte y_pos, char *text, byte text_size) {
  display.setCursor(x_pos, y_pos);
  display.setTextSize(text_size);
  display.print(text);
  display.display();
}

void loop() {
  DateTime d = rtc.now();

  int dow_orig, dow, day, month, year, hour, minute, i;
  dow = d.dayOfTheWeek();
  dow_orig = dow;
 
  if (!digitalRead(button1)) {
    // if button1 is pressed
    i = 0;
    while (!digitalRead(button1)) {}
      // set position of text when editing on button press
      day = edit(i, DATE_LOC[0], 0, d.day());
      month = edit(i, DATE_LOC[1], 0, d.month());
      year = edit(i, DATE_LOC[2], 0, d.year() - 2000) + 2000;
      hour = edit(i, 20, 24, d.hour(), 2);
      minute = edit(i, 56, 24, d.minute(), 2);

      DateTime adjusted(year, month, day, hour, minute, d.second());
      rtc.adjust(adjusted);
      DateTime newtime = rtc.now();
      delay(200);
    }

    display_day(dow);
    
    d = rtc.now();
    watchface_display(d, rtc.getTemperature());
    
    delay(50);
}
