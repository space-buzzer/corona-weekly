#include <SPI.h>
#include <Wire.h>


/**
 * This examples shows how to use the RealTime Clock module (DS3231).
 * 
 * We'll write to the RTC module and read from it, learn about the encoding 
 * used by DS3231.
 * This is a low level example, that uses the Wire interface directly, without a 
 * library to abstract away communication details.
 * 
 * 
 * The code first checks whether the component lost power, if so, it'll set
 * the date and time from last compile time of the sketch.
 * Then we go on to print the date and time every few seconds
 * 
 * For reference, here's a spec for a DS3231 component by Dallas Semiconductors:
 * https://web.wpi.edu/Pubs/E-project/Available/E-project-010908-124414/unrestricted/DS3231-DS3231S.pdf
 * 
 * BCD = Binary Coded Decimal. We use a single byte for numbers, so 2 digit decimals.
 */

#define DS3231_ADDRESS 0x68


byte second, minute, hour, day_of_week, day, month, year;


/*********************************************
 *   --- Read/Write bytes on wire ---
 *********************************************/

/**
 * Read a single byte from from the address given as argument
 */
byte read_byte(byte register_address) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(register_address);  // The register to read
  Wire.endTransmission(false);
  Wire.requestFrom(DS3231_ADDRESS, (byte)1);

  byte res = Wire.read();
  return res;
}

/**
 * Write a single byte value, given as argument, to the address
 */
void write_byte(byte register_address, byte value) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((byte)register_address); // address
  Wire.write(value);
  Wire.endTransmission();
}

/*********************************************
 *   --- Reading & setting control bits ---
*********************************************/

void set_osf(bool value) {
  byte status_res = read_byte(0x0F);
  status_res &= 0x7F;  // reset the bit
  if (value) 
    status_res |= 0x80;

  write_byte(0x0F, status_res);      
}

bool read_osf() {
  byte res = read_byte(0x0F);
  Serial.print("OSF byte = ");
  Serial.println(res, BIN);
  return (res >> 7); // Oscillator stop flag is on
}


/*********************************************
 *   --- Date/time related functions ---
*********************************************/

/**
 * This function will update the global datetime parameters from the 
 * __DATE__ and __TIME__ special variables set at compile time of 
 * the sketch.
 * These variables are given as a string, and need to be parsed. 
 */
void update_datetime_from_env() {
  Serial.print("Updating datetime from: ");
  Serial.print(__DATE__);
  Serial.print(" | ");
  Serial.println(__TIME__);

  char* date = __DATE__;
  char* time = __TIME__;

  // date: "%b %d %Y", example: Jan 01 2020
  //Jan, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
  switch(date[0]) {
    case 'J': month = (date[1] == 'a' ? 1 : (date[2] == 'n' ? 6 : 7)); break;
    case 'F': month = 2; break;
    case 'M': month = date[2] == 'r' ? 3 : 5; break;
    case 'A': month = date[1] == 'p' ? 4 : 8; break;
    case 'S': month = 9; break;
    case 'O': month = 10; break;
    case 'N': month = 11; break;
    case 'D': month = 12; break;
    // else: nothing!
  }

  day = (date[4] - '0') * 10 + (date[5] - '0');
  // leaving this open for Y3K bugs in the future, reading only last 2 chars
  year = (date[9] - '0') * 10 + (date[10] - '0');

  
  // time: "%H:%M:%S" example: 18:24:00
  hour = (time[0] - '0') * 10 + (time[1] - '0');
  minute = (time[3] - '0') * 10 + (time[4] - '0');

  // TODO: day of week needs to be calculated
}


/**
 * Send the given decimal number over the wire interface as BCD
 * Works only for byte size -- 2 digit numbers 0-99
 */
void wire_as_bcd(byte num) {
  byte foo = ((num / 10) << 4) + num % 10;
  foo &= 0xFF;
  Wire.write(foo);
}


void set_rtc_datetime() {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write((byte)0); // address
  Wire.write(second);
  wire_as_bcd(minute);
  wire_as_bcd(hour);
  wire_as_bcd(day_of_week);
  wire_as_bcd(day);
  wire_as_bcd(month);
  wire_as_bcd(year);
  Wire.endTransmission();
}


/**
 * Read from wire and convert BCD to decimal.
 * Works for numbers encoded in a single byte, 0-99
 */
byte read_byte_as_dec(){
  byte num = Wire.read();
  return (num & 0x0F) + ((num >> 4) * 10);
}

void read_rtc_datetime() {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0);
  Wire.endTransmission(false);
  Wire.requestFrom(DS3231_ADDRESS, 7);

  second = read_byte_as_dec(); // TODO: macro?
  minute = read_byte_as_dec();
  hour = read_byte_as_dec();
  day_of_week = read_byte_as_dec();
  day = read_byte_as_dec();
  month = read_byte_as_dec();
  year = read_byte_as_dec();
}



void setup() {
  Wire.begin();
  
  Serial.begin(9600);
  Serial.println("--- ** ---");

  // Set the time if it was reset
  bool restart = read_osf();
  Serial.print("Powered down? ");
  Serial.println(restart);
  if (restart) {
    // set the time
    update_datetime_from_env();
    set_rtc_datetime();
    
    // update OSF bit
    set_osf(false);    
  }  
}


void loop() {
  // Read and print the time
  read_rtc_datetime();
  
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.print(second);
  Serial.println("");
   

  delay(4000);
}
