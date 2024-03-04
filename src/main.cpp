/*

MIT License

Copyright (c) [2023] [MADE4HOME]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma region Definitions

#define UPDATE_INTERVAL_MS 2000

#define DEFAULT_SSID "YOUR_SSID"

#define DEFAULT_PASS "YOUR_PASS"

#pragma endregion

#pragma region Headers

#include <Arduino.h>

#include <WiFi.h>

#include "made4home.h"

#pragma endregion

#pragma region Variables

/**
 * @brief Ethernet connection state.
 * 
 */
static bool EthernetConnected_g = false;

/**
 * @brief Current time.
 * 
 */
unsigned long CurrentTime_g = millis();

/**
 * @brief Previous time.
 * 
 */
unsigned long PreviousTime_g = 0; 

/**
 * @brief Variable to store the HTTP request.
 * 
 */
String Header_g;

/**
 * @brief Relay 1 state.
 * 
 */
String Relay1State_g = STATE_OFF;

/**
 * @brief Relay 2 state.
 * 
 */
String Relay2State_g = STATE_OFF;

/**
 * @brief Relay 3 state.
 * 
 */
String Relay3State_g = STATE_OFF;

/**
 * @brief Relay 4 state.
 * 
 */
String Relay4State_g = STATE_OFF;

/**
 * @brief Set web server port number to 80.
 * 
 */
WiFiServer *Server_g;

/**
 * @brief Replace with your network credentials.
 * 
 */
const char* SSID_g = DEFAULT_SSID;

/**
 * @brief Replace with your network credentials.
 * 
 */
const char* PASS_g = DEFAULT_PASS;

#pragma endregion

#pragma region Prototypes

/**
 * @brief Connect to WiFi.
 * 
 */
void connect_to_wifi();

/**
 * @brief Do serve.
 * 
 */
void do_serve();

#pragma endregion

void setup()
{
    // Setup the serial port.
    Serial.begin(DEFAULT_BAUDRATE, SERIAL_8N1);
    while (!Serial) {}

    Made4Home.setup();
    
    // Connect to Wi-Fi network with SSID and password.
    connect_to_wifi();

    Server_g = new WiFiServer(80);
    Server_g->begin();
}

void loop()
{
    do_serve();
}

#pragma region Functions

/**
 * @brief Connect to WiFi.
 * 
 */
