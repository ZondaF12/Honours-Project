/*
Ruaridh Bell
40314947

GPS and Accelerometer Tracking Datalogger

 ** MOSI - pin 11
 ** MISO - pin 12
 ** SCK - pin 13
 ** CS - pin 4

*/


#include <SD.h>
#define LED 8           //status LED for SD to state when it is active
#define MAXBUFFER 100   //creates the size of GPS & SD buffers

const int xpin = A0;  //x-axis of the accelerometer
const int ypin = A1;  //y-axis of the accelerometer
const int zpin = A2;  //z-axis of the accelerometer

char inBuffer[MAXBUFFER];    //Input buffer used to read NMEA lines from the GPS
byte outBuffer[MAXBUFFER];   //Output buffer used to write NMEA lines to the SD card
int sizeBuffer = 0;        //counter of how many characters there are in each NEMA line

File Logfile;  //States the file name of the created file on the SD card

const int chipSelect = 4;

void setup()
{ 
  Serial.begin(4800); //Sets the baud rate of the LS20031 GPS receiver to 4800 

  pinMode(10, OUTPUT);   //As per the SD library example code, pin 10 must be set to output
  pinMode(LED, OUTPUT); //sets the LED to an ouput
  
  Serial.print("Initializing SD card...");  
  //prints to the serial monitor saying that the SD card is initialising
  if (SD.begin(chipSelect)) { //Checks if SD card is detected
    Serial.println("initialisation done.");
  }
  else{ //if the initialisation of the SD card fails then pring this section of code to the serial monitor
    Serial.println("initialisation failed! Please check the following:"); 
    Serial.println("1. Is an SD card inserted?");
    Serial.println("2. Is the wiring correct?");
    Serial.println("Note: Press the reset button after the issue has been corrected to restart the system!");
    return;
  }
  //opens or creates a file called Logfile.log on the SD card
  Logfile = SD.open("Logfile.log", O_CREAT | O_WRITE);  

  if (Logfile) { //if the file opens successfully do this
    digitalWrite(LED, HIGH);  //The status LED turns on if the file opens 
    Serial.print("Logging Data...");
  } 
  else { //else, do this
    digitalWrite(LED,LOW);     //The status LED turns off if file fails to open
    Serial.println("error opening Logfile.log");
  }
} 
void loop()
{
  while (Serial.available()>0)  //while data is being recieved from the GPS do this
  {         
    sizeBuffer = Serial.readBytesUntil('\n', inBuffer, MAXBUFFER);  
    //this continuously reads NMEA line from GPS until end of the line then re does all this data again

    int xvalue = analogRead (xpin); //reads the data from the xpin (A0)
    int x = map(xvalue, 272, 409, -100, 100); //maps the extreme ends analog values from -100 to 100
    float xg = (float)x/(-100.00);  //converts the mapped value into acceleration in terms of "g"
    Serial.print(xg);
    
    int yvalue = analogRead (ypin); //reads the data from the ypin (A1)
    int y = map(yvalue, 269, 406, -100, 100);
    float yg = (float)y/(-100.00);
    Serial.print("\t");
    Serial.print(yg);
    
    int zvalue = analogRead (zpin); //reads the data from the zpin (A2)
    int z = map(zvalue, 287, 424, -100, 100);
    float zg = (float)z/(100.00);
    Serial.print("\t");
    Serial.println(zg);

    //loops the NMEA sentences to be written to the SD card
    for (int i = 0; i < MAXBUFFER; i++) outBuffer[i] = inBuffer[i];   

    if (Logfile) {
      Logfile.print(xg);    //writes the current value of zg (X) to the SD card
      Logfile.print(",");      
      Logfile.print(yg);    //writes the current value of zg (Y) to the SD card    
      Logfile.print(",");      
      Logfile.print(zg);    //writes the current value of zg (Z) to the SD card
      Logfile.print(",");     

      Logfile.write(outBuffer, sizeBuffer);  // write GPS NMEA output to SD
      Logfile.print("\r\n");  //this creates a Carriage-return then a new line   
      Logfile.flush();
      digitalWrite(LED, HIGH);  //Keeps the LED on so long as SD card logging is working.
    }
    else {
      digitalWrite(LED, LOW);  //turn the LED off if writing to the file fails
    }
  }
}
