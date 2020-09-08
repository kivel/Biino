
/* include libs */
#include <IRremote.h>
#include <EEPROM.h>
#include <SPI.h>
#include <LiquidCrystal.h>

/* Pin Configuration */

const int ChipSelect = 4;                   // Pin to select the relais boards
const int Encoder1 = 2;                     // Pin for the encoder, must be attached to interrupt
const int S1 = 3;                           // Button to load the settings to the EEPROM
const int LCDbacklight = 5;                 // Pin for LCD backlight dimming, PWM necessary
const int RECV_PIN = 6;                     // Pin for IR receiver input
const int EncoderButton = 7;                // Pin for the encoder button
const int Encoder2 = 8;                     // Pin for the second encoder connection
const int redLED = 9;                       // Pin for the red led
const int greenLED = 10;                    // Pin for the green led
// MOSI = 11  --> do not change!
// MISO = 12  --> do not change!
// SCK  = 13  --> do not change!
LiquidCrystal lcd(A5, A4, A2, A3, A0, A1);  // settings for lcd (rs, en, d4, d5, d6, d7)
const int RemoteRelais = 0;                 // Pin for the Biino Power Relais
const int PowerButton = 1;                 // Pin for the Power Button


/* settings for IR */
IRrecv irrecv(RECV_PIN);
decode_results results;
boolean RECV_TYPE = true;
int irType = 0;

/* settings for Volume */
int Volume = 0;
int OldVolume = Volume;
boolean mute = false;
int MuteVol = Volume;

/* settings for Input */
int Input = EEPROM.read(1);
int OldInput = Input;
int input_count = EEPROM.read(2);

/* settings for buttons */
long lastDebounceTime = 0;
int debounceDelay = 500;

boolean button_Up = false;
boolean button_Down = false;
boolean button_Left = false;
boolean button_Right = false;
boolean button_Enter = false;
boolean button_Menu = false;
boolean button_Play = false;
boolean button_Channel = false;
boolean menu_action = false;
boolean last_volume = false;
boolean in_menu = false;
boolean button_Power = false;

/* settings for SPI communication */
#define IOCON 0x05 // Control register
#define GPIO 0x09 // InOut register

/* settings for backlight dimming */
int backlightdimming = EEPROM.read(9);

/* settings for the DUO-LED */
int red = 0;
int green = 255;
int stepup = 4;

/* general settings */
long lastChange = 0;
long currentMillis = 0;
long lastMillis = 0;
int encInterval = 50;
int startup_interval = 150;
int count = 0;
int menu_count = 0;
int spi_input = 0;
int relay_delay = 0;
int location = 0;


