// --------- Program code for the IOS2SD Atari 1050 disk drive emulator -----------
// The program should be compatible with most arduino boards, as well
// as with a bare-bone atmega mcu.
// On boards with the ATmega32U4, logging via the usb uplink is enabled,
// while the the serial communication to the SIO interface 
// is routed over the dedicated serial pins. 

#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>
#include <EEPROM.h>

// serial port RX/TX to Atari is either Serial or Serial1, d
// and if available, use USB line for logging
#if defined(__AVR_ATmega32U4__)
    #define SIO Serial1
    #define PRINT Serial.print
    #define PRINTLN Serial.println
    #define PRINTBEGIN Serial.begin
#else
    #define SIO Serial
    #define PRINT(...)        1
    #define PRINTLN(...)      1
    #define PRINTBEGIN(...)   1
#endif

// ------------------------ the wiring up of all components ------------------------
// -- connection to Atari SIO port
                              // default arduino RX -> serial data from Atari SIO pin 5
                              // default arduino TX -> serial data to Atari SIO pin 3
#define PIN_SIOCOMMAND  2     //                    -> CMD signal from Atari SIO pin 7

// -- connection to SDCARD
                              // default MOSI
                              // default MSIO
                              // default SCK
#define PIN_CHIPSELECT  10    // enables SDCARD on SPI bus

// -- connection to LED display and mode selector
#define PIN_DIGIT0 A4         // enables LED digit 0 (right digit)
#define PIN_DIGIT1 3          // enables LED digit 1 (left digit)
// the digit segments are differently assigned for digit 0 and 1  
//           segment:    a   b   c   d   e   f   g   DP
byte pin_segments0[8] = {A1, A0, A2, 6,  A5, 4,  5,  A3}; 
byte pin_segments1[8] = {4,  5,  A5, A0, A1, A3, A2, 6};  
#define COMMON_ANODE_SELECTOR 9  // pull this input low to select common annode LED 

// -- connection to input buttons
#define PIN_BUTTON0  7      // up button   (connects to GND)
#define PIN_BUTTON1  8      // down button (connects to GND)


// ------------------ DISK SELECTOR (LED DIGITS AND BUTTONS) -----------
// because the two digits are displayed in a multiplexted fashion (to reduce number of needed pins),
// they will be displayed alternately. a timer interrupt  comes handy to do this.

volatile byte selecteddisk[2];
volatile int activitylight[2];
#define ACTIVITY_LIGHT_DURATION 10

byte activedigit; 
byte showndiskdrive;
int valuesavedelay;

byte prevbuttonstate;
byte timesincebuttonchange;
bool inputlock;
bool showerror;

// various digits             gfedcba
byte digitpatterns[13] = 
{   B00111111,   // 0
    B00000110,   // 1
    B01011011,   // 2
    B01001111,   // 3
    B01100110,   // 4
    B01101101,   // 5
    B01111101,   // 6
    B00000111,   // 7
    B01111111,   // 8
    B01101111,   // 9
    B01000000,   // -
    B01111001,   // E
    B01010000,   // r
};

void initdiskselector(bool error)
{
    byte i;

    // init input buttons
    pinMode(PIN_BUTTON0, INPUT_PULLUP);
    pinMode(PIN_BUTTON1, INPUT_PULLUP);
    pinMode(COMMON_ANODE_SELECTOR, INPUT_PULLUP);
    
    // read selector values at startup
    selecteddisk[0] = read_eeprom(0);
    selecteddisk[1] = read_eeprom(1);
  
    // init various UI flags
    activitylight[0] = 0;
    activitylight[1] = 0;
    activedigit = 0;
    showndiskdrive = 0;
    valuesavedelay = 0;
    prevbuttonstate = 0;
    timesincebuttonchange = 0;
    inputlock = false;
  
    // configure and turn of digit selectors 
    pinMode(PIN_DIGIT0, OUTPUT);
    digitalWrite (PIN_DIGIT0, LOW);
    pinMode(PIN_DIGIT1, OUTPUT);
    digitalWrite (PIN_DIGIT1, LOW);  
    // configure and turn off individual digits
    for (i=0; i<8; i++)
    {   pinMode(pin_segments0[i], OUTPUT);
        digitalWrite (pin_segments0[i], LOW);
        pinMode(pin_segments1[i], OUTPUT);
        digitalWrite (pin_segments1[i], LOW);
    }  
    // start timer
    Timer1.initialize(5000);     // call with 200 Hz   
    Timer1.attachInterrupt(polldiskselector);    

    showerror = error;
}



