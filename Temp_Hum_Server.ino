/*

 This is going to read in temperature and humidity values and create
 a server which will post this on the web.  
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "DHT.h"

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x19, 0x7C };
IPAddress ip(192,168,0,12);

EthernetServer server(80);

void setup()
{
  Ethernet.begin(mac, ip);
  server.begin(); 
  Serial.begin(9600);
  dht.begin();

  delay(1000);
  Serial.println("Starting");

  //Print local ip address
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  EthernetClient client = server.available();

  // Read in the Humidity and Temp values
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
  }  

  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();

          client.println("Humidity: "); 
          client.println(h);
          client.println(" %\t");
          client.println("Temperature: "); 
          client.println(t);
          client.println(" *C");
          // output the value of each analog input pin
          //  for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
          //    client.print("analog input ");
          //    client.print(analogChannel);
          //    client.print(" is ");
          //    client.print(analogRead(analogChannel));
          //    client.println("<br />");
          //  }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}