byte bar1[8] =																	// Create custom characters
{
  B11100,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11100
};
byte bar2[8] =
{
  B00111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B01111,
  B00111
};
byte bar3[8] =
{
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte bar4[8] =
{
  B11110,
  B11100,
  B00000,
  B00000,
  B00000,
  B00000,
  B11000,
  B11100
};
byte bar5[8] =
{
  B01111,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00011,
  B00111
};
byte bar6[8] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B11111
};
byte bar7[8] =
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00111,
  B01111
};
byte bar8[8] =
{
  B11111,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
void rotEncoder1()																// Subroutine for the rotary encoder
{
  if (digitalRead(Encoder1) == digitalRead(Encoder2))
  {

    if (in_menu == true)
    {
      button_Left = true;
    }
    else
    {
      button_Down = true;
    }
  }
  else
  {
    if (in_menu == true)
    {
      button_Right = true;
    }
    else
    {
      button_Up = true;
    }
  }
}
void custom0(int col)															// Create custom numbers
{
  // uses segments to build the number 0
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(8);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}
void custom1(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}
void custom2(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(6);
}
void custom3(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(5);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}
void custom4(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}
void custom5(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}
void custom6(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(4);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}
void custom7(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(8);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(32);
  lcd.write(32);
  lcd.write(1);
}
void custom8(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(2);
  lcd.write(6);
  lcd.write(1);
}
void custom9(int col)
{
  lcd.setCursor(col, 0);
  lcd.write(2);
  lcd.write(3);
  lcd.write(1);
  lcd.setCursor(col, 1);
  lcd.write(7);
  lcd.write(6);
  lcd.write(1);
}
void clear_buttons()
{
  button_Up = false;
  button_Down = false;
  button_Left = false;
  button_Right = false;
  button_Enter = false;
  button_Menu = false;
  button_Play = false;
  button_Channel = false;
  menu_action = false;
  button_Power = false;
}
void printNumber(int value, int col)											// Print custom numbers
{
  if (value == 0)
  {
    custom0(col);
  }
  if (value == 1)
  {
    custom1(col);
  }
  if (value == 2)
  {
    custom2(col);
  }
  if (value == 3)
  {
    custom3(col);
  }
  if (value == 4)
  {
    custom4(col);
  }
  if (value == 5)
  {
    custom5(col);
  }
  if (value == 6)
  {
    custom6(col);
  }
  if (value == 7)
  {
    custom7(col);
  }
  if (value == 8)
  {
    custom8(col);
  }
  if (value == 9)
  {
    custom9(col);
  }
}
void printVolume(int vol)														// Print Volume on display
{
  int d, u, number;
  number = vol;
  if (number > 9)
  {
    d = (number - (number % 10)) / 10;
    number = number % 10;
  }
  else
  {
    d = 0;
  }

  u = number;

  printNumber(d, 14);
  printNumber(u, 17);

}
void change_volume()
{
  if (Volume == 0)
  {
    digitalWrite(ChipSelect, LOW);  								      // take the CS pin low to select the chip:
    SPI.transfer(0b01000000);          									  // Optcode to write to MCP23S17 #1
    SPI.transfer(GPIO);               									  // Select GPIO register for Volume
    SPI.transfer(0xFF);              									    // Clear Volume
    digitalWrite(ChipSelect, HIGH); 								      // take the CS pin high to de-select the chip:
  }
  else
  {
    digitalWrite(ChipSelect, LOW);  								      // take the CS pin low to select the chip:
    SPI.transfer(0b01000000);          									  // Optcode to write to MCP23S17 #1
    SPI.transfer(GPIO);               									  // Select GPIO register for Volume
    SPI.transfer(0xFF);              									    // Clear Volume
    digitalWrite(ChipSelect, HIGH); 								      // take the CS pin high to de-select the chip:

    delay(5);															                // Anti "pop" delay

    int volnew = Volume ^ 0xFF;												    // Bit manipulation to set the correct value
    digitalWrite(ChipSelect, LOW);  								      // take the CS pin low to select the chip:
    SPI.transfer(0b01000000);          									  // Optcode to write to MCP23S17 #1
    SPI.transfer(GPIO);               									  // Select GPIO register for Volume
    SPI.transfer(volnew);           									    // Set Volume
    digitalWrite(ChipSelect, HIGH);								        // take the CS pin high to de-select the chip:
  }

  printVolume(Volume);
  OldVolume = Volume;
  green = 252 - (Volume * stepup);
  analogWrite(greenLED, green);
  red = 0 + (Volume * stepup);
  analogWrite(redLED, red);
  if (last_volume == true)
  {
    EEPROM.write(4, Volume);
  }
}
void printInput(int Input)														      // Print Input on display and set relay
{
  int v = 0;
  if (Input <= input_count)
  {
    lcd.setCursor(0, 1);
    for (int i = 0; i <  input_count; i++)
    {
      lcd.print("-");
    }

    int a = Input - 1;
    lcd.setCursor(a, 1);
    lcd.write(255);

    lcd.setCursor(0, 0);
    for (int i = 0; i <  14; i++)
    {
      lcd.write(254);
    }

    digitalWrite(ChipSelect, LOW);
    SPI.transfer(0b01000110);
    SPI.transfer(GPIO);
    SPI.transfer(0xFF);
    digitalWrite(ChipSelect, HIGH);

    lcd.setCursor(0, 0);
    switch (Input)
    {
      case (1):
        location = 100;
        spi_input = 0xFE;
        break;
      case (2):
        location = 120;
        spi_input = 0xFD;
        break;
      case (3):
        location = 140;
        spi_input = 0xFB;
        break;
      case (4):
        location = 160;
        spi_input = 0xF7;
        break;
      case (5):
        location = 180;
        spi_input = 0xEF;
        break;
      case (6):
        location = 200;
        spi_input = 0xDF;
        break;
    }

    digitalWrite(ChipSelect, LOW);
    SPI.transfer(0b01000110);
    SPI.transfer(GPIO);
    SPI.transfer(spi_input);
    digitalWrite(ChipSelect, HIGH);


    EEPROM.write(1, Input);
    lcd.setCursor(0, 0);
    int y = location;
    int x = (location + 12);
    for (y; y < x; y++)
    {
      lcd.write(EEPROM.read(y));
    }
  }
}
void decodeIR(decode_results * results)											// Decode IR type
{

  int count = results->rawlen;
  if (results->decode_type == UNKNOWN)
  {
    //lcd.print("Received Unknowm");
  }
  else if (results->decode_type == NEC)
  {
    irType = 1;
  }
  else if (results->decode_type == SONY)
  {
    irType = 2;
  }
  else if (results->decode_type == RC5)
  {
    irType = 3;
  }
  else if (results->decode_type == RC6)
  {
    irType = 4;
  }
}
int decodeIRstartup(decode_results * results)									// Decode IR at startup
{
  int count = results->rawlen;
  if (results->decode_type == UNKNOWN)
  {
    lcd.print("Received Unknowm");
  }
  else if (results->decode_type == NEC)
  {
    lcd.print("NEC");
    // Predefined IR codes for Apple Remote
    EEPROM.write (10, 0x3A);  // Enter
    EEPROM.write (11, 0x50);  // Up
    EEPROM.write (12, 0x30);  // Down
    EEPROM.write (13, 0x10);  // Left
    EEPROM.write (14, 0x60);  // Right
    EEPROM.write (15, 0x40);  // Menu
    EEPROM.write (16, 0x7A);  // Play
    EEPROM.write (17, 0x01);  // Marker for Apple Remote
  }
  else if (results->decode_type == SONY)
  {
    EEPROM.write (10, 0x00);  // Enter
    EEPROM.write (11, 0x00);  // Up
    EEPROM.write (12, 0x00);  // Down
    EEPROM.write (13, 0x00);  // Left
    EEPROM.write (14, 0x00);  // Right
    EEPROM.write (15, 0x00);  // Menu
    EEPROM.write (16, 0x00);  //
  }
  else if (results->decode_type == RC5)
  {
    lcd.print("RC5");
    irType = 3;
    EEPROM.write (10, 0x3F);  // Enter
    EEPROM.write (11, 0x10);  // Up
    EEPROM.write (12, 0x11);  // Down
    EEPROM.write (13, 0x21);  // Left
    EEPROM.write (14, 0x20);  // Right
    EEPROM.write (15, 0x0C);  // Menu
    EEPROM.write (16, 0x0D);  // Mute
  }
  else if (results->decode_type == RC6)
  {
    EEPROM.write (10, 0x00);  // Enter
    EEPROM.write (11, 0x00);  // Up
    EEPROM.write (12, 0x00);  // Down
    EEPROM.write (13, 0x00);  // Left
    EEPROM.write (14, 0x00);  // Right
    EEPROM.write (15, 0x00);  // Menu
    EEPROM.write (16, 0x00);  //
  }
}
void learn_IR(int menu_count)													// Learn and store IR codes
{
  if (irrecv.decode(&results))												// Receive IR commands
  {
    decodeIR(&results);

    if (irType == 1)														// Decode IR codes for Apple Remote
    {
      unsigned long a = results.value >> 8;
      int ircode = lowByte(a);
      bitClear(ircode, 7);
      EEPROM.write(menu_count, ircode);
      lcd.setCursor(10, 1);
      lcd.print(ircode);
      lcd.write(32);
      lcd.write(32);
      delay(500);

    }

    else if (irType == 2)													// Decode IR codes for Sony Remote
    {


    }

    else if (irType == 3)													// Decode IR codes for RC5 Remote
    {
      unsigned long a = results.value >> 6;
      int ircode = lowByte(a);
      bitClear(ircode, 7);
      bitClear(ircode, 6);
      bitClear(ircode, 5);
      EEPROM.write(17, ircode);


      unsigned long b = results.value;
      ircode = lowByte(b);
      bitClear(ircode, 7);
      bitClear(ircode, 6);
      EEPROM.write(menu_count, ircode);
      lcd.setCursor(10, 1);
      lcd.print(ircode);
      lcd.write(32);
      lcd.write(32);
      delay(500);

    }

    else if (irType == 4)													// Decode IR codes for RC6 Remote
    {


    }

    irrecv.resume();             // Receive the next value
  }
}
void setup()																	// Setup
{

  pinMode(RemoteRelais, OUTPUT);
  digitalWrite(RemoteRelais, HIGH);
  pinMode(EncoderButton, INPUT);
  pinMode(S1, INPUT);
  pinMode(Encoder1, INPUT);
  digitalWrite(Encoder1, HIGH);
  pinMode(Encoder2, INPUT);
  digitalWrite(Encoder2, HIGH);
  analogWrite(LCDbacklight, backlightdimming);
  pinMode(PowerButton, INPUT);

  /* Setup SPI communication */
  pinMode (ChipSelect, OUTPUT);     // Set ChipSelectPin as output
  digitalWrite(ChipSelect, HIGH);   // take the CS pin high to de-select the chip:
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);

  digitalWrite(ChipSelect, LOW);         // take the CS pin low to select the chip:
  SPI.transfer(0b01000000);              // Optcode to write to MCP23S17
  SPI.transfer(0x00);                    // Select IODIR register
  SPI.transfer(0x00);                    // Set GPIO as output
  digitalWrite(ChipSelect, HIGH);        // take the CS pin high to de-select the chip:

  digitalWrite(ChipSelect, LOW);         // take the CS pin low to select the chip:
  SPI.transfer(0b01000000);              // Optcode to write to MCP23S17
  SPI.transfer(IOCON);                   // Select IOCON register
  SPI.transfer(0b00001000);              // Set Hardware Adress Enable
  digitalWrite(ChipSelect, HIGH);        // take the CS pin high to de-select the chip:

  /* Set start volume */
  if (EEPROM.read(3) < 1)
  {
    last_volume = true;
    Volume = EEPROM.read(4);
  }
  else
  {
    last_volume = false;
    Volume = EEPROM.read(3);
  }
  int v = (Volume - EEPROM.read(30));
  int volnew = v ^ 0xFF;												// Bit manipulation to set the correct value
  digitalWrite(ChipSelect, LOW);  								// take the CS pin low to select the chip:
  SPI.transfer(0b01000000);          									// Optcode to write to MCP23S17 #1
  SPI.transfer(GPIO);               									// Select GPIO register for Volume
  SPI.transfer(volnew);           									// Set Volume
  digitalWrite(ChipSelect, HIGH);								// take the CS pin high to de-select the chip:

  /* Set start Input */
  Input = EEPROM.read(1);
  if (Input > input_count)
  {
    (Input = 1);
  }

  /* create charakters for LCD */
  lcd.createChar(1, bar1);
  lcd.createChar(2, bar2);
  lcd.createChar(3, bar3);
  lcd.createChar(4, bar4);
  lcd.createChar(5, bar5);
  lcd.createChar(6, bar6);
  lcd.createChar(7, bar7);
  lcd.createChar(8, bar8);


  /* Start the IR-receiver */
  irrecv.enableIRIn();


  /* Start the red/green LED */
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  green = 252 - (Volume * stepup);
  analogWrite(greenLED, green);
  red = 0 + (Volume * stepup);
  analogWrite(redLED, red);

  lcd.begin(20, 2);
  lcd.noDisplay();
  delay(200);
  lcd.display();
  lcd.setCursor(6, 0);
  lcd.print("BIINO MK3");
  lcd.setCursor(0, 1);

  while (count < 21)
  {
    currentMillis = millis();
    if (currentMillis - lastMillis > startup_interval)
    {
      count++;
      lcd.write(255);
      lastMillis = millis();
    }

    if (irrecv.decode(&results))
    {
      decodeIRstartup(&results);
      if (irType == 3)
      {
        unsigned long a = results.value >> 6;
        int ircode = lowByte(a);
        bitClear(ircode, 7);
        bitClear(ircode, 6);
        bitClear(ircode, 5);
        EEPROM.write (17, ircode); // Device address
      }
      irrecv.resume();
    }
  }

  //lcd.clear();
  lcd.setCursor(0, 0);
  for (int i = 0; i <  20; i++)
  {
    lcd.write(254);
  }
  lcd.setCursor(0, 1);
  for (int i = 0; i <  20; i++)
  {
    lcd.write(254);
  }
  lcd.setCursor(0, 0);
  printInput(Input);
  printVolume(Volume);
  attachInterrupt(0, rotEncoder1, CHANGE);  // ISR for rotary encoder
  clear_buttons();

}
void loop()																		// Loop
{
  control();

  actions();
}