// interrupt service routine is called 200 times per second 
// (must not take much time)
// this handles user input and also drives the LED digits
void polldiskselector()   
{
    // poll buttons only 100 times per second to reduce bumping
    if (activedigit==0 && !showerror) 
    {   byte buttonstate = 0;
    
        // --- query current button states and detect changes ---
        if (digitalRead(PIN_BUTTON0)==LOW) { buttonstate =  buttonstate | 1; }
        if (digitalRead(PIN_BUTTON1)==LOW) { buttonstate =  buttonstate | 2; }
    
        // -- detect a press of both buttons: switch shown disk drive 
        if ((buttonstate&3)==3 && (prevbuttonstate&3)!=3 && !inputlock)
        {   showndiskdrive = 1-showndiskdrive;
            inputlock = true;
        }
        if (prevbuttonstate==0 && buttonstate==0) { inputlock=false; }
    
        // -- detect clicks of single up and down buttons and provide key repeat function
        if (!inputlock)
        {   if ( ((buttonstate&1)==0 && (prevbuttonstate&1)!=0) || ((buttonstate&1)!=0 && timesincebuttonchange>=100) ) 
            {   selecteddisk[showndiskdrive] ++;
                if (selecteddisk[showndiskdrive]>100) selecteddisk[showndiskdrive]=0;  
                valuesavedelay = 100;    
            }
            if ( ((buttonstate&2)==0 && (prevbuttonstate&2)!=0) || ((buttonstate&2)!=0 && timesincebuttonchange>=100) ) 
            {   selecteddisk[showndiskdrive] --;
                if (selecteddisk[showndiskdrive]>100) selecteddisk[showndiskdrive]=100;      
                valuesavedelay = 100;    
            }
        }    
    
        // -- memorize current button state for next poll to then detect edges
        if (buttonstate!=prevbuttonstate)
        {   prevbuttonstate = buttonstate;
            timesincebuttonchange=0;
        }          
        else
        {   if (timesincebuttonchange<100) { timesincebuttonchange++; }
            else                           { timesincebuttonchange-=5; }
        }
        
        // --- handling of delay before saving the current selected value
        if (valuesavedelay>0 && buttonstate==0)
        {   valuesavedelay --;
            if (valuesavedelay==0)
            {   store_eeprom(0, selecteddisk[0]);
                store_eeprom(1, selecteddisk[1]);
            }
        }
    }
    

    // --- count down activity light duration
    if (activitylight[0]>0) {   activitylight[0]--;  }
    if (activitylight[1]>0) {   activitylight[1]--;  }
  
    // --- display digits alternatingly ----
    // switch to other digit
    activedigit = 1-activedigit;

    // determine if need to drive a common cathode or common anode LED
    byte commoncathode = digitalRead(COMMON_ANODE_SELECTOR);
  
    // turn on/off digit segments as needed (pin assignment is quite arbitrary)
    if (activedigit==0) 
    {   byte value = selecteddisk[showndiskdrive];
        value = showerror ? 12 : ((value<=99) ? (value % 10) : 10);   // when drive is off: - 
        digitalWrite (PIN_DIGIT1, commoncathode ? HIGH : LOW);  // turn off digit 1
        byte i;
        byte v = digitpatterns[value];
        for (i=0; i<7; i++) 
        {   if (v&(1<<i)) { digitalWrite (pin_segments0[i], commoncathode ? HIGH : LOW); }
            else          { digitalWrite (pin_segments0[i], commoncathode ? LOW : HIGH); }
        }
        // this dot is used to see if drive 0 is visible and if activity on disk 0
        bool dot = showndiskdrive==0 && !showerror;
        if (activitylight[0]>0) { dot = !dot; }
        if (dot) { digitalWrite (pin_segments0[7], commoncathode ? HIGH : LOW); }
        else     { digitalWrite (pin_segments0[7], commoncathode ? LOW : HIGH); }
        digitalWrite (PIN_DIGIT0, commoncathode ? LOW : HIGH);  // turn on digit 0
    }
    else 
    {   byte value = selecteddisk[showndiskdrive];
        value = showerror ? 11 : ((value<=99) ? (value / 10) : 10);   // when drive is off: -
        digitalWrite (PIN_DIGIT0, commoncathode ? HIGH  : LOW);   // turn off digit 0
        byte i;
        byte v = digitpatterns[value];
        for (i=0; i<7; i++) 
        {   if (v&(1<<i)) { digitalWrite (pin_segments1[i], commoncathode ? HIGH : LOW); }
            else          { digitalWrite (pin_segments1[i], commoncathode ? LOW : HIGH); }
        }
        // this dot is used to see if drive 1 is visible and if activity on disk 1
        bool dot = showndiskdrive==1 && !showerror;
        if (activitylight[1]>0) { dot = !dot; }
        if (dot) { digitalWrite (pin_segments1[7], commoncathode ? HIGH : LOW); } 
        else     { digitalWrite (pin_segments1[7], commoncathode ? LOW : HIGH); } 
        digitalWrite (PIN_DIGIT1, commoncathode ? LOW : HIGH);  // turn on digit 1
    }  
}

