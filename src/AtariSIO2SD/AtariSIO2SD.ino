// --------- Program code for the IOS2SD atari disk drive emulator -----------
// This sketch support the device design using the Arduino Micro, as well as 
// the design built on a bare Atmega382p with 8Mhz (the miniature 1050 floppy drive)). 
// At compile time, the appropriate pins and settings will be used.

#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>
#include <EEPROM.h>


// all settings for the Arduino Micro based design
#if defined(__AVR_ATmega32U4__)
  #define SIO Serial1
  #define PRINT Serial.print
  #define PRINTLN Serial.println
  #define PRINTBEGIN Serial.begin
  #define PIN_CHIPSELECT  13  
  #define PIN_SIOCOMMAND  2
  #define PIN_DIGIT0 12
  #define PIN_DIGIT1 11
  byte pin_segments0[8] = {3,6,7,5,9,10,8,4};
  byte pin_segments1[8] = {3,7,4,10,8,5,6,9};
  byte pin_buttons[4] = {A0,A1,A2,A3};
  #define DIGITACTIVE     HIGH
  #define DIGITINACTIVE   LOW
  #define SEGMENTACTIVE   LOW
  #define SEGMENTINACTIVE HIGH   
// all settings for the miniature 1050
#else
  #define SIO Serial
  #define PRINT(...)        1
  #define PRINTLN(...)      1
  #define PRINTBEGIN(...);  1
  #define PIN_CHIPSELECT  10  
  #define PIN_SIOCOMMAND  2
  #define PIN_DIGIT0 A4
  #define PIN_DIGIT1 3
  byte pin_segments0[8] = {A1,5,6,4,A5,A0,A2,A3}; 
  byte pin_segments1[8] = {4,A2,A0,A3,A1,5,A5,6};  
  byte pin_buttons[4] = {9,9,7,8};  // only two connected, pin 9 is n.c.
  #define DIGITACTIVE     LOW
  #define DIGITINACTIVE   HIGH
  #define SEGMENTACTIVE   HIGH
  #define SEGMENTINACTIVE LOW   
#endif


// ------------------ DISK SELECTOR (LED DIGITS AND BUTTONS) -----------
// because the two digits are displayed in a multiplexted fasion (to reduce number of needed pins),
// they will be displayed alternately. a timer interrupt  comes handy to do this.

#define EEPROM_ADDRESS  47

byte prevbuttonstate[4];

volatile byte digit0value;  
volatile byte digit1value;  
volatile int activitylight;
int valuesavedelay;
int savelight;
byte activedigit; 

// various digits              
bool segmentsfordigits[7][10] = {
    // 0      1      2      3      4      5      6      7      8     9
  { true,  false, true,  true,  false, true,  true,  true,  true, true  },
  { false, false, true,  true,  true,  true,  true,  false, true, true  },
  { true,  false, true,  true,  false, true,  true,  false, true, true  },
  { true,  false, false, false, true,  true,  true,  false, true, true  },
  { true,  false, true,  false, false, false, true,  false, true, false },
  { true,  true,  true,  true,  true,  false, false, true,  true, true  },
  { true,  true,  false, true,  true,  true,  true,  true,  true, true  }
};

void initdiskselector()
{
  byte i;

  // init input buttons
  for (i=0; i<4; i++)
  {   pinMode(pin_buttons[i], INPUT_PULLUP);
      prevbuttonstate[i] = HIGH;
  }
  
  // read selector value at startup
  byte s = EEPROM.read(EEPROM_ADDRESS);
  if (s>99) s=0;
  
  // init visible digit values and toggle register
  digit0value = s % 10;
  digit1value = s / 10;
  activedigit = 0;
  valuesavedelay = 0;
  savelight = 0;
  activitylight = 0;
  
  // configure and turn of digit selectors 
  pinMode(PIN_DIGIT0, OUTPUT);
  digitalWrite (PIN_DIGIT0, DIGITINACTIVE);
  pinMode(PIN_DIGIT1, OUTPUT);
  digitalWrite (PIN_DIGIT1, DIGITINACTIVE);  
  // configure and turn off individual digits
  for (i=0; i<8; i++)
  {    
    pinMode(pin_segments0[i], OUTPUT);
    digitalWrite (pin_segments0[i], SEGMENTINACTIVE);
  }
  
  // start timer
  Timer1.initialize(5000);     // call with 200 Hz   
  Timer1.attachInterrupt(polldiskselector);    
}


int getdiskselectorvalue()
{
  return digit0value + digit1value*10;  
}