void actions()																	// Execute tasks
{
  if ((millis() - lastChange) > encInterval)
  {
    if (button_Up == true)
    {
      button_Up = false;
      if (Volume < 63)
      {
        (++Volume);
      }
    }
    else if (button_Down == true)
    {
      button_Down = false;
      if (Volume > 0)
      {
        (--Volume);
        if (Volume == 0)
        {
          mute = false;
          Volume = 0;
        }
      }
    }
  }
  if (OldVolume != Volume)
  {
    change_volume();
  }
  if (button_Left == true)
  {
    if (Input > 1)(--Input);
    printInput(Input);
    button_Left = false;
  }
  if (button_Right == true)
  {
    if (Input < input_count)(++Input);
    printInput(Input);
    button_Right = false;
  }
  if (button_Channel == true)
  {
    if (Input < input_count)
    {
      (++Input);
    }
    else
    {
      Input = 1;
    }
    printInput(Input);

    button_Channel = false;
  }
  if (button_Enter == true)													// Enter Menu
  {
    button_Enter = false;
    mainmenu();
  }
  if (button_Menu == true)													// Enter Menu
  {
    button_Menu = false;
    mainmenu();
  }
  if (button_Power == true)                          // Enter Menu
  {
    button_Power = false;                           // Enter Standby
    standby();
  }
}

