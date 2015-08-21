#include <SPI.h>
#include <SD.h>
#include <TimerOne.h>

#define PIN_CHIPSELECT  13
#define PIN_SIOCOMMAND  2


// ------------------ DISC SELECTOR (LED DIGITS AND BUTTONS) -----------
// because the two digits are displayed in a multiplexted fasion (to reduce number of needed pins),
// they will be displayed alternately. a timer interrupt  comes handy to do this.

byte prevbuttonstate[4];

volatile byte digit0value;  
volatile byte digit1value;  
volatile int activitylight;
byte activedigit; 

// various digits                0      1      2      3      4      5      6      7      8     9
bool segment_top[10]         = { true,  false, true,  true,  false, true,  true,  true,  true, true  };
bool segment_middle[10]      = { false, false, true,  true,  true,  true,  true,  false, true, true  };
bool segment_bottom[10]      = { true,  false, true,  true,  false, true,  true,  false, true, true  };
bool segment_lefttop[10]     = { true,  false, false, false, true,  true,  true,  false, true, true  };
bool segment_leftbottom[10]  = { true,  false, true,  false, false, false, true,  false, true, false };
bool segment_righttop[10]    = { true,  true,  true,  true,  true,  false, false, true,  true, true  };
bool segment_rightbottom[10] = { true,  true,  false, true,  true,  true,  true,  true,  true, true  };

void initdiscselector()
{
  int i;

  // init input buttons
  for (int i=0; i<4; i++)
  {   pinMode(A0+i, INPUT);
      prevbuttonstate[i] = HIGH;
  }
  
  // init visible digit values and toggle register
  digit0value = 0;
  digit1value = 0;
  activedigit = 0;
  activitylight = 0;
  
  // digit selectors are active high (turn off everything at begin)
  pinMode(11, OUTPUT);
  digitalWrite (11, LOW);
  pinMode(12, OUTPUT);
  digitalWrite (12, LOW);  
  // individual digit segments are active low (turn off everything at begin)
  for (i=3; i<=10; i++)
  {    
    pinMode(i, OUTPUT);
    digitalWrite (i, HIGH);
  }
  
  // start timer
  Timer1.initialize(5000);     // toggle with 100 Hz   
  Timer1.attachInterrupt(polldiscselector);    
}


int getdiskselectorvalue()
{
  return digit0value + digit1value*10;  
}

int setactivitylight()
{
    activitylight = 10;  
}

void polldiscselector()   // interrupt service routine (must not take much time)
{
  // --- query button changes ---
  int i;
  for (i=0; i<4; i++)
  {
      byte s = digitalRead(A0+i);
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
               case 2: if (digit0value<9)
                       {  digit0value++;
                       }
                       else 
                       {  digit0value=0;
                          digit1value = digit1value<9 ? digit1value+1 : 0;
                       }
                       break;
               case 3: if (digit0value>0)
                       {  digit0value--;
                       }
                       else
                       {
                         digit0value = 9;
                         digit1value = digit1value>0 ? digit1value-1 : 9;
                       }
                       break;
            }
        }
      }
  } 

  // --- count down activity light duration
  if (activitylight>0)
  {
     activitylight--;
  }
  
  // --- display digits alternatingly ----
  // turn off previously active digit
  digitalWrite (11, LOW);
  digitalWrite (12, LOW);
  
  // switch to other digit
  activedigit = 1-activedigit;
  
  // turn on/off digit segments as needed (pin assignment is very arbitrary)
  if (activedigit==0)
  {
     byte v = digit0value;
     digitalWrite (3, segment_top[v] ? LOW : HIGH);
     digitalWrite (6, segment_middle[v] ? LOW : HIGH);
     digitalWrite (7, segment_bottom[v] ? LOW : HIGH);
     digitalWrite (5, segment_lefttop[v] ? LOW : HIGH);  
     digitalWrite (9, segment_leftbottom[v] ? LOW : HIGH);
     digitalWrite (10, segment_righttop[v] ? LOW : HIGH);  
     digitalWrite (8, segment_rightbottom[v] ? LOW : HIGH);
     digitalWrite (4, activitylight>0 ? LOW:HIGH);   // the DOT is used to show activity 
  
     digitalWrite (12, HIGH);  // turn on digit 0
   }
   else
   {
     byte v = digit1value;
     digitalWrite (3, segment_top[v] ? LOW : HIGH);
     digitalWrite (7, segment_middle[v] ? LOW : HIGH);
     digitalWrite (4, segment_bottom[v] ? LOW : HIGH);
     digitalWrite (10, segment_lefttop[v] ? LOW : HIGH);  
     digitalWrite (8, segment_leftbottom[v] ? LOW : HIGH);
     digitalWrite (5, segment_righttop[v] ? LOW : HIGH);  
     digitalWrite (6, segment_rightbottom[v] ? LOW : HIGH);
     digitalWrite (9, HIGH);         // DOT is unused

     digitalWrite (11, HIGH);  // turn on digit 1
   }
}


