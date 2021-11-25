// SelectSpeakers_S1  7/8/2020
// SIMPLE SERVER CODE
//
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#define REQ_BUF_SZ 60

byte mac[] = {0x90,0xA2,0xDA,0x0F,0x55,0x60};
IPAddress ip(192, 168, 1, 179);
EthernetServer server(8081);
File webFile;
char HTTP_req[REQ_BUF_SZ] = {0};
char req_index = 0;
int LastSRC = 5;
int mute = 0;
int DenonDelay = 600;
int VolDelay = 800;
boolean SEL_state[8] = {0};

void SetRelays(void);
void XML_response(EthernetClient);
void StrClear(char*, char);
char StrContains(char*, const char*);
void UnMute(void);

void setup()
{  pinMode(22, INPUT_PULLUP);
   pinMode(22, OUTPUT);
   pinMode(24, INPUT_PULLUP);
   pinMode(24, OUTPUT);
   pinMode(26, INPUT_PULLUP);
   pinMode(26, OUTPUT);
   pinMode(28, INPUT_PULLUP);
   pinMode(28, OUTPUT);
   pinMode(30, INPUT_PULLUP);
   pinMode(30, OUTPUT);
   pinMode(32, INPUT_PULLUP);
   pinMode(32, OUTPUT);
   pinMode(34, INPUT_PULLUP);
   pinMode(34, OUTPUT);
   pinMode(36, INPUT_PULLUP);
   pinMode(36, OUTPUT);
   pinMode(31, INPUT_PULLUP);
   pinMode(31, OUTPUT);
   pinMode(33, INPUT_PULLUP);
   pinMode(33, OUTPUT);
   pinMode(35, INPUT_PULLUP);
   pinMode(35, OUTPUT);
   pinMode(37, INPUT_PULLUP);
   pinMode(37, OUTPUT);
   pinMode(39, INPUT_PULLUP);
   pinMode(39, OUTPUT);
   pinMode(41, INPUT_PULLUP);
   pinMode(41, OUTPUT);
   pinMode(43, INPUT_PULLUP);
   pinMode(43, OUTPUT);
   
  Serial.begin(9600);
  
   Serial.println("Initializing SD card...");
   if (!SD.begin(4))
   {   Serial.println("SD card init failed!");
       return;
   }
   Serial.println("SUCCESS - SD card initialized.");
   
   Ethernet.begin(mac, ip);
   server.begin();
}

void loop()
{
   EthernetClient client = server.available();
   if (client)
   {   boolean currentLineIsBlank = true;
     
       while (client.connected())
       {
           if (client.available())
           {
               char c = client.read();  // read 1 byte (character) from client

               if (req_index < (REQ_BUF_SZ - 1)) {
                   HTTP_req[req_index] = c;          // save HTTP request character
                   req_index++;
               }
              
               if (c == '\n' && currentLineIsBlank)
               { 
                   client.println("HTTP/1.1 200 OK");
                        
                   if (StrContains(HTTP_req, "ajax_inputs"))
                   {
                      client.println("Content-Type: text/xml");
                      client.println("Connection: keep-alive");
                      client.println();
                      
                      SetRelays();
                      
                      XML_response(client);
                   }
                   else
                   {
                      client.println("Content-Type: text/html");
                      client.println("Connection: keep-alive");
                      client.println();

                      webFile = SD.open("index.htm");        // open web page file
                      if (webFile)
                      {
                         while(webFile.available())
                         {
                            client.write(webFile.read());  // send web page to client
                         }
                         webFile.close();
                      }
                   }

                   req_index = 0;
                   StrClear(HTTP_req, REQ_BUF_SZ);
                   break;
               }

               if (c == '\n')
               {  currentLineIsBlank = true; }
               else if (c != '\r')
               {   currentLineIsBlank = false; }
           }
       }
       delay(1);
       client.stop();
   }
}