// ------------------ EEPROM storage with wear-levelling -----------
// To prevent frequent writing to a single EEPROM cell to wear out this particual storage,
// the writes are distributed over a wide range of the available EEPROM. This is
// possible because only a very tiny amount of data needs to be stored int each slot
// (a 7-bit value).
// It works as follows: A splot consists of 256 bytes EEPROM storage. The one active
// data value (7 bits) can be found on the first byte position that has a different most significant
// bit (MSB) value than all previous bytes. In the case that all bytes in the slot have the
// same MSB, the very first byte is taken.
// Writing a new value into the slot will be done by overwriting the byte just before
// the previous value-carrying byte with the proper MSB set. 
// By this only one byte is overwritten in each write operation and it will always be a
// different one in the slot.

byte read_eeprom(byte slot)
{
    // find the first different MSB and take the value from there
    int start = ((int)slot) * 256;
    byte b0 = EEPROM.read(start);
    int i;
    for (i=start+1; i<start+256; i++)
    {   byte b1 = EEPROM.read(i);
        if ( (b0&0x80) != (b1&0x80)) { return b1 & 0x7f; }
        b0 = b1;
    }
    return EEPROM.read(start) & 0x7f;
}

void store_eeprom(byte slot, byte value)
{
    // make sure to not store unnecessarily 
    if (read_eeprom(slot)==value) { return; }

    // find the first different MSB 
    int start = ((int)slot) * 256;
    byte b0 = EEPROM.read(start);
    int i;
    for (i=start+1; i<start+256; i++)
    {   byte b1 = EEPROM.read(i);
        // if found a different MSB, store the value just in the place before
        if ( (b0&0x80) != (b1&0x80)) 
        {   EEPROM.write(i-1, (value&0x7f) | (b1&0x80));
            PRINT(F("Stored "));
            PRINT(value);
            PRINT(F(" to EEPROM byte "));
            PRINTLN(i-1);
            // when having written to location 0 make sure to have no other MSB edges 
            if (i==1) { cleanmsb_eeprom(slot); }
            return;
        }
        b0 = b1;
    }
    // if no suitable place could be found (all MSB equal, start at the end)
    EEPROM.write(start+255, (value&0x7f) | ((~b0)&0x80)); 
}

void cleanmsb_eeprom(byte slot)
{
    int start = ((int)slot) * 256;
    byte b0 = EEPROM.read(start);
    int i;
    for (i=start+1; i<start+256; i++)
    {   byte b1 = EEPROM.read(i);
        if ( (b0&0x80)!=(b1&0x80) )
        {   EEPROM.write(i, (b0&0x80)|(b1&0x7f)); 
            PRINT(F("Cleaned EEPROM byte "));
            PRINTLN(i);            
        }
    }
}



// ---------------------- DISK FILE HANDLING  -----------------------


File diskfile;                 // can be a .ATR file or a directory
unsigned int disksize;         // size in sectors

File atarifile;                // currently used file of a directory (if used in this mode)
unsigned int firstfilesector;  // first sector (numbered in "usable sectors) of the file
byte directoryindexoffile;     // to which directory entry the current file belongs

boolean initsdcard()
{
    pinMode(PIN_CHIPSELECT,OUTPUT);
    digitalWrite(PIN_CHIPSELECT,HIGH);
    if (!SD.begin(PIN_CHIPSELECT)) 
    {   PRINTLN(F("SDCard failed, or not present"));
        return false;
    }      
}