// ---------------------- DISK FILE HANDLING  -----------------------

bool didinitsd = false;

File discfile;  
unsigned int discsize;         // size in sectors
bool discreadonly;

void opendiscfile(int index)
{  
  // check if desired file is already open
  if (discfile && isrequesteddiscfile(discfile,index))
  {    
    return;
  } 
  
  // before switching disc file, close previous one if still open
  if (discfile )
  {
    discfile.close();
  }
  
  // if not done, try to initialize the SD subsystem
  if (!didinitsd)
  {  
      pinMode(PIN_CHIPSELECT,OUTPUT);
      digitalWrite(PIN_CHIPSELECT,HIGH);
      if (!SD.begin(PIN_CHIPSELECT)) {
        Serial.println("SDCard failed, or not present");
        return;
      }
      didinitsd = true;
  }
        
  // try to scan the files in the ATARI directory on the SDCARD and 
  // locate the file name beginning with the index (2 digits).
  File root = SD.open("ATARI");
  if (!root || !root.isDirectory())
  {
      Serial.println("Can not locate ATARI directory");
      return;    
  }  
  for (;;)
  {  File entry = root.openNextFile(O_RDWR);
     if (!entry) break;
Serial.print("trying file: ");
Serial.println(entry.name());

     // check if name matches
     if (isrequesteddiscfile(entry,index))
     {
       Serial.print("Trying to open ");
       Serial.println(entry.name());      
       
       // read the header
       byte header[16];
       int didread = entry.readBytes(header,16);
       if (didread!=16)
       {
          Serial.println("Can not read file header");
          goto incorrect;          
       }
       // check for signature and other settings
       if (header[0]!=0x96 || header[1]!=0x02)
       {
          Serial.println("Magic number not present");
          goto incorrect;          
       }
       if (header[4]!=0x80 || header[5]!=0x00)
       {
          Serial.println("Can only handle 128 byte sectors");
          goto incorrect;          
       }
       unsigned long paragraphs = header[6];
       paragraphs = (paragraphs<<8) | header[3];
       paragraphs = (paragraphs<<8) | header[2];
       if (paragraphs<8 || paragraphs>0x70000)
       {
          Serial.println("Disk file size out of range.");
          goto incorrect;                   
       }
       
       // seems to be a valid diskfile - use it
       discfile = entry;
       discsize = paragraphs >> 3;
       discreadonly = (header[15]&1 == 1);
       
       Serial.print("SIZE: ");
       Serial.print(discsize);
       Serial.print(" sectors of 128 bytes");
       Serial.println(discreadonly ? " (readonly)": "");
       
       break;   // stop searching
     }
    
    // incorrect file
  incorrect:
    entry.close();
  }
  
  root.close();
  
  if (!discfile)
  {
     Serial.print("Could not find file for disc ");
     Serial.println(index);
  }
}

bool isrequesteddiscfile(File f, int index)
{
  char* n = f.name();
  return (n[0]>='0' && n[0]<='9' && n[1]>='0' && n[1]<='9' && ((n[0]-'0')*10 + n[1]-'0')==index);
}


bool readsector(unsigned int sector, byte* data)
{
  if (!discfile || sector>=discsize)
  {  return false;
  }
  if (!discfile.seek(16+((unsigned long)sector)*128))
  {  return false;
  }
  return discfile.readBytes(data, 128) == 128;
}

bool writesector(unsigned int sector, byte* data)
{
  if (!discfile || sector>=discsize)
  {  return false;
  }
  if (!discfile.seek(16+((unsigned long)sector)*128))
  {  return false;
  }
  return discfile.write(data, 128) == 128;
}

bool discavailable()
{
    if (discfile)
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
  Serial1.write(data,len);
  Serial1.write(sum);
}