int setactivitylight()
{
    activitylight = 10;  
}

void polldiskselector()   // interrupt service routine (must not take much time)
{
  // --- query button changes ---
  byte i;
  for (i=0; i<4; i++)
  {
      byte s = digitalRead(pin_buttons[i]);
      if (s!=prevbuttonstate[i])
      {
        prevbuttonstate[i]=s;
        if (s==LOW)
        {   
            // modify digits without needing to do divisions (must be fast)
            switch (i)
            {  case 0: digit1value = digit1value<9 ? digit1value+1 : 0;
                       break;
               case 1: digit1value = digit1value>0 ? digit1value-1 : 9;
                       break;
               case 2: digit0value = digit0value<9 ? digit0value+1 : 0;
                       if (digit0value==0) digit1value = digit1value<9 ? digit1value+1 : 0;
                       break;
               case 3: digit0value = digit0value>0 ? digit0value-1 : 9;
                       if (digit0value==9) digit1value = digit1value>0 ? digit1value-1 : 9;
                       break;
            }
            valuesavedelay = 600; // 3 seconds before save
        }
      }
  } 
  
  // --- handling of delay before saving the current selected value
  if (valuesavedelay>0)
  {
    valuesavedelay --;
    if (valuesavedelay==0)
    {
      EEPROM.update(EEPROM_ADDRESS, digit0value + digit1value*10);
      savelight = 200;
    }
  }

  // --- count down activity light duration
  if (activitylight>0)
  {
     activitylight--;
  }

  // --- count down save light duration
  if (savelight>0)
  {
     savelight--;
  }
  
  // --- display digits alternatingly ----
  // turn off previously active digit
  digitalWrite (PIN_DIGIT0, DIGITINACTIVE);
  digitalWrite (PIN_DIGIT1, DIGITINACTIVE);
  
  // switch to other digit
  activedigit = 1-activedigit;
  
  // turn on/off digit segments as needed (pin assignment is very arbitrary)
  if (activedigit==0) {
    byte v = digit0value;
    for (i=0; i<7; i++) {
       digitalWrite (pin_segments0[i], segmentsfordigits[i][v] ? SEGMENTACTIVE : SEGMENTINACTIVE);
    }
    digitalWrite (pin_segments0[7], activitylight>0 ? SEGMENTACTIVE : SEGMENTINACTIVE);   // the DOT is used to show activity 
    digitalWrite (PIN_DIGIT0, DIGITACTIVE);  // turn on digit 0
  } else {
    byte v = digit1value;
    for (i=0; i<7; i++) {
       digitalWrite (pin_segments1[i], segmentsfordigits[i][v] ? SEGMENTACTIVE : SEGMENTINACTIVE);
    }
     digitalWrite (pin_segments1[7], savelight>0 ? SEGMENTACTIVE : SEGMENTINACTIVE);   // the DOT is used to show settings saving 
     digitalWrite (PIN_DIGIT1, DIGITACTIVE);  // turn on digit 1
  }  
}


// ---------------------- DISK FILE HANDLING  -----------------------

bool didinitsd = false;

File diskfile;  
unsigned int disksize;         // size in sectors

