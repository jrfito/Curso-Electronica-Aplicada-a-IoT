/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "studio24g"
#define STAPSK  "4977583973206"
#endif

#define LED_RED D2

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {
  Serial.begin(115200);

  // prepare LED
  pinMode(LED_RED, OUTPUT);
  digitalWrite(LED_RED, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("WiFi connected"));

  // Start the server
  server.begin();
  Serial.println(F("Server started"));

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println(F("new client"));

  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(F("request: "));
  Serial.println(req);

  // Match the request
  int val;
  if (req.indexOf(F("/gpio/0")) != -1) {
    val = 0;
  } else if (req.indexOf(F("/gpio/1")) != -1) {
    val = 1;
  } else {
    Serial.println(F("invalid request"));
    val = digitalRead(LED_RED);
  }

  // Set LED according to the request
  digitalWrite(LED_RED, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html lang='es'>"));
  client.print(F("<head>\r\n<meta charset='UTF-8'>\r\n<meta http-equiv='X-UA-Compatible' content='IE=edge'>\r\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\r\n<title>Electrónica Aplicada al Internet de las Cosas</title>"));
  client.println("<style> *,*::after,*::before {margin: 0; padding: 0; box-sizing: inherit;}");
  client.println("html {font-size: 62.5%;}");
  client.println("body { font-family: Lato, sans-serif; font-weight: 400; line-height: 1.7; color: #777; padding: 2rem; box-sizing: border-box;text-align: center; }");
  client.println(".btn:link,.btn:visited {background-color: rgb(133, 228, 154);color: #fff;display: inline-block;text-transform: uppercase;text-decoration: none;margin: 1rem;padding: 1.5rem 4rem;border-radius: 5rem;transition: all .2s;position: relative; font-size: 1.5rem;}");
  client.println(".btn:active{transform: translateY(-.1rem);box-shadow: 0 .5rem 1rem rgba(0,0,0.2);}");
  client.println(".btn::after{content: "";display: inline-block;height: 100%;width: 100%;border-radius: 5rem;position: absolute;top: 0;left: 0;z-index: -1;transition: all .4s;}");
  client.println(".btn:hover{background-color: rgb(74, 141, 89);color: #fff;opacity: 50%;}");
  client.println("p { text-transform: uppercase;}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body><h1>Electrónica Aplicada al Internet de las Cosas</h1><h2>Practica 6</h2><p>GPIO D2 = ");
  client.print((val) ? F("high") : F("low"));
  client.println("</p>");
  if(val){
    client.print("<a class='btn' href='http://");
    client.print(WiFi.localIP());
    client.print("/gpio/0'>Off</a>");
    }
  else {
    client.print("<a class='btn' href='http://");
    client.print(WiFi.localIP());
    client.print("/gpio/1'>On</a>");
    }    
    
  client.println("</body>");
  client.println("</html>");
  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
  Serial.println(F("Disconnecting from client"));
}