void connect_to_wifi()
{
    Serial.print("Connecting to ");
    Serial.println(SSID_g);
    WiFi.begin(SSID_g, PASS_g);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.print("Connected to ");
    Serial.println(SSID_g);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

/**
 * @brief Do serve.
 * 
 */
void do_serve()
{
    // Listen for incoming clients
    WiFiClient ClientL = Server_g->available();
    
    // If a new client connects,
    if (ClientL)
    {
        CurrentTime_g = millis();
        PreviousTime_g = CurrentTime_g;
        Serial.println("New Client.");          // print a message out in the serial port
        String CurrentLineL = "";                // make a String to hold incoming data from the client

        // loop while the client's connected
        while (ClientL.connected() && CurrentTime_g - PreviousTime_g <= UPDATE_INTERVAL_MS)
        {
            CurrentTime_g = millis();
            if (ClientL.available())
            {
            // if there's bytes to read from the client,
                char CharacterL = ClientL.read();             // read a byte, then
                Serial.write(CharacterL);                    // print it out the serial monitor
                Header_g += CharacterL;
                if (CharacterL == '\n') {                    // if the byte is a newline character
                // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:
                if (CurrentLineL.length() == 0)
                {
                    // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                    // and a content-type so the client knows what's coming, then a blank line:
                    ClientL.println("HTTP/1.1 200 OK");
                    ClientL.println("Content-type:text/html");
                    ClientL.println("Connection: close");
                    ClientL.println();

                    if (Header_g.indexOf("GET /R1/on") >= 0)
                    {
                        Serial.println("Relay 1 on");
                        Relay1State_g = STATE_ON;
                        Made4Home.digitalWrite(0, HIGH);
                    }
                    else if (Header_g.indexOf("GET /R1/off") >= 0)
                    {
                        Serial.println("Relay 1 off");
                        Relay1State_g = STATE_OFF;
                        Made4Home.digitalWrite(0, LOW);
                    }
                    else if (Header_g.indexOf("GET /R2/on") >= 0)
                    {
                        Serial.println("Relay 2 on");
                        Relay2State_g = STATE_ON;
                        Made4Home.digitalWrite(1, HIGH);
                    }
                    else if (Header_g.indexOf("GET /R2/off") >= 0)
                    {
                        Serial.println("Relay 2 off");
                        Relay2State_g = STATE_OFF;
                        Made4Home.digitalWrite(1, LOW);
                    }
                    else if (Header_g.indexOf("GET /R3/on") >= 0)
                    {
                        Serial.println("Relay 3 on");
                        Relay3State_g = STATE_ON;
                        Made4Home.digitalWrite(2, HIGH);
                    }
                    else if (Header_g.indexOf("GET /R3/off") >= 0)
                    {
                        Serial.println("Relay 3 off");
                        Relay3State_g = STATE_OFF;
                        Made4Home.digitalWrite(2, LOW);
                    }
                    // turns the GPIOs on and off
                    else if (Header_g.indexOf("GET /R4/on") >= 0)
                    {
                        Serial.println("Relay 4 on");
                        Relay4State_g = STATE_ON;
                        Made4Home.digitalWrite(3, HIGH);
                    }
                    else if (Header_g.indexOf("GET /R4/off") >= 0)
                    {
                        Serial.println("Relay 4 off");
                        Relay4State_g = STATE_OFF;
                        Made4Home.digitalWrite(3, LOW);
                    }
                    
                    // Display the HTML web page
                    ClientL.println("<!DOCTYPE html><html>");
                    
                    ClientL.println("<head>");

                    ClientL.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                    ClientL.println("<link rel=\"icon\" href=\"data:,\">");
                    // CSS to style the on/off buttons 
                    // Feel free to change the background-color and font-size attributes to fit your preferences
                    ClientL.println("<style>");
                    ClientL.println("html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                    ClientL.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
                    ClientL.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                    ClientL.println(".button2 {background-color: #555555;}");
                    ClientL.println("</style>");
                    
                    ClientL.println("</head>");
                    
                    // Web Page Heading
                    ClientL.println("<body>");

                    // 
                    ClientL.println("<h1>Made4Home WEB Server</h1>");

                    // Display inputs
                    ClientL.println("<p>Input 1: " + String(Made4Home.digitalRead(0) ? STATE_ON : STATE_OFF) + "</p>");
                    ClientL.println("<p>Input 2: " + String(Made4Home.digitalRead(1) ? STATE_ON : STATE_OFF) + "</p>");
                    ClientL.println("<p>Input 3: " + String(Made4Home.digitalRead(2) ? STATE_ON : STATE_OFF) + "</p>");
                    ClientL.println("<p>Input 4: " + String(Made4Home.digitalRead(3) ? STATE_ON : STATE_OFF) + "</p>");

                    // Display current state, and ON/OFF buttons for Relay 1.
                    ClientL.println("<p>Relay 1: " + Relay1State_g + "</p>");
                    // If the Relay1State_g is off, it displays the ON button.
                    if (Relay1State_g==STATE_OFF)
                    {
                        ClientL.println("<p><a href=\"/R1/on\"><button class=\"button\">ON</button></a></p>");
                    }
                    else
                    {
                        ClientL.println("<p><a href=\"/R1/off\"><button class=\"button button2\">OFF</button></a></p>");
                    }

                    // Display current state, and ON/OFF buttons for Relay 2.
                    ClientL.println("<p>Relay 2: " + Relay2State_g + "</p>");
                    // If the Relay2State_g is off, it displays the ON button.
                    if (Relay2State_g==STATE_OFF)
                    {
                        ClientL.println("<p><a href=\"/R2/on\"><button class=\"button\">ON</button></a></p>");
                    }
                    else
                    {
                        ClientL.println("<p><a href=\"/R2/off\"><button class=\"button button2\">OFF</button></a></p>");
                    }

                    
                    // Display current state, and ON/OFF buttons for Relay 3.
                    ClientL.println("<p>Relay 3: " + Relay3State_g + "</p>");
                    // If the Relay3State_g is off, it displays the ON button.
                    if (Relay3State_g==STATE_OFF)
                    {
                        ClientL.println("<p><a href=\"/R3/on\"><button class=\"button\">ON</button></a></p>");
                    }
                    else
                    {
                        ClientL.println("<p><a href=\"/R3/off\"><button class=\"button button2\">OFF</button></a></p>");
                    }

                    // Display current state, and ON/OFF buttons for Relay 4.
                    ClientL.println("<p>Relay 4: " + Relay4State_g + "</p>");
                    // If the Relay4State_g is off, it displays the ON button.
                    if (Relay4State_g==STATE_OFF)
                    {
                        ClientL.println("<p><a href=\"/R4/on\"><button class=\"button\">ON</button></a></p>");
                    }
                    else
                    {
                        ClientL.println("<p><a href=\"/R4/off\"><button class=\"button button2\">OFF</button></a></p>");
                    }

                    // 
                    ClientL.println("</body>");

                    // 
                    ClientL.println("</html>");
                    
                    // The HTTP response ends with another blank line
                    ClientL.println();

                    // Break out of the while loop
                    break;
                }
                else
                { // if you got a newline, then clear CurrentLineL
                    CurrentLineL = "";
                }
                }
                else if (CharacterL != '\r')
                {  // if you got anything else but a carriage return character,
                    CurrentLineL += CharacterL;      // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        Header_g = "";

        // Close the connection
        ClientL.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}

#pragma endregion