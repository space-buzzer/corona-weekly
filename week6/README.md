# Real Time Clock
This week we're going to make a Pebble clone! We're going to build a watch with a nice screen and interchangeable watch faces.

Every watch needs a screen, which we already used in [week5](../week5), but a watch also needs a component that can do accurate timekeeping. Our new component this week is a Real Time Clock (RTC) DS3231.



## RTC Example
In this example, we'll use I2C wire protocol to communicate with the RTC. It's going to be low level, bare-bones work of communicating with a component, without the libraries that make this work nicer. The goal is to show that although it's very low level, it's not complicated, and libraries provide nice abstractions, but no magic happens there.

For this examples, no new libraries are needed.

### Code
The full project is available at [rtc_example](rtc_example/rtc_example.ino).

To understand how to communicate with DS3231 RTC we can look at [its datasheet](https://datasheets.maximintegrated.com/en/ds/DS3231M.pdf). The datasheet specifies the "API" for the component, and other information that allow users to use it effectively and in its correct environment and setting (e.g., our component should not be used above 85℃, so it's not suitable as oven thermometer).

DS3231 has 20 registers we can read and write. I'll give a partial address table here, and a full table is available in the datasheet.

| Address | Bits 7 - 4      | Bits 3 - 0 | Function | Range                   |
|---------|-----------------|------------|----------|-------------------------|
| 0x00    | 10 sec          | sec        | Seconds  | 0-59                    |
| 0x01    | 10 min          | min        | Minutes  | 0-59                    |
| 0x02    | AM/PM + 10 hour | hour       | Hours    | 1-12 & AM/PM<br/> 00-23 |

And so on, all the way to 0x12.<br />
Most of the registers are or date time and temperature data, but some are used for control and status, like whether the device was reset (lost power), whether alarms are set, etc.


#### I2C
Let's look at this simple code that connects and reads a byte.


```c
#include <Wire.h>

void setup() {
    // Initialize I2C bus
    Wire.begin();

    byte minutes_address = 0x01;

    // Reading a byte
    Wire.beginTransmission(0x68);
    Wire.write(minutes_address);  // The register to read
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, (byte)1);

    byte res = Wire.read();
}

void loop() {
}
```

The first line `Wire.begin();` initializes the I2C bus. Once 





#### Binary-Coded Decimal






# Structure for each project
1. Short description
2. Summary of new components used/new techniques
3. BOM - bill of materials
4. Circuit diagram
5. Recap
6. [Optional] Extra credit: List of extensions to the project
