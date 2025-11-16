# LCD-Driver
Arduino code to control a 16x2 LCD with a HD44780U driver using 4-bit data transfer mode

Made using data sheet for [HD44780 driver](https://www.crystalfontz.com/controllers/datasheet-viewer.php?id=97)

## Wiring for LCD

<img width="1164" height="696" alt="image" src="https://github.com/user-attachments/assets/e8d0690e-e29a-4aa1-aed4-ccf53f0e632e" />

VDD: GND

VSS: +5 V

Vo: Output from 10k potentiometer, for adjusting the backlight

RS: Pin 13 on Arduino

RW: Pin 12 on Arduino

ENABLE: Pin 11 on Arduino

D4 - D7: Pins 6 - 3 on Arduino

A: +5 V

K: GND

## HD44780U Registers

The data register (DR) and instruction register (IR) are both written to / read from using the pins (D4 - D7).


## Register Select (RW) and Read / Write (RW)
The instruction register is selected when RS = 0, and the data register is selected when RS = 1.

The driver is in write mode when RW = 0, and is in read mode when RW = 1.

## How data is represented and sent in 4-bit mode

Instructions are 8 bit opcodes which tell the LCD to execute predefined commands (clear screen, move cursor, turn on display), and the data is an 8-bit ASCII code for characters (to send characters to LCD).

Instructions are sent to the IR and ASCII codes are sent to the DR.

To transfer / read to a register in 4-bit data transfer mode, we set ENABLE high, read / write the 4 MSB (high nibble) first, set ENABLE low, then high again,
then read / write the 4 LSB (low nibble) and set ENABLE low.

Reading from the instruction register will give the busy flag as the MSB read, and the next 7 bits will be an address.

### Timing Diagram:

<img width="1440" height="856" alt="image" src="https://github.com/user-attachments/assets/c349a63b-22e1-44b1-827b-a01a2c7425cf" />


## Busy Flag

The MSB from a read from the IR will give the busy flag. When the busy flag is 1, that means that LCD driver is currently executing commands. If the busy flag is 0, that means the LCD isn't executing anything and is ready for a new command. Sending commands to the IR when the busy flag is 1 will result in a longer wait time between instructions, longer than if we waited until the busy flag was 0. Checking the busy flag before sending an instruction is important to prevent unnecessary delay.

A typical instruction takes around 37 us (microseconds) according to the data sheet.


## Instruction Register

A complete list of opcodes and their function for 4-bit mode can be found on pages 41 and 40 of the data sheet.