byte receivebyte()
{
  int d = Serial1.read();
  while (d<0)
  {  d = Serial1.read();
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
      Serial.print("CMD: ");
      for (i=0; i<5; i++)
      {  
          Serial.print(command[i], HEX);
          Serial.print(" ");
      }
      Serial.println("]");
}      

void logdata(byte* data, int length)
{
    int i;
      Serial.print("DATA: ");
      for (i=0; i<length; i++)
      {  
          Serial.print(data[i], HEX);
          Serial.print(" ");
      }
      Serial.println();  
}

void handlecommand_status()
{
  opendiscfile(getdiskselectorvalue());

  byte status[4];
  if (discavailable())
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
  Serial1.write('C');  
  sendwithchecksum(status,4);
}

void handlecommand_read(unsigned int sector)
{
   setactivitylight();
  opendiscfile(getdiskselectorvalue());
 
  byte data[128];
  if (!readsector(sector,data))
  {
    Serial1.write('E');
    return;
  }      
  
//  Serial.print(sector);
//  logdata(data,128);

//  bool evensector = (sector&1) == 0;
//  setdigitsanddots(getselecteddisc(), evensector,!evensector);
  
  Serial1.write('C');
  sendwithchecksum(data,128);
}

void handlecommand_write(unsigned int sector)
{ 
   byte data[128];
   if (!receivewithchecksum(data,128))
   {
     Serial.println("Received sector with invalid checksum");
     Serial1.write('E');
     return;
   } 

   delay(1);
   Serial1.write('A');
   
   setactivitylight();
   opendiscfile(getdiskselectorvalue());
 
  if (!writesector(sector,data))
  {
    Serial.println("Writing sector to SD card failed");
    Serial1.write('E');
    return;
  }      
  
//  Serial.print(sector);
//  logdata(data,128);

//  bool evensector = (sector&1) == 0;
//  setdigitsanddots(getselecteddisc(), evensector,!evensector);
  
   delay(1);      
  Serial1.write('C');
}


void handle_sio()
{
  int i;

    // must wait until the command line goes low 
    while (digitalRead(PIN_SIOCOMMAND)!=LOW)
    {
      int r = Serial1.read();  // discard any data garbage that may have arrived
      if (r>=0)
      {  
        Serial.print("Received while CMD not active: ");
        Serial.println(r,HEX);
      }
    }
    // read the command - but abort when command line goes high prematurely
    byte command[5];
    int commandlength=0;
    while (commandlength<5 && digitalRead(PIN_SIOCOMMAND)==LOW)
    {  
      int r = Serial1.read();
      if (r>=0)
      {
        command[commandlength] = (byte) r;
        commandlength++;
      }
    }
    if (commandlength<5)
    {  Serial.println("CMD went inactive before receiving full command");
       return;
    }
    // waiting for command line to go high again, so it is allowed so send
    while (digitalRead(PIN_SIOCOMMAND)!=HIGH)
    { 
      int r = Serial1.read();
      if (r>=0)
      {
        Serial.print("Received more data before CMD went inactive: ");
        Serial.println(r,HEX);
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
      Serial.print("Ignored command with invalid checksum");
      return;
    }
    // when the command is not inteded for this device, ignore it
    if (command[0]!=0x31)
    {              
//      Serial.print("Received command for different device ");
//      Serial.println(command[0], HEX);
      return;       
    }
    
    switch (command[1])
    {
        case 0x53:  // STATUS
          logcommand(command);
          Serial1.write('A');
          delay(1);
          handlecommand_status();
          break;
        case 0x52:  // READ
          logcommand(command);
          Serial1.write('A');
          delay(1);
          handlecommand_read( ((unsigned int)command[2]) + (((unsigned int)command[3])<<8) - 1);
          break;
        case 0x57:   // WRITE WITH VERIFY
          logcommand(command);
          Serial1.write('A');
          handlecommand_write( ((unsigned int)command[2]) + (((unsigned int)command[3])<<8) - 1);
          break;
        default:
          Serial1.write('N');
          logcommand(command);
          break;
    }
}


// ---------------- MAIN PROGRAM --------------------
void setup() {
  // start serial monitor for debugging
  Serial.begin(9600);       
//  while( Serial.read()<0);

  // configure connection to the SIO interface
  pinMode(PIN_SIOCOMMAND,INPUT);
  Serial1.begin(19200, SERIAL_8N1);  
  
  // start displaying digits  
  initdiscselector();
}

void loop()
{
  handle_sio();
}

