#include <EEPROM.h>
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

const long SAMPLE_INTERVAL = 5000;
const long REPORT_INTERVAL = 30*60*1000l;
const byte STORE_CODE = 0xb007;

float temperature;
byte second, minute, hour, day_of_week, day, month, year;
char report[] = " 00.00C at 00/00/00 00:00:00";
long ms_since_last_report = REPORT_INTERVAL - SAMPLE_INTERVAL;


typedef struct {
  float temperature;
  byte second, minute, hour, day, month, year;
} Record;


Record new_record(){
  return {temperature, second, minute, hour, day, month, year};
};

void format_record(Record &record, char *text) {
  int temp1 = int(floor(record.temperature));
  int temp2 = record.temperature * 100  - temp1 * 100;
  sprintf(text, "%u.%uC at %02u/%02u/%02u %02u:%02u:%02u", 
    temp1, temp2, record.day, record.month, record.year,
    record.hour, record.minute, record.second
  );
}

Record alltime_high = new_record(), alltime_low = new_record();
Record daily_high = new_record(), daily_low = new_record();


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
  Wire.requestFrom(DS3231_ADDRESS, 1);

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

  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(0x11);
  Wire.endTransmission(false);
  Wire.requestFrom(DS3231_ADDRESS, 2);

  byte temperature_msb = Wire.read();
  byte temperature_lsb = Wire.read();
  temperature = (float)temperature_msb + (temperature_lsb >> 6) * 0.25f;
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

  // try to read previous values from eeprom, or initialize with current 
  // datetime/temperature
  bool success = read_stored_stats();
  if (success) {
    Serial.println("Successfully read stored records from EEPROM");
  } else {
    alltime_high = new_record();
    alltime_low = new_record();
    alltime_low.temperature = 100;
    daily_high = new_record();
    daily_low = new_record();
    daily_low.temperature = 100;
  }
}


// ****** EEPROM Storage ******
void read_stored_record(Record &record, int &offset) {
  record.year = EEPROM.read(offset++);
  record.month = EEPROM.read(offset++);
  record.day = EEPROM.read(offset++);
  record.hour = EEPROM.read(offset++);
  record.minute = EEPROM.read(offset++);
  record.second = EEPROM.read(offset++);

  byte temp1 = EEPROM.read(offset++);
  byte temp2 = EEPROM.read(offset++);
  record.temperature = temp1 + temp2/100.0;
}

bool read_stored_stats(){
  // read the first byte, and compare it to our store code
  int offset = 0;
  byte code = EEPROM.read(offset++);

  if (code != STORE_CODE) {
    // do not have previous values in eeprom, cannot load 
    return false;
  }
  
  // Can read previously stored values from EEPROM
  read_stored_record(alltime_high, offset);
  read_stored_record(alltime_low, offset);
  read_stored_record(daily_high, offset);
  read_stored_record(daily_low, offset);
  return true;
}

void store_record(Record &record, int &offset) {
  EEPROM.update(offset++, record.year);
  EEPROM.update(offset++, record.month);
  EEPROM.update(offset++, record.day);
  EEPROM.update(offset++, record.hour);
  EEPROM.update(offset++, record.minute);
  EEPROM.update(offset++, record.second);
    
  byte temp1 = byte(floor(record.temperature));
  byte temp2 = record.temperature * 100  - temp1 * 100;
  EEPROM.update(offset++, temp1);
  EEPROM.update(offset++, temp2);
}

void store_stats() {
  Serial.println("Storing temperature");
  int offset = 0;
  // start with a secret code
  EEPROM.update(offset++, STORE_CODE);
  store_record(alltime_high, offset);
  store_record(alltime_low, offset);
  store_record(daily_high, offset);
  store_record(daily_low, offset);
}

// ****** (EEPROM Storage) ******


void check_max_min(Record &low, Record &high){
  bool changed = false;
  if (high.temperature < temperature) {
    high = new_record();
    changed = true;
  }
  if (low.temperature > temperature) {
    low = new_record();
    changed = true;
  }

  if (changed) {
    store_stats();
  }

}

void temperature_logging() {
  check_max_min(alltime_low, alltime_high);

  // daily values
  if (day != daily_low.day) {
    daily_low = new_record();
    daily_high = new_record();
  } else {
    check_max_min(daily_low, daily_high);
  }
}

void loop() {
  // Read and print the time
  read_rtc_datetime();
  temperature_logging();

  if (ms_since_last_report >= REPORT_INTERVAL) {  
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
    
    Serial.print(" -- ");
    Serial.print("All time high: ");
    format_record(alltime_high, report);
    Serial.print(report);
    Serial.print(". All time low: ");
    format_record(alltime_low, report);
    Serial.println(report);
  
    Serial.print("                  Daily high: ");
    format_record(daily_high, report);
    Serial.print(report);
    Serial.print(". Daily low: ");
    format_record(daily_low, report);
    Serial.print(report);
  
    Serial.println("");
    ms_since_last_report = 0;
  }
  
  delay(SAMPLE_INTERVAL);
  ms_since_last_report += SAMPLE_INTERVAL;
}