void control()																	// Check control and input devices
{
  if ((millis() - lastDebounceTime) > debounceDelay)							// Delay for push buttons
  {
    int readS1 = digitalRead(S1);
    if (readS1 == HIGH)
    {
      factory_settings();
      lastDebounceTime = millis();
    }

    int readPowerButton = digitalRead(PowerButton);
    if (readPowerButton == HIGH)
    {
      button_Power = true;
      lastDebounceTime = millis();
    }


    int readEncoderButton = digitalRead(EncoderButton);
    if (readEncoderButton == HIGH)
    {
      if (Volume == 0)
      {
        button_Enter = true;
      }
      else
      {
        button_Channel = true;
      }
      if (in_menu == true)(button_Enter = true);
      lastDebounceTime = millis();
    }
  }
  if (irrecv.decode(&results))												// Receive IR commands
  {
    decodeIR(&results);														// Decode IR type

    if (irType == 1)														// Decode IR codes for Apple Remote
    {
      unsigned long a = results.value >> 8;
      int ircode = lowByte(a);
      bitClear(ircode, 7);

      if (ircode == EEPROM.read(10))
      {
        button_Enter = true;
      }
      if (ircode == EEPROM.read(11))
      {
        button_Up = true;
      }
      else if (ircode == EEPROM.read(12))
      {
        button_Down = true;
      }
      else if (ircode == EEPROM.read(13))
      {
        button_Left = true;
      }
      else if (ircode == EEPROM.read(14))
      {
        button_Right = true;
      }
      else if (ircode == EEPROM.read(15))  // Menu Button
      {
        button_Power = true;
      }
      else if (ircode == EEPROM.read(16))
      {
        mute = !mute;
        if (mute == true)
        {
          MuteVol = Volume;
          Volume = 0;
        }
        else
        {
          Volume = MuteVol;
        }
      }
    }
    else if (irType == 2)													// Decode IR codes for Sony Remote
    {


    }
    else if (irType == 3)													// Decode IR codes for RC5 Remote
    {
      unsigned long a = results.value >> 6;
      int ircode = lowByte(a);
      bitClear(ircode, 7);
      bitClear(ircode, 6);
      bitClear(ircode, 5);


      if (ircode == EEPROM.read(17))
      {
        unsigned long a = results.value;
        byte ircode = lowByte(a);
        bitClear(ircode, 7);
        bitClear(ircode, 6);

        if (ircode == EEPROM.read(10))
        {
          button_Enter = true;
        }
        if (ircode == EEPROM.read(11))
        {
          button_Up = true;
        }
        else if (ircode == EEPROM.read(12))
        {
          button_Down = true;
        }
        else if (ircode == EEPROM.read(13))
        {
          button_Left = true;
        }
        else if (ircode == EEPROM.read(14))
        {
          button_Right = true;
        }
        else if (ircode == EEPROM.read(15))
        {
          button_Power = true;
        }
        else if (ircode == EEPROM.read(16))
        {
          mute = !mute;
          if (mute == true)
          {
            MuteVol = Volume;
            Volume = 0;
          }
          else
          {
            Volume = MuteVol;
          }
        }
        else if (ircode == 1)
        {
          Input = 1;
          printInput(Input);
        }
        else if (ircode == 2)
        {
          Input = 2;
          printInput(Input);
        }
        else if (ircode == 3)
        {
          Input = 3;
          printInput(Input);
        }
        else if (ircode == 4)
        {
          Input = 4;
          printInput(Input);
        }
        else if (ircode == 5)
        {
          Input = 5;
          printInput(Input);
        }
        else if (ircode == 6)
        {
          Input = 6;
          printInput(Input);
        }
        else if (ircode == 7)
        {
          Input = 7;
          printInput(Input);
        }
        else if (ircode == 8)
        {
          Input = 8;
          printInput(Input);
        }
        else if (ircode == 9)
        {
          Input = 9;
          printInput(Input);
        }
        if (Input > input_count)
        {
          Input = EEPROM.read(1);
        }
      }
    }
    else if (irType == 4)													// Decode IR codes for RC6 Remote
    {


    }
    delay(50);
    irrecv.resume();             											// Receive the next value
  }
}

