#include "Watchface.h"

RetroWatch::Watchface(&Adafruit_SSD1306 display) {
  
}
void RetroWatch::begin(&Adafruit_SSD1306 display){
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

void RetroWatch::update(&Adafruit_SSD1306 display, const DateTime& t){
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