void opendiskfile(byte drive, int index)
{  
  // check if desired file is already open - continue to use it
  if (diskfile && isrequesteddiskfile(diskfile,drive,index))
  {    
    return;
  } 
  
  // before switching disk file, close previous one if still open
  if (diskfile )
  {
    diskfile.close();
  }
  
  // if not done, try to initialize the SD subsystem
  if (!didinitsd)
  {  
      pinMode(PIN_CHIPSELECT,OUTPUT);
      digitalWrite(PIN_CHIPSELECT,HIGH);
      if (!SD.begin(PIN_CHIPSELECT)) {
        PRINTLN("SDCard failed, or not present");
        return;
      }
      didinitsd = true;
  }
        
  // try to scan the files in the ATARI directory on the SDCARD and 
  // locate the file name beginning with the correct index (2 digits).
  char fullname[100];
  fullname[0] = '\0';
  
  File root = SD.open(F("ATARI"));
  if (!root || !root.isDirectory())
  { PRINTLN(F("Can not locate ATARI/ directory"));
    return;             
  }  
  while (fullname[0]=='\0')
  { File entry = root.openNextFile();
    if (!entry)
    { PRINT(F("Could not find file for disk "));
      PRINT(index);
      PRINT(F(" in drive "));
      PRINTLN(drive);
      root.close();
      return;
    }
    // check if name matches
    if (isrequesteddiskfile(entry,drive,index))
    { // fill data into the fullname variable (which terminates the loop)
      strcat (fullname, "ATARI/");
      strcat (fullname, entry.name());
    }
    entry.close();
  }      
  root.close();
  
  // try to open the file in read-write mode, and if this does not
  // succeed in read-only mode
  PRINT(F("Trying to open "));
  PRINTLN(fullname);      

  bool readonly = false;
  diskfile = SD.open(fullname, FILE_WRITE);
  if (diskfile)
  {  diskfile.seek(0);
  }
  else
  { diskfile = SD.open(fullname, FILE_READ);
    readonly = true;
  }  
  // abort if not possible to open
  if (!diskfile)
  { PRINT(F("Can not open disk file"));
    PRINTLN(fullname);
    return;
  }       
  // read the header
  byte header[16];
  int didread = diskfile.readBytes(header,16);
  if (didread!=16)
  { PRINTLN(F("Can not read file header"));
    diskfile.close();
    return;
  }
  // check for signature and other settings
  if (header[0]!=0x96 || header[1]!=0x02)
  { PRINTLN(F("Magic number not present"));
    diskfile.close();
    return;
  }
  if (header[4]!=0x80 || header[5]!=0x00)
  { PRINTLN(F("Can only handle 128 byte sectors"));
    diskfile.close();     
    return;
  }

  unsigned long paragraphs = header[6];
  paragraphs = (paragraphs<<8) | header[3];
  paragraphs = (paragraphs<<8) | header[2];
  if (paragraphs<8 || paragraphs>0x70000)
  { PRINTLN(F("Disk file size out of range."));
    diskfile.close();
    return;
  }
  
  // seems to be a valid disk file - use it
  disksize = paragraphs >> 3;
       
  PRINT(F("SIZE: "));
  PRINT(disksize);
  PRINT(F(" sectors of 128 bytes"));
  if (readonly) PRINTLN(F("(read only)"));
  else          PRINTLN(F(" (writeable)"));
}

bool isrequesteddiskfile(File f, byte drive, int index)
{
  char* n = f.name();
  if (n[0]>='0' && n[0]<='9' && n[1]>='0' && n[1]<='9' && ((n[0]-'0')*10 + n[1]-'0')==index) {
    if (n[2]=='_' && drive==0) return true;
    if (n[2]>='B' && n[2]<='H' && n[2]-'A'==drive) return true;
  }
  return false;
}


bool readsector(unsigned int sector, byte* data)
{
  if (!diskfile || sector>=disksize)
  {  return false;
  }
  if (!diskfile.seek(16+((unsigned long)sector)*128))
  {  return false;
  }
  return diskfile.readBytes(data, 128) == 128;
}

bool writesector(unsigned int sector, byte* data)
{
  if (!diskfile || sector>=disksize)
  {  return false;
  }
  if (!diskfile.seek(16+((unsigned long)sector)*128))
  {  return false;
  }
  return diskfile.write(data, 128) == 128;
}

bool diskavailable()
{
    if (diskfile)
    {  return true;
    }
    return false;
}


// ----------------- SIO PROTOCOL -------------------

void sendwithchecksum(byte* data, int len)
{
  int sum = 0;
  int i;
  for (i=0; i<len; i++)
  {  sum += data[i];
     if (sum>=256)
     {  sum = (sum - 256) + 1;  // add carry into sum
     }  
  }
  SIO.write(data,len);
  SIO.write(sum);
}

byte receivebyte()
{
  int d = SIO.read();
  while (d<0)
  {  d = SIO.read();
  }
  return d;
}

bool receivewithchecksum(byte* data, int len)
{
  int sum = 0;
  int i;
  for (i=0; i<len; i++)
  {  data[i] = receivebyte();
     sum += data[i];
     if (sum>=256)
     {  sum = (sum - 256) + 1;  // add carry into sum
     }  
  }
  byte check = receivebyte();
  return check==sum;    
}

void logcommand(byte* command)
{
    int i;
      PRINT(F("CMD: "));
      for (i=0; i<5; i++)
      {  
          PRINT(command[i], HEX);
          PRINT(F(" "));
      }
      PRINT(F("]"));
}      

void logdata(byte* data, int length)
{
    int i;
      PRINT(F("DATA: "));
      for (i=0; i<length; i++)
      {  
          PRINT(data[i], HEX);
          PRINT(" ");
      }
      PRINTLN();  
}