void standby()
{
  lcd.noDisplay();
  analogWrite(LCDbacklight, 0);
  analogWrite(greenLED, 0);
  analogWrite(redLED, 0);
  digitalWrite(RemoteRelais, LOW);

  digitalWrite(ChipSelect, LOW);      // Clear Input Relays
  SPI.transfer(0b01000110);
  SPI.transfer(GPIO);
  SPI.transfer(0xFF);
  digitalWrite(ChipSelect, HIGH);

  digitalWrite(ChipSelect, LOW);      // Clear Volume Relays
  SPI.transfer(0b01000000);
  SPI.transfer(GPIO);
  SPI.transfer(0xFF);
  digitalWrite(ChipSelect, HIGH);
  delay(1000);
  while (menu_action == false)
  {
    control();

    if (button_Power == true)
    {
      menu_action = true;
    }
  }

  lcd.display();
  analogWrite(LCDbacklight, backlightdimming);

  int volnew = Volume ^ 0xFF;                           // Bit manipulation to set the correct value
  digitalWrite(ChipSelect, LOW);                        // take the CS pin low to select the chip:
  SPI.transfer(0b01000000);                             // Optcode to write to MCP23S17 #1
  SPI.transfer(GPIO);                                   // Select GPIO register for Volume
  SPI.transfer(volnew);                                 // Set Volume
  digitalWrite(ChipSelect, HIGH);                       // take the CS pin high to de-select the chip:

  green = 252 - (Volume * stepup);
  analogWrite(greenLED, green);
  red = 0 + (Volume * stepup);
  analogWrite(redLED, red);

  printInput(Input);
  lastChange = millis();
  clear_buttons();
  digitalWrite(RemoteRelais, HIGH);
  delay(500);

}

