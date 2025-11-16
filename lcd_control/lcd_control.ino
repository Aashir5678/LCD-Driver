#define RS 13      // Selects command register when low, and data register when high
#define RW 12      // Low to write to the register; High to read from the register
#define ENABLE 11  // Sends data to data pins when a high to low pulse is given (give couple ms of delay)

int DATA_PINS[4] = { 6, 5, 4, 3 };  // D4 - D7 (LSB to MSB)

// https://www.electronicsforu.com/technology-trends/learn-electronics/16x2-lcd-pinout-diagram
// https://www.crystalfontz.com/controllers/datasheet-viewer.php?id=97
// https://circuitdigest.com/article/16x2-lcd-display-module-pinout-datasheet

void setup() {
  // put your setup code here, to run once:
  pinMode(RS, OUTPUT);
  pinMode(RW, OUTPUT);
  pinMode(ENABLE, OUTPUT);
  pinMode(BL, OUTPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(DATA_PINS[i], OUTPUT);
  }

  Serial.begin(9600);

  initializeScreen();
}

void loop() {
  writeTextDelay("Hello, world !");
  delay(100);
  clearScreen();

}

void writeCharacter(char c) {
  digitalWrite(RS, HIGH);
  digitalWrite(RW, LOW);

  byte ascii = int(c);
  sendData(ascii);

}

void writeTextDelay(String text) {
  for (int i=0; i < text.length(); i++) {
    writeCharacter(text[i]);
    delay(random(50, 200));
  }

  // delay(100);
}

void writeText(String text) {
  for (int i=0; i < text.length(); i++) {
    writeCharacter(text[i]);
  }
}

void readDataReg() {
  // Reads 8 bit DR and prints the value (MSB -> LSB) in hex

  digitalWrite(RS, HIGH);  // 0 = command reg, 1 = data reg


  digitalWrite(RW, HIGH);


  byte highNibble = 0b0000;
  byte lowNibble = 0b0000;


  digitalWrite(ENABLE, HIGH);

  delayMicroseconds(5);

  for (int i = 0; i < 3; i++) {
    pinMode(DATA_PINS[i], INPUT);
    if (digitalRead(DATA_PINS[i])) {
      highNibble |= 1 << i;
    }


  }

  digitalWrite(ENABLE, LOW);
  delayMicroseconds(2);

  digitalWrite(ENABLE, HIGH);
  delayMicroseconds(5);

  for (int i = 0; i < 3; i++) {
    pinMode(DATA_PINS[i], INPUT);
    if (digitalRead(DATA_PINS[i])) {
      lowNibble |= 1 << i;
    }

    pinMode(DATA_PINS[i], OUTPUT);
  }

  digitalWrite(ENABLE, LOW);

  Serial.print(highNibble, HEX);
  Serial.print(lowNibble, HEX);

  Serial.println();
}

void sendData(byte data) {
  int index = 3;

  digitalWrite(ENABLE, HIGH);
  delayMicroseconds(5);

  for (word mask = 0b10000000; mask > 0; mask >>= 1) {
    if (mask == 8) {  // sending low bit for mask >= 8
      digitalWrite(ENABLE, LOW);
      delayMicroseconds(5);
      digitalWrite(ENABLE, HIGH);
      index = 3;
      
    }

    if (mask & data) {
      digitalWrite(DATA_PINS[index], HIGH);

    }

    else {
      digitalWrite(DATA_PINS[index], LOW);
    }


    index--;

    if (index < 0) {
      index = 3;
    }
  }


  digitalWrite(ENABLE, LOW);

  delayMicroseconds(50); // Minimum 40 micro seconds


}

void executeInstruction(byte opcode) {
  int index = 0;

  digitalWrite(RS, LOW);
  digitalWrite(RW, LOW);
  sendData(opcode);

  if (opcode == 0x01 || opcode == 0x02) {  // Clear screen and return home are slower commands
    delay(2);
  }
}

bool getBusy() {

  digitalWrite(RS, LOW);
  digitalWrite(RW, HIGH);
  digitalWrite(ENABLE, HIGH);
  delayMicroseconds(10);
  pinMode(DATA_PINS[3], INPUT);

  int busy = digitalRead(DATA_PINS[3]); // busy = 1 means command register is currently processing a command, don't send new command until busy = 0
  pinMode(DATA_PINS[3], OUTPUT);

  digitalWrite(ENABLE, LOW);
  digitalWrite(RW, LOW);

  if (busy == 1) {
    return true;
  }

  return false;
}

void waitUntilNotBusy() {
  while (getBusy()) {
    delay(60);
  }
  
}

void initializeScreen() {
  executeInstruction(0x20); // Sets to 4-bit operation
  waitUntilNotBusy();

  executeInstruction(0x20); // Confirmation
  waitUntilNotBusy();

  
  executeInstruction(0x00); // Turn on display / cursor
  waitUntilNotBusy();

  executeInstruction(0x00); // Increment cursor after every write to DR
  waitUntilNotBusy();

  clearScreen();
  goHome();

}

void goHome() {
  // Returns cursor to first character of first line
  executeInstruction(0x02);
  waitUntilNotBusy();
}


void clearScreen() {
  executeInstruction(0x01);
  waitUntilNotBusy();
}