void handlecommand_status(byte drive)
{
  opendiskfile(drive, getdiskselectorvalue());

  byte status[4];
  if (diskavailable())
  {  status[0] = 0x10;  // status flags byte 0  (motor=on, single density)
     status[1] = 0x00;  // status flags byte 1
     status[2] = 0xe0;  // format timeout 
     status[3] = 0x00;  // copy of internal registers  
  }
  else
  {  status[0] = 0x00;  // status flags byte 0  (motor=off)
     status[1] = 0x80;  // status flags byte 1  (not ready)
     status[2] = 0xe0;  // format timeout 
     status[3] = 0x00;  // copy of internal registers  
  }
  SIO.write('C');  
  sendwithchecksum(status,4);
}

void handlecommand_read(byte drive, unsigned int sector)
{
   setactivitylight();
  opendiskfile(drive, getdiskselectorvalue());
 
  byte data[128];
  if (!readsector(sector,data))
  {
    SIO.write('E');
    return;
  }      
  
  SIO.write('C');
  sendwithchecksum(data,128);
}

void handlecommand_write(byte drive, unsigned int sector)
{ 
   byte data[128];
   if (!receivewithchecksum(data,128))
   {
     PRINTLN(F("Received sector with invalid checksum"));
     SIO.write('E');
     return;
   } 

   delay(1);
   SIO.write('A');
   
   setactivitylight();
   opendiskfile(drive, getdiskselectorvalue());
 
  if (!writesector(sector,data))
  {
    PRINTLN(F("Writing sector to SD card failed"));
    SIO.write('E');
    return;
  }      
  
  delay(1);      
  SIO.write('C');
}


void handle_sio()
{
  int i;

    // must wait until the command line goes low 
    while (digitalRead(PIN_SIOCOMMAND)!=LOW)
    {
      int r = SIO.read();  // discard any data garbage that may have arrived
      if (r>=0)
      {  
        PRINT(F("Received while CMD not active: "));
        PRINTLN(r,HEX);
      }
    }
    // read the command - but abort when command line goes high prematurely
    byte command[5];
    int commandlength=0;
    while (commandlength<5 && digitalRead(PIN_SIOCOMMAND)==LOW)
    {  
      int r = SIO.read();
      if (r>=0)
      {
        command[commandlength] = (byte) r;
        commandlength++;
      }
    }
    if (commandlength<5)
    {  PRINT(F("CMD went inactive before receiving full command"));
       return;
    }
    // waiting for command line to go high again, so it is allowed so send
    while (digitalRead(PIN_SIOCOMMAND)!=HIGH)
    { 
      int r = SIO.read();
      if (r>=0)
      {
        PRINT(F("Received more data before CMD went inactive: "));
        PRINTLN(r,HEX);
      }
    }
        
    // check if command has correct checksum
    int sum = 0;
    for (i=0; i<4; i++)
    {   sum += command[i];
        if (sum>=256)
        {  sum = (sum - 256) + 1;  // add carry into sum
        }
    }
    if (sum!=command[4])
    {
      PRINT(F("Ignored command with invalid checksum"));
      return;
    }
    // when the command is not inteded for a floppy device, ignore it
    if (command[0]<0x31 || command[0]>0x39)
    {              
      PRINT(F("Received command for different device "));
      PRINTLN(command[0], HEX);
      return;       
    }
    
    switch (command[1])
    {
        case 0x53:  // STATUS
          logcommand(command);
          SIO.write('A');
          delay(1);
          handlecommand_status(command[0]-0x31);
          break;
        case 0x52:  // READ
          logcommand(command);
          SIO.write('A');
          delay(1);
          handlecommand_read(command[0]-0x31, ((unsigned int)command[2]) + (((unsigned int)command[3])<<8) - 1);
          break;
        case 0x57:   // WRITE WITH VERIFY
          logcommand(command);
          SIO.write('A');
          handlecommand_write(command[0]-0x31, ((unsigned int)command[2]) + (((unsigned int)command[3])<<8) - 1);
          break;
        default:
          SIO.write('N');
          logcommand(command);
          break;
    }
}


// ---------------- MAIN PROGRAM --------------------
void setup() {
  // start serial monitor for debugging
  PRINTBEGIN(9600);

  // configure connection to the SIO interface
  pinMode(PIN_SIOCOMMAND,INPUT);
  SIO.begin(19200, SERIAL_8N1);  
  
  // start displaying digits  
  initdiskselector();
}

void loop()
{
  handle_sio();
}




