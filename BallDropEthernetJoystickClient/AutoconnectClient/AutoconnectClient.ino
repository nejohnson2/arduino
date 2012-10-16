#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x19, 0x7C };
IPAddress ip(128,122,151,70);
//90-A2-DA-0D-19-7C
// Enter the IP address of the computer on which 
// you'll run the pong server:
IPAddress server(128,122,151,164);

const int sendInterval = 50;  // minimum time between messages to the server
long lastTimeSent = 0;       // timestamp of the last server message

float humidity = 0;
float temp = 0 ;

unsigned int localPort = 8888;      // local port to listen on

EthernetClient client; 
EthernetUDP Udp;

char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void setup()
{
  // initialize serial and Ethernet ports:
  Ethernet.begin(mac, ip);
  Udp.begin(localPort);
  Serial.begin(9600);  
  dht.begin();

  delay(1000);      // give the Ethernet shield time to set up
  Serial.println("Starting");
  
    // Print the local ip address
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
}

void loop()
{
  // note the current time in milliseconds:
  long currentTime = millis();

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  
  char receivedStop;

  int packetSize = Udp.parsePacket();

  /******************** Read data in from the sensor  *****************/
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
  }
  else {
    Serial.print("Humidity: "); 
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(t);
    Serial.println(" *C");

    //client.print("n=Nick Temp\n");
  }
  if(client.connected()){

      
   

      if(h > humidity){
        client.print("r"); 
        Serial.println("humidity greater");
        Serial.println(humidity);
        humidity = h;
      } else if(h < humidity){
        client.print("l");
        humidity = h;
        Serial.println("humidity less");
      }
      

      if(t > temp){
        client.print("u");
        temp = t;
      }else if (t < temp){
        client.print("d");
        temp = t;
      }
       lastTimeSent = currentTime; 
       
    client.print("n=");
    client.print(t);
    client.print("\n");
    }  
  


  /******************** Receive UDP Packets  ******************/
  if(packetSize)
  {
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    Serial.print("From ");
    IPAddress remote = Udp.remoteIP();
    for (int i =0; i < 4; i++)
    {
      Serial.print(remote[i], DEC);
      if (i < 3)
      {
        Serial.print(".");
      }
    }
    Serial.print(", port ");
    Serial.println(Udp.remotePort());

    // read the packet into packetBufffer
    Udp.read(packetBuffer,UDP_TX_PACKET_MAX_SIZE);
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    packetBuffer[0];
    receivedStop = packetBuffer[0];

    /*************  Disconnect from Server  ****************/
    if(receivedStop == 'x') {
      client.print("x");
      // if the client's connected, disconnect:
      client.stop();
      Serial.println("disconnecting");
    }

    /************* Connect to Server  ********************/
    if(receivedStop == 'o'){
      client.connect(server,8080);
      Serial.println("connected");

      
    }

  }
}