void SetRelays(void)
{  // Kitchen = SEL1
   if (StrContains(HTTP_req, "SEL1=1"))
   {   digitalWrite(22, LOW);
       digitalWrite(24, LOW);
       SEL_state[0] = 1;
   }
   else if (StrContains(HTTP_req, "SEL1=0"))
   {   digitalWrite(22, HIGH);
       digitalWrite(24, HIGH);
       SEL_state[0] = 0;
   }
  
   // Library = SEL2
   if (StrContains(HTTP_req, "SEL2=1"))
   {   digitalWrite(26, LOW);
       digitalWrite(28, LOW);
       SEL_state[1] = 1;
   }
   else if (StrContains(HTTP_req, "SEL2=0"))
   {   digitalWrite(26, HIGH);
       digitalWrite(28, HIGH);
       SEL_state[1] = 0;
   }
  
   // Master Bedroom = SEL3
   if (StrContains(HTTP_req, "SEL3=1"))
   {   digitalWrite(30, LOW);
       digitalWrite(32, LOW);
       SEL_state[2] = 1;
   }
   else if (StrContains(HTTP_req, "SEL3=0"))
   {   digitalWrite(30, HIGH);
       digitalWrite(32, HIGH);
       SEL_state[2] = 0;
   }
  
   // Back Bedroom = SEL4
   if (StrContains(HTTP_req, "SEL4=1"))
   {   digitalWrite(34, LOW);
       digitalWrite(36, LOW);
       SEL_state[3] = 1;
   }
   else if (StrContains(HTTP_req, "SEL4=0"))
   {   digitalWrite(34, HIGH);
       digitalWrite(36, HIGH);
       SEL_state[3] = 0;
   }

   // Bedroom CD/USB Player = SEL5
   if (StrContains(HTTP_req, "SEL5=1"))
   {   digitalWrite(37, LOW);
       delay(DenonDelay);
       digitalWrite(37, HIGH);
       SEL_state[4] = 1;
       SEL_state[5] = 0;
       SEL_state[6] = 0;
       SEL_state[7] = 0;
       LastSRC = 5;
   }
  
   // Den CD Player = SEL6
   if (StrContains(HTTP_req, "SEL6=1"))
   {   digitalWrite(35, LOW);
       delay(DenonDelay);
       digitalWrite(35, HIGH);
       SEL_state[4] = 0;
       SEL_state[5] = 1;
       SEL_state[6] = 0;
       SEL_state[7] = 0;
       LastSRC = 6;
   }
  
   // TV cable box = SEL7
   if (StrContains(HTTP_req, "SEL7=1"))
   {   digitalWrite(39, LOW);
       delay(DenonDelay);
       digitalWrite(39, HIGH);
       SEL_state[4] = 0;
       SEL_state[5] = 0;
       SEL_state[6] = 1;
       SEL_state[7] = 0;
       LastSRC = 7;
   }
   
   // FM input = SEL8
   if (StrContains(HTTP_req, "SEL8=1"))
   {   digitalWrite(41, LOW);
       delay(DenonDelay);
       digitalWrite(41, HIGH);
       SEL_state[4] = 0;
       SEL_state[5] = 0;
       SEL_state[6] = 0;
       SEL_state[7] = 1;
       LastSRC = 8;
   }
    
   // volume Up = VOL9.  '=1' means vol up
   if (StrContains(HTTP_req, "VOL9=1"))
   {
       if (mute == 1)
       {  UnMute(); }
       else
       {  digitalWrite(31, LOW);
          delay(VolDelay);
          digitalWrite(31, HIGH);
       }
   }
  
   // volume Down = VOL10.  '=1' means vol down
   if (StrContains(HTTP_req, "VOL10=1"))
   { 
       if (mute == 1)
       {  UnMute(); }
       else
       {  digitalWrite(33, LOW);
          delay(VolDelay);
          digitalWrite(33, HIGH);
       }
   }

   // USB input = VOL11.  button toggles Mute
   if (StrContains(HTTP_req, "VOL11=1"))
   {   
       if (mute == 0)
       {  digitalWrite(43, LOW);
          delay(DenonDelay);
          digitalWrite(43, HIGH);
          mute = 1;
       }
       else
       {  UnMute(); }
   }
}

void UnMute(void)
{
   // last source was Bedroom CD
   if ( LastSRC == 5 )
   {   digitalWrite(37, LOW);
       delay(DenonDelay);
       digitalWrite(37, HIGH);
   }
   // last source was Den CD
   else if ( LastSRC == 6 )
   {   digitalWrite(35, LOW);
       delay(DenonDelay);
       digitalWrite(35, HIGH);
   }
   // last source was TV
   else if ( LastSRC == 7 )
   {   digitalWrite(39, LOW);
       delay(DenonDelay);
       digitalWrite(39, HIGH);
   }
   // last source was FM
   else if ( LastSRC == 8 )
   {   digitalWrite(41, LOW);
       delay(DenonDelay);
       digitalWrite(41, HIGH);
   }
   
   mute = 0;
}

void XML_response(EthernetClient cl)
{  cl.println("<?xml version = \"1.0\" ?>");
   cl.println("<inputs>");
  
   // Kitchen = SEL1
   cl.print("<SEL>");
   if (SEL_state[0])
   { cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
        
   // Library = SEL2
   cl.print("<SEL>");
   if (SEL_state[1])
   { cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
  
   // Master Bedroom = SEL3
   cl.print("<SEL>");
   if (SEL_state[2])
   { cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
   
   // Back Bedroom = SEL4
   cl.print("<SEL>");
   if (SEL_state[3])
   { cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");

   // Bedroom CD/USB Player = SEL5
   cl.print("<SEL>");
   if (SEL_state[4])
   {  cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
   
   // Den CD Player = SEL6
   cl.print("<SEL>");
   if (SEL_state[5])
   {  cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
  
   // TV cable box = SEL7
   cl.print("<SEL>");
   if (SEL_state[6])
   {  cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
   
   // FM input = SEL8
   cl.print("<SEL>");
   if (SEL_state[7])
   {  cl.print("checked"); }
   else
   { cl.print("unchecked"); }
   cl.println("</SEL>");
   
   cl.println("</inputs>");
}

void StrClear(char *str, char length)
{
   for (int i = 0; i < length; i++) 
   { str[i] = 0; }
}

char StrContains(char *str, const char *sfind)
{
   char found = 0;
   char index = 0;
   char len;
   len = strlen(str);
   
   if (strlen(sfind) > len)
   {  return 0; }
   while (index < len)
   {
     if (str[index] == sfind[found])
     {  found++;
        if (strlen(sfind) == found)
        {  return 1; }
     }
     else
     {  found = 0; }
     index++;
   }
   return 0;
}
