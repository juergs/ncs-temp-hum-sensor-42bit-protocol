/*
  Liest dieses komische Tchibo-Wetterdingens per 433MhZ aus.
*/
#define telegramLength  42
boolean cnt=false;
int     reader=0;
int     cnter=0;
char    reading[telegramLength];

int   decodeTemp(String bitstream);
char    decodeTime(int time);

//-------------------------------------------------
void setup() 
{
  Serial.begin(57600);
  Serial.println("*** go! **");
  pinMode(2, INPUT);
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);   
}
//-------------------------------------------------
void loop() 
{
    //digitalWrite(12,LOW);   
    int LowVal=pulseIn(2,LOW);
    //digitalWrite(12,HIGH); 
    
    
    if (LowVal < 11000) 
	  {  
	    // Kuezer als 1100ms Low ? Koennte unserer Sensor sein
      if (decodeTime(LowVal) == 'S') 
	    {  
	      // Startsequenz ?
        cnt=true;                       // Dann go fuer die Sammlung
        cnter=0;                        // BitCounter auf 0
        //Serial.println("S*");
      }
      
      if ( (cnter<=(telegramLength-1)) && cnt && ((decodeTime(LowVal) == '0') || (decodeTime(LowVal)=='1'))) 
	    { 
	      // Stream noch nicht voll und ein Bit erkannt ?
        //Serial.print(decodeTime(LowVal)); 
        //Serial.print("LV: "); 
        //Serial.println(LowVal); 
        reading[cnter] = decodeTime(LowVal);  // Ab ins Array damit
        cnter = cnter+1;                      // Arraycounter fuers naechste Bit inkrementieren
      }
    } 
    else 
    {
      cnt=false;                           // Zurueck auf Anfang - nix fuer uns.
      //Serial.print("R*"); 
    }

    if ((cnter == telegramLength)) 
	  { 
	      // Arrray Voll ?
        Serial.print('/');
        Serial.print(reading);
        Serial.print('/');
        Serial.println(decodeTemp(reading));
        cnter=0;
        cnt=false; 
    }  
}
//-------------------------------------------------
//float decodeTemp(String bitstream) 
int decodeTemp(String bitstream) 
{  // Versucht aus dem Bitstrom die Temp. zu berechnen
  int x=0;
  int chk=0;
  for (int i=14;i<26;i++) 
  { 
    // Extrahiert das Byte zwischen Bit 14 und 25 und packt es als integer in "x"
    if (bitstream[i] == '1') 
	  {
      bitSet(x,(25-i));
    }
  }


  /*
   * 
   *  01 23456789 01 23 456789012345 67890123 45678901
      00 00111011 00 10 001101000110 00100010 00010110        ID: 59 / 1676

   *  01 23456789 01 23  4567 8901 2345  6789 0123 45678901
   *  00 01001110 00 00  0111 0100 0110   0011 0010 00011100  ID: 78 / 3724
                         0110 0100 0111   
   *   116                 6    4   7       3    2    28   
   *   116                 1607              32
   * 
   * 
   */
   
  //for (int i=4;i<15;i++) 
  for (int i=2;i<10;i++) 
  {      
    // Kenner aus den 3 Nibbles zwischen Bit 4 und 15 holen (koennte auch der Kanal sein ?)
    if (bitstream[i] == '1') 
	  {
      //bitSet(chk,(14-i));
      bitSet(chk,(9-i));
    }
  }

  Serial.print("ID: "); Serial.print(chk); Serial.print(" / ");
  /*
  if (chk != 136) 
  {            // Kenner = 136 ? Dann ist es unserer Sensor !
    return(-999);              // Wenn nicht, dann -999 zurueck
  } 
  else 
  {
  */
  return x;
    //return ((float)((int)x-900)/10);
  //}
}
//-------------------------------------------------
char decodeTime(int time)
{  // Wandelt die Pulse in Bits um.
   if (time > 700 && time < 11000) 
   {     
      // passendes Signal (zwischen 150ms und 11000ms) ?
      if (time < 2200) 
      {           
        // kleiner 250ms ? Dann LOW
        return '0';
      } 
 
      if (time < 5000 && time >3900) 
  		{  
  		  // Zwischen 500ms und 1000ms dann HIGH
            return '1';
      }
          
      if (time > 7800) 
      {   
        // Groesser 8ms dann Startsequenz !
        return 'S';
      }
  } 
  else 
  {
    return 'X';
  }  
}