void opendiskfile(int index)
{  
    // check if desired file is already open - continue to use it
    if (diskfile && isrequesteddiskfile(diskfile,index)) { return; } 
  
    // before switching disk file, close any previous open
    if (diskfile) { diskfile.close(); }
    if (atarifile) { atarifile.close(); }
  
    // try to scan the files in the ATARI directory on the SDCARD and 
    // locate the file name beginning with the correct index (2 digits).
    char fullname[60];
    fullname[0] = '\0';
  
    File root = SD.open(F("ATARI"));
    if (!root || !root.isDirectory())
    {   PRINTLN(F("Can not locate ATARI/ directory"));      
        return;             
    }  
    while (fullname[0]=='\0')
    {   File entry = root.openNextFile();
        if (!entry)
        {   PRINT(F("Could not find file for disk "));
            PRINTLN(index);
            root.close();
            return;
        }
        // check if name matches
        if (isrequesteddiskfile(entry,index))
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
    {   diskfile.seek(0);
    }
    else
    {   diskfile = SD.open(fullname, FILE_READ);
        readonly = true;
    }  
    // abort if not possible to open
    if (!diskfile)
    {   PRINT(F("Can not open disk file"));
        PRINTLN(fullname);
        return;
    }       
    
    // check if the file is a directory instead of a diskfile 
    if (diskfile.isDirectory())
    {   PRINT(F("Using directory content to emulate a 1040 sector floppy: "));
        PRINTLN(fullname);
        disksize = 1040;
        return;
    }
    
    // read the header
    byte header[16];
    int didread = diskfile.read(header,16);
    if (didread!=16)
    {   PRINTLN(F("Can not read file header"));
        diskfile.close();
        return;
    }
    // check for signature and other settings
    if (header[0]!=0x96 || header[1]!=0x02)
    {   PRINTLN(F("Magic number not present"));
        diskfile.close();
        return;
    }
    if (header[5]!=0 or header[4]!=128)
    {   PRINTLN(F("Can only handle 128 byte sectors"));
        diskfile.close();     
        return;
     }

    unsigned long paragraphs = header[6];
    paragraphs = (paragraphs<<8) | header[3];
    paragraphs = (paragraphs<<8) | header[2];
    if (paragraphs<8 || paragraphs>0x70000)
    {   PRINTLN(F("Disk file size out of range."));
        diskfile.close();
        return;
    }
  
    // seems to be a valid diskfile - use it
    disksize = (paragraphs) >> 3;
    
    PRINT(F("SIZE: "));
    PRINT(disksize);
    PRINT(F(" sectors of 128 bytes"));
    if (readonly) { PRINTLN(F(" (read only)")); }
    else          { PRINTLN(F(" (writeable)")); }
}

bool isrequesteddiskfile(File f, int index)
{
    char* n = f.name();
    if (n[0]>='0' && n[0]<='9' && n[1]>='0' && n[1]<='9' && ((n[0]-'0')*10 + n[1]-'0')==index && n[2]=='_')
    {   return true; 
    }
    return false;
}

bool readsector(unsigned int sector, byte* data)
{   
    if (!diskfile || sector<1 || sector>disksize) { return false; }
    if (diskfile.isDirectory()) 
    {   if (!readsectorfromdirectory(sector,data)) { return false; }
        return true;
    }    
    if (!diskfile.seek(16+((unsigned long)(sector-1))*128) ) { return false; }
    return diskfile.read(data, 128) == 128;
}

bool writesector(unsigned int sector, byte* data)
{
    if (!diskfile || diskfile.isDirectory() || sector<1 || sector>disksize) { return false; }
    if (!diskfile.seek(16+((unsigned long)(sector-1))*128)) { return false; }
    return diskfile.write(data, 128) == 128;
}

bool diskavailable()
{
    if (diskfile) { return true; }
    else          { return false; }
}

bool creatediskfile(int index, int sectors)
{
    char fullname[101];
    fullname[0] = 0;
    strcat (fullname, "ATARI/");    
    
    opendiskfile(index);    
    // if possible reuse the already available disk file name (and remove old file first)
    if (diskfile)
    {   if (diskfile.isDirectory()) 
        {   PRINTLN(F("Can not format directory"));
            return false;
        }
        strcat (fullname, diskfile.name());
        diskfile.close();
        if (!SD.remove(fullname)) {
            PRINT(F("Can not remove old disk file "));
            PRINTLN(fullname);
            return false;
        }
    }
    // otherwise invent a new disk file name
    else
    {   strcat(fullname, "00_disk.atr");
        fullname[6] = '0' + (index / 10);
        fullname[7] = '0' + (index % 10);
    }

    PRINT(F("Creating disk file "));
    PRINT(fullname);
    PRINT(F(" with "));
    PRINT(sectors);
    PRINTLN(F(" sectors"));
    diskfile = SD.open(fullname, FILE_WRITE);
    if (!diskfile) { return false; }
    
    byte chunk[16];
    // construct header: magic number
    chunk[0] = (byte) 0x96;
    chunk[1] = (byte) 0x02;
    // paragraphs
    int paragraphs = sectors * 8; 
    chunk[2] = (byte) (paragraphs & 0xff);
    chunk[3] = (byte) ((paragraphs>>8) & 0xff);
    // sector size
    chunk[4] = (byte)128;
    chunk[5] = (byte)0;
    // 10 byte unused
    int i=0;
    for (i=6; i<16; i++) { chunk[i] = 0; }
    diskfile.write(chunk, 16);
    
    // create rest of file
    for (i=0; i<16; i++) { chunk[i] = 0x00; }
    for (i=0; i<paragraphs; i++)
    {   diskfile.write(chunk, 16);
    }

    diskfile.close();
    PRINTLN(F("Finished creating disk file"));
    return true;
}

const byte bootblocks[] PROGMEM = {
    0x00,0x03,0x00,0x07,0x40,0x15,0x4c,0x14,0x07,0x03,0x83,0x00,0xcc,0x19,0x01,0x04,
    0x00,0x7d,0xcb,0x07,0xac,0x0e,0x07,0xf0,0x35,0x20,0x5f,0x07,0xad,0x10,0x07,0xac,
    0x0f,0x07,0xa6,0x24,0x8e,0x04,0x03,0xa6,0x25,0x8e,0x05,0x03,0x18,0x20,0x6c,0x07,
    0x30,0x1c,0xac,0x11,0x07,0xb1,0x24,0x29,0x03,0xaa,0xc8,0x11,0x24,0xf0,0x11,0xb1,
    0x24,0x48,0xc8,0xb1,0x24,0x20,0x55,0x07,0x68,0xa8,0x8a,0x4c,0x22,0x07,0xa9,0xc0,
    0x0a,0xa8,0x60,0xa9,0x80,0x18,0x65,0x24,0x85,0x24,0x90,0x02,0xe6,0x25,0x60,0xad,
    0x12,0x07,0x85,0x24,0xad,0x13,0x07,0x85,0x25,0x60,0x00,0x00,0x8d,0x0b,0x03,0x8c,
    0x0a,0x03,0xa9,0x52,0xa0,0x40,0x90,0x04,0xa9,0x57,0xa0,0x80,0x08,0xa6,0x21,0xe0,
    0x08,0xd0,0x07,0x28,0x20,0x81,0x14,0x4c,0xb9,0x07,0x28,0x8d,0x02,0x03,0xa9,0x0f,
    0x8d,0x06,0x03,0x8c,0x17,0x13,0xa9,0x31,0x8d,0x00,0x03,0xa9,0x03,0x8d,0x09,0x13,
    0xa9,0x80,0x8d,0x08,0x03,0x0a,0x8d,0x09,0x03,0xad,0x17,0x13,0x8d,0x03,0x03,0x20,
    0x59,0xe4,0x10,0x05,0xce,0x09,0x13,0x10,0xf0,0xa6,0x49,0x98,0x60,0x20,0xad,0x11,
    0x20,0x64,0x0f,0x20,0x04,0x0d,0x4c,0xc7,0x12,0x00,0x00,0x64,0x08,0x8f,0x0a,0x4d,
    0x0a,0x8f,0x09,0xbc,0x07,0x2a,0x0b,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01,0xff,
    0xad,0x0c,0x07,0x85,0x24,0xad,0x0d,0x07,0x85,0x25,0xad,0x0a,0x07,0x85,0x43,0xa2,
    0x07,0xa9,0x00,0x06,0x43,0x90,0x15,0xa0,0x05,0x91,0x24,0xa5,0x24,0x9d,0x29,0x13,
    0xa5,0x25,0x9d,0x31,0x13,0xa9,0x90,0x20,0x55,0x07,0xa9,0x64,0x9d,0x19,0x13,0xca,
    0x10,0xdf,0xa5,0x24,0x8d,0x39,0x13,0xa5,0x25,0x8d,0x3a,0x13,0xac,0x09,0x07,0xa2,
    0x00,0x88,0x98,0x9d,0x21,0x13,0x30,0x03,0x20,0x53,0x07,0xe8,0xe0,0x08,0xd0,0xf1,
    0xa5,0x24,0x8d,0xe7,0x02,0xa5,0x25,0x8d,0xe8,0x02,0xa9,0x00,0xa8,0x99,0x81,0x13,
    0xc8,0x10,0xfa,0xa8,0xb9,0x1a,0x03,0xf0,0x0c,0xc9,0x44,0xf0,0x08,0xc8,0xc8,0xc8,
    0xc0,0x1e,0xd0,0xf0,0x00,0xa9,0x44,0x99,0x1a,0x03,0xa9,0xcb,0x99,0x1b,0x03,0xa9,
    0x07,0x99,0x1c,0x03,0x60,0x20,0xad,0x11,0x20,0x7d,0x0e,0xbd,0x4a,0x03,0x9d,0x82,
    0x13,0x29,0x02,0xf0,0x03,0x4c,0x72,0x0d,0x20,0xec,0x0e,0x08,0xbd,0x82,0x13,0xc9      
};
// construct sectors to emulate a 1040 sector disk using data from a directory
bool readsectorfromdirectory(unsigned int sector, byte* data)
{
  int i;
  for (int i=0; i<128; i++) { data[i] = 0; };
  
  // boot blocks
  if (sector>=1 && sector<=3)
  {   for (i=0; i<128; i++)
      {   data[i] = pgm_read_byte_near(bootblocks+(sector-1)*128+i);
      }
      return true;
  }
  // VTOC (simulate completely full disk)
  if (sector==360)
  {   data[0] = 2;
      data[1] = 707 & 0xff;;
      data[2] = (707>>8) & 0xff;
      return true;
  }
  // VTOC2 (simulate completely full disk)
  if (sector==1024)
  {   return true;
  }
  // directory sectors (use data from actual SD card directory
  if (sector>=361 && sector<=368) 
  {   unsigned long startsector = 4;
      int fidx;
      for (fidx=0; fidx<64; fidx++)
      {   trynextatarifile(fidx);
          if (!atarifile) { break; }
          if (sector-361 == fidx/8)
          {   unsigned int sizeinsectors = (atarifile.size()+124) / 125;
              unsigned int endsector = startsector;
              for (i=0; i<sizeinsectors; i++)
              {   endsector = (endsector==359) ? 369 : endsector+1;
              }
              byte* direntry = data + 16*(fidx%8);          
              direntry[0] = endsector < 720 ? 0x62 : 0x23;  // files not usable by dos 2.0
              direntry[1] = sizeinsectors & 0xff;
              direntry[2] = (sizeinsectors>>8) & 0xff;
              direntry[3] = startsector&0xff;
              direntry[4] = (startsector>>8)&0xff;
              for (i=5; i<16; i++) direntry[i]=32;
              char* n = atarifile.name();              
              int target=5;
              for (i=0; n[i] && target<16; i++)
              {   char c = n[i];
                  if (!c) { break; }
                  if (c=='.') { target=13; continue; }
                  direntry[target] = c;
                  target++;
              }              
              startsector = endsector;
          }  
          atarifile.close();
      }
      return true;
  }  
  // read sectors that belong to a file
  if ( (sector>=4 && sector<=359) || (sector>=369 && sector<=1023))
  {   unsigned long usablesector = (sector<=359) ? sector-4 : 356+(sector-369);
      // check if need to (re)open the atari file that covers the given sector
      if (!atarifile || usablesector<firstfilesector || usablesector>=firstfilesector+((atarifile.size()+124)/125))
      {   if (!openatarifile(usablesector))
          {   return false;
          }          
      }
      // read data from the file      
      unsigned long pos = (usablesector-firstfilesector)*125;
      if (!atarifile.seek(pos)) { return false; }
      unsigned long remainder = atarifile.size() - pos;      
      if (remainder<=125)  // the last sector of the file
      {   if (atarifile.read(data,remainder)!=remainder) { return false; }      
          data[125] = directoryindexoffile<<2;
          data[126] = 0; 
          data[127] = remainder & 0xff; 
      }
      else                // there are more sectors to follow
      {   if (atarifile.read(data,125)!=125) { return false; }      
          unsigned int nextsector = (sector==359) ? 369 : sector+1;
          data[125] = (directoryindexoffile<<2) | ((nextsector>>8) & 0x3);
          data[126] = nextsector & 0xff; 
          data[127] = 125; 
      }   
      return true; 
  }

  return false;
}

bool openatarifile(unsigned long usablesector)
{
      if (atarifile) { atarifile.close(); }

      firstfilesector = 0;
      directoryindexoffile = 0;
      
      while (directoryindexoffile<64)
      {   trynextatarifile(directoryindexoffile);
          if (!atarifile) { return false; }
          
          unsigned int sizeinsectors = (atarifile.size()+124) / 125;
          if (usablesector>=firstfilesector && usablesector<firstfilesector+sizeinsectors) 
          {   return true;
          }
          atarifile.close();
          firstfilesector += sizeinsectors;
          directoryindexoffile++;
      }
      return false;  
}
void trynextatarifile(int fidx)
{
    if (atarifile) { atarifile.close(); }    

    if (fidx==0)  // for the first list entry try to get DOS.SYS
    {   char fullname[50];
        fullname[0] = 0;
        strcat (fullname, "ATARI/");    
        strcat (fullname, diskfile.name());
        strcat (fullname, "/DOS.SYS");
        atarifile = SD.open(fullname);        
        diskfile.rewindDirectory();
        if (atarifile) return;
    }
    for (;;)
    {   atarifile = diskfile.openNextFile();
        if (!atarifile) return;
        // skip non-directory and DOS.SYS
        if (!atarifile.isDirectory() && strcmp(atarifile.name(),"DOS.SYS")!=0) 
        {   return;
        }        
        atarifile.close();
    }    
}


// --------------------- SIO PROTOCOL -----------------------

void sendwithchecksum(byte* data, int len)
{
    int sum = 0;
    int i;
    for (i=0; i<len; i++)
    {   sum += data[i];
        if (sum>=256)
        {  sum = (sum - 256) + 1;  // add carry into sum
        }  
    }
    SIO.write(data,len);
    SIO.write(sum);

//    for (i=0; i<len; i++) { PRINT(data[i], HEX); PRINT(" "); }
//    PRINTLN();
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
    {   data[i] = receivebyte();
        sum += data[i];
        if (sum>=256)
        {   sum = (sum - 256) + 1;  // add carry into sum
        }  
    }
    byte check = receivebyte();
    return check==sum;    
}

void logdata(byte* data, int length)
{
    int i;
    PRINT(F("DATA: "));
    for (i=0; i<length; i++)
    {   PRINT(data[i], HEX);
        PRINT(F(" "));
    }
    PRINTLN();  
}

void handlecommand_status()
{
    byte status[4];
    if (diskavailable())
    {   
        status[0] = 0x10;  // default status flags byte 0 (motor=on, single density)
        status[1] = 0x00;  // status flags byte 1
        status[2] = 0xe0;  // format timeout 
        status[3] = 0x00;  // copy of internal registers          
        if (disksize>720) status[0] |= 0x80; // enhanced density
    }
    else
    {   status[0] = 0x00;  // status flags byte 0  (motor=off)
        status[1] = 0x80;  // status flags byte 1  (not ready)
        status[2] = 0xe0;  // format timeout 
        status[3] = 0x00;  // copy of internal registers  
    }
    SIO.write('C');  
    sendwithchecksum(status,4);
}

bool handlecommand_read(unsigned int sector)
{ 
    byte data[128];
    if (!readsector(sector,data))
    {   SIO.write('E');
        PRINTLN(F("Read error"));
        return false;
    }      
    SIO.write('C');
    sendwithchecksum(data, 128);    
    return true;
}

bool handlecommand_write(byte drive, unsigned int sector)
{ 
    byte data[128];
    if (!receivewithchecksum(data,128))
    {   PRINTLN(F("Received sector with invalid checksum"));
        SIO.write('E');
        return true;
    } 

    delay(1);
    SIO.write('A');

    opendiskfile(selecteddisk[drive]);   
    if (!writesector(sector,data))
    {   PRINTLN(F("Writing sector to SD card failed"));
        SIO.write('E');
        return false;
    }      
  
    delay(1);      
    SIO.write('C');
    return true;
}

void handle_sio()
{
    int i;
    byte drive;
    unsigned int sector;
    
    // must wait until the command line goes low 
    while (digitalRead(PIN_SIOCOMMAND)!=LOW)
    {   int r = SIO.read();  // discard any data garbage that may have arrived
        if (r>=0)
        {   PRINT(F("Received while CMD not active: "));
            PRINTLN(r,HEX);
        }
    }
    // read the command - but abort when command line goes high prematurely
    byte command[5];
    int commandlength=0;
    while (commandlength<5 && digitalRead(PIN_SIOCOMMAND)==LOW)
    {   int r = SIO.read();
        if (r>=0)
        {   command[commandlength] = (byte) r;
            commandlength++;
        }
    }
    // when no command was sent at all, ignore the CMD being low for short time
    if (commandlength<=0) { return; }; 
    
    if (commandlength<5)
    {   PRINT(F("CMD went inactive before receiving full command: "));
        for (i=0; i<commandlength; i++)
        {   PRINT(command[i], HEX);
        }
        PRINTLN();
        return;
    }
    // waiting for command line to go high again, so it is allowed so send
    while (digitalRead(PIN_SIOCOMMAND)!=HIGH)
    {   int r = SIO.read();
        if (r>=0)
        {   commandlength++;
        }
    }
    // log incomming commands
    PRINT(F("CMD: "));
    for (i=0; i<5; i++)
    {   PRINT(command[i], HEX);
        PRINT(F(" "));
    }
    if (commandlength>5) 
    {   PRINT(F("too many bytes: "));
        PRINT(commandlength-5);
    }
    PRINTLN();
    
    // check if command has correct checksum
    int sum = 0;
    for (i=0; i<4; i++)
    {   sum += command[i];
        if (sum>=256)
        {  sum = (sum - 256) + 1;  // add carry into sum
        }
    }
    if (sum!=command[4])
    {   PRINTLN(F("Ignored command with invalid checksum"));
        return;
    }
    // when the command is not intended for floppy device D1 or D2, ignore it
    if (command[0]<0x31 || command[0]>0x32)
    {   PRINT(F("Received command for different device "));
        PRINTLN(command[0], HEX);
        return;       
    }
    drive = command[0]-0x31;
    // when the floppy drive is switched to "-" it is meant to be turned off
    if (selecteddisk[drive]>99)
    {   PRINT(F("Received command for turned off disk drive "));
        PRINTLN(command[0], HEX);
        return;       
    }
        
    switch (command[1])
    {   
        case 0x21:  // FORMAT SINGLE DENSITY        
        case 0x22:  // FORMAT MEDIUM DENSITY
        {   delay(1);
            SIO.write('A');
            delay(2);
            if (creatediskfile(selecteddisk[drive], (command[1]==0x21) ? 720 : 1040))
            {   byte buffer[128];
                for (i=0; i<128; i++) buffer[i]=0;
                SIO.write('C');                
                sendwithchecksum(buffer, 128);
            }
            else 
            {   SIO.write('E');
            }
            break;
        }
        case 0x53:  // STATUS
        {   delay(1);
            SIO.write('A');
            delay(2);
            opendiskfile(selecteddisk[drive]);
            handlecommand_status();
            break;
        }
        case 0x52:  // READ
        {   delay(1);
            SIO.write('A');
            delay(2);
            sector = ((unsigned int)command[2]) + (((unsigned int)command[3])<<8);
            opendiskfile(selecteddisk[drive]);
            if (handlecommand_read(sector))
            {   activitylight[drive] = ACTIVITY_LIGHT_DURATION;
            }
            break;
        }
        case 0x57:   // WRITE WITH VERIFY
        {   delay(1);
            sector = ((unsigned int)command[2]) + (((unsigned int)command[3])<<8);
            SIO.write('A');
            if (handlecommand_write(drive,sector))
            {   activitylight[drive] = ACTIVITY_LIGHT_DURATION;
            }
            break;
        }
        default:
        {   delay(1);
            SIO.write('N');            
            break;
        }
    }
}


// ----------------- INITIALIZATION  AND RUN LOOP ----------------------
void setup() 
{
    // start serial monitor for debugging
    PRINTBEGIN(9600);

    // try to initialize the SDcard - abort if failure
    if (!initsdcard()) 
    {   initdiskselector(true);
        return; 
    }
  
    // configure connection to the SIO interface
    pinMode(PIN_SIOCOMMAND,INPUT);
    SIO.begin(19200, SERIAL_8N1);  

    // start displaying digits  
    initdiskselector(false);       

    // main program loop (instead of loop() function)
    for (;;) { handle_sio(); }
}

void loop()
{
}