void mainmenu()																	// Main Menu
{
  clear_buttons();
  in_menu = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(127);
  lcd.setCursor(19, 0);
  lcd.write(126);
  switch (menu_count)
  {
    case (0):
      lcd.setCursor(3, 0);
      lcd.print("learn ir-codes");
      break;
    case (1):
      lcd.setCursor(4, 0);
      lcd.print("input names");
      break;
    case (2):
      lcd.setCursor(4, 0);
      lcd.print("start volume");
      break;
    case (3):
      lcd.setCursor(2, 0);
      lcd.print("number of inputs");
      break;
    case (4):
      lcd.setCursor(5, 0);
      lcd.print("backlight");
      break;
    case (5):
      lcd.setCursor(8, 0);
      lcd.print("exit");
      break;
  }
  while (menu_action == false)
  {
    control();
    if (button_Right == true)
    {
      if (menu_count < 5)(++menu_count);
      else
      {
        menu_count = 0;
      }
      menu_action = true;
    }
    if (button_Left == true)
    {
      if (menu_count > 0)(--menu_count);
      else
      {
        menu_count = 5;
      }
      menu_action = true;
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      exit();
      return;
    }
  }
  if (button_Enter == true)
  {
    clear_buttons();
    switch (menu_count)
    {
      case (0):
        menu_count = 10;
        submenu0();
        break;
      case (1):
        menu_count = 1;
        count = 1;
        submenu1();
        break;
      case (2):
        submenu2();
        break;
      case (3):
        submenu3();
        break;
      case (4):
        submenu4();
        break;
      case (5):
        exit();
        return;
        break;
    }
  }
  mainmenu();
}
void exit()																		// Exit from Menu
{
  delay(100);
  menu_count = 0;
  lcd.clear();
  if (Input > input_count)(Input = 1);
  change_volume();
  in_menu = false;
  mute = false;
  clear_buttons();
  printInput(Input);
  printVolume(Volume);
}
void submenu0()																	// Learn IR codes
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("received:");
  lcd.setCursor(0, 0);
  lcd.write(127);
  lcd.setCursor(19, 0);
  lcd.write(126);
  lcd.setCursor(2, 0);
  switch (menu_count)
  {
    case (10):
      lcd.print("learn: Enter");
      break;
    case (11):
      lcd.print("learn: Up");
      break;
    case (12):
      lcd.print("learn: Down");
      break;
    case (13):
      lcd.print("learn: Left");
      break;
    case (14):
      lcd.print("learn: Right");
      break;
    case (15):
      lcd.print("learn: Menu");
      break;
    case (16):
      lcd.print("learn: Mute");
      break;
    case (17):
      lcd.print("      back    ");
      break;
  }
  while (menu_action == false)
  {
    control();
    learn_IR(menu_count);

    if (button_Right == true)
    {
      if (menu_count < 17)(++menu_count);
      menu_action = true;
    }
    if (button_Left == true)
    {
      if (menu_count > 10)(--menu_count);
      menu_action = true;
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  if (button_Enter == true)
  {
    clear_buttons();
    if (menu_count == 17)
    {
      menu_count = 0;
      return;
    }

  }
  clear_buttons();
  submenu0();
}
void submenu1()																	// Input Names
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(127);
  lcd.setCursor(19, 0);
  lcd.write(126);
  lcd.setCursor(2, 0);
  lcd.print("input name ");
  switch (menu_count)
  {
    case (1):
      lcd.print(menu_count);
      break;
    case (2):
      lcd.print(menu_count);
      break;
    case (3):
      lcd.print(menu_count);
      break;
    case (4):
      lcd.print(menu_count);
      break;
    case (5):
      lcd.print(menu_count);
      break;
    case (6):
      lcd.print(menu_count);
      break;
    case (7):
      lcd.setCursor(2, 0);
      lcd.print("      back    ");
      break;
  }
  while (menu_action == false)
  {
    control();

    if (button_Right == true)
    {
      if (menu_count < 7)(++menu_count);
      else {
        menu_count = 1;
      }
      menu_action = true;
    }
    if (button_Left == true)
    {
      if (menu_count > 1)(--menu_count);
      else {
        menu_count = 7;
      }
      menu_action = true;
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  if (button_Enter == true)
  {
    clear_buttons();
    if (menu_count == 7)
    {
      menu_count = 1;
      return;
    }
    switch (menu_count)
    {
      case (1):
        location = 100;
        break;
      case (2):
        location = 120;
        break;
      case (3):
        location = 140;
        break;
      case (4):
        location = 160;
        break;
      case (5):
        location = 180;
        break;
      case (6):
        location = 200;
        break;
    }

    submenu101();
  }

  clear_buttons();
  submenu1();
}

void submenu2()																	// Start Volume
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("start volume");
  lcd.setCursor(13, 1);
  lcd.print("0=last");
  lcd.setCursor(0, 1);
  lcd.print(EEPROM.read(3));
  int x = EEPROM.read(3);
  while (menu_action == false)
  {
    control();

    if (button_Right == true)
    {
      button_Right = false;
      if (x < 30)
      {
        (++x);
        lcd.setCursor(0, 1);
        lcd.print(x);
        lcd.write(32);

      }
    }
    if (button_Left == true)
    {
      button_Left = false;
      if (x  > 0)
      {
        (--x);
        lcd.setCursor(0, 1);
        lcd.print(x);
        lcd.write(32);
        button_Left = false;
      }
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  clear_buttons();
  EEPROM.write(3, x);
  last_volume = true;
  menu_count = 2;
  return;
}
void submenu3()																	// Number of Inputs
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("number of inputs");
  lcd.setCursor(0, 1);
  lcd.print(input_count);
  while (menu_action == false)
  {
    control();
    if (button_Right == true)
    {
      button_Right = false;
      if (input_count < 6)
      {
        (++input_count);
        lcd.setCursor(0, 1);
        lcd.print(input_count);
        lcd.write(32);
      }
    }
    if (button_Left == true)
    {
      button_Left = false;
      if (input_count  > 1)
      {
        (--input_count);
        lcd.setCursor(0, 1);
        lcd.print(input_count);
        lcd.write(32);
      }
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  clear_buttons();
  EEPROM.write(2, input_count);
  menu_count = 3;
  return;
}

void submenu4()																	// Backlight dimming
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("set backlight value");
  lcd.setCursor(0, 1);
  lcd.print(backlightdimming);
  while (menu_action == false)
  {
    control();
    if (button_Right == true)
    {
      button_Right = false;
      if (backlightdimming < 255)
      {
        (++backlightdimming);
        lcd.setCursor(0, 1);
        lcd.print(backlightdimming);
        lcd.write(32);
        lcd.write(32);
        analogWrite(LCDbacklight, backlightdimming);
      }
    }
    if (button_Left == true)
    {
      button_Left = false;
      if (backlightdimming > 0)
      {
        (--backlightdimming);
        lcd.setCursor(0, 1);
        lcd.print(backlightdimming);
        lcd.write(32);
        lcd.write(32);
        analogWrite(LCDbacklight, backlightdimming);

      }
    }
    if (button_Enter == true)
    {
      menu_action = true;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  clear_buttons();
  EEPROM.write(9, backlightdimming);
  menu_count = 4;
  return;
}

void submenu101()																// Store input names
{
  clear_buttons();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("input ");
  lcd.print(menu_count);
  lcd.setCursor(12, 1);
  lcd.write(62);
  lcd.setCursor(0, 1);
  int y = location;
  int character = EEPROM.read(location);
  int x = location + 12;
  for (y; y < x; y++)		// Display stored name
  {
    lcd.write(EEPROM.read(y));
  }
  y = location;
  lcd.setCursor(0, 1);
  lcd.blink();
  int c = 0;
  while (c < 12)
  {
    control();
    lcd.setCursor(c, 1);
    if (button_Right == true)
    {
      if (character < 127)
      {
        (++character);
      }
      else
      {
        character = 32;
      }
      lcd.setCursor(c, 1);
      lcd.write(character);
      button_Right = false;
    }

    if (button_Left == true)
    {
      if (character > 32)
      {
        (--character);
      }
      else
      {
        character = 127;
      }
      lcd.setCursor(c, 1);
      lcd.write(character);
      button_Left = false;
    }
    if (button_Enter == true)
    {
      EEPROM.write(y, character);
      ++c;
      ++y;
      character = EEPROM.read(y);
      button_Enter = false;
    }
    if (button_Menu == true)
    {
      return;
    }
  }
  lcd.noBlink();
}


void factory_settings()
{
  EEPROM.write (1,   1); 		  // Start Input
  EEPROM.write (2,   6); 		  // Numder of Inputs
  EEPROM.write (3,  10); 		  // Start Volume
  EEPROM.write (4,  10);      // Last Volume
  EEPROM.write (9, 255); 		  // Backlight dimming
  EEPROM.write (10, 0x3A);  	// Enter
  EEPROM.write (11, 0x50);  	// Up
  EEPROM.write (12, 0x30);  	// Down
  EEPROM.write (13, 0x10);  	// Left
  EEPROM.write (14, 0x60);  	// Right
  EEPROM.write (15, 0x40);  	// Menu
  EEPROM.write (16, 0x7A);  	// Play
  EEPROM.write (17, 0x01);  	// Marker for Apple Remote
  EEPROM.write (30,  0); 		  // Power Relay Delay
  EEPROM.write (31,  0); 		  // Att. board 2
  EEPROM.write (32,  0); 		  // Att. board 3
  EEPROM.write (33,  0); 		  // Att. board 4
  EEPROM.write (34,  0); 		  // max. attenuation

  EEPROM.write (100,   73);	// I
  EEPROM.write (101,   78);	// N
  EEPROM.write (102,   80);	// P
  EEPROM.write (103,   85);	// U
  EEPROM.write (104,   84);	// T
  EEPROM.write (105,   32);	// 	blank
  EEPROM.write (106,   49);	// 1
  EEPROM.write (107,   32);	//	blank
  EEPROM.write (108,   32);	//	blank
  EEPROM.write (109,   32);	//	blank
  EEPROM.write (110,   32);	//  blank
  EEPROM.write (111,   32);	//	blank

  EEPROM.write (120,   73);	// I
  EEPROM.write (121,   78);	// N
  EEPROM.write (122,   80);	// P
  EEPROM.write (123,   85);	// U
  EEPROM.write (124,   84);	// T
  EEPROM.write (125,   32);	// 	blank
  EEPROM.write (126,   50);	// 2
  EEPROM.write (127,   32);	//	blank
  EEPROM.write (128,   32);	//	blank
  EEPROM.write (129,   32);	//	blank
  EEPROM.write (130,   32);	//  blank
  EEPROM.write (131,   32);	//	blank

  EEPROM.write (140,   73);	// I
  EEPROM.write (141,   78);	// N
  EEPROM.write (142,   80);	// P
  EEPROM.write (143,   85);	// U
  EEPROM.write (144,   84);	// T
  EEPROM.write (145,   32);	// 	blank
  EEPROM.write (146,   51);	// 3
  EEPROM.write (147,   32);	//	blank
  EEPROM.write (148,   32);	//	blank
  EEPROM.write (149,   32);	//	blank
  EEPROM.write (150,   32);	//  blank
  EEPROM.write (151,   32);	//	blank

  EEPROM.write (160,   73);	// I
  EEPROM.write (161,   78);	// N
  EEPROM.write (162,   80);	// P
  EEPROM.write (163,   85);	// U
  EEPROM.write (164,   84);	// T
  EEPROM.write (165,   32);	// 	blank
  EEPROM.write (166,   52);	// 4
  EEPROM.write (167,   32);	//	blank
  EEPROM.write (168,   32);	//	blank
  EEPROM.write (169,   32);	//	blank
  EEPROM.write (170,   32);	//  blank
  EEPROM.write (171,   32);	//	blank

  EEPROM.write (180,   73);	// I
  EEPROM.write (181,   78);	// N
  EEPROM.write (182,   80);	// P
  EEPROM.write (183,   85);	// U
  EEPROM.write (184,   84);	// T
  EEPROM.write (185,   32);	// 	blank
  EEPROM.write (186,   53);	// 5
  EEPROM.write (187,   32);	//	blank
  EEPROM.write (188,   32);	//	blank
  EEPROM.write (189,   32);	//	blank
  EEPROM.write (190,   32);	//  blank
  EEPROM.write (191,   32);	//	blank

  EEPROM.write (200,   73);	// I
  EEPROM.write (201,   78);	// N
  EEPROM.write (202,   80);	// P
  EEPROM.write (203,   85);	// U
  EEPROM.write (204,   84);	// T
  EEPROM.write (205,   32);	// 	blank
  EEPROM.write (206,   54);	// 6
  EEPROM.write (207,   32);	//	blank
  EEPROM.write (208,   32);	//	blank
  EEPROM.write (209,   32);	//	blank
  EEPROM.write (210,   32);	//  blank
  EEPROM.write (211,   32);	//	blank

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Please restart!");
  while (1)
  {
  }
}
