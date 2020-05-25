//https://github.com/prampec/IotWebConf

#include <IotWebConf.h>
#include <FastLED.h>

//---Variables for Server Config --------------------------------------------------------------------------------
// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "AbeBlinkin";
// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "smrtTHNG8266";

DNSServer dnsServer;
String header;
WebServer server(80);

#define STATUS_PIN LED_BUILTIN

// -- Callback method declarations.
void wifiConnected();

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

//---Variables for FASTLED Control ------------------------------------------------------------------------------
#define LED_PIN     1
#define NUM_LEDS    50
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB

CRGB leds[NUM_LEDS];

//---Variables for Lighting Control -----------------------------------------------------------------------------
// Decode HTTP GET value
String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

//---Variables for Pattern Management ---------------------------------------------------------------------------
bool patternMode = false;
int pattern = 0;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  setAll(0x00,0x00,0x00);

  // -- Initializing the configuration.
  iotWebConf.setStatusPin(STATUS_PIN);
  iotWebConf.setWifiConnectionCallback(&wifiConnected);
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.on("/color", handlePicker);
  server.on("/pattern", handlePattern);
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");

}

void loop() {
  // put your main code here, to run repeatedly:
  iotWebConf.doLoop();
  if(patternMode){
    switch(pattern){
      case 0:
        setAll(0x00,0x00,0x00);
        patternMode = false;
      break;
      case 1:
        CylonBounce(0xff, 0, 0, 4, 10, 50);
      break;
      case 2:
        TwinkleRandom(20, 100, false);
      break;
      case 3:
        Sparkle(random(255), random(255), random(255), 0);
      break;
    }
  }
}

void wifiConnected()
{
  Serial.println("WiFi was connected.");
  colorWipe(0x00,0xFF, 0x00, 50);
  colorWipe(0x00,0x00, 0x00, 50);
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title>";
  s += "</head><body><div class=\"container\"><div class=\"row\"><h1>Abe-Blinkin Light Controller</h1></div>";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "Go to <a href='color'>color page</a> to change colors.";
  s += "Go to <a href='pattern'>pattern page</a> to select a color pattern.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

void handlePicker()
{
  //Shut down Pattern Mode and go dark.
  pattern = 0;
  
  String s = "<!DOCTYPE html><html>";
  s += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<link rel=\"icon\" href=\"data:,\">";
  s += "<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">";
  s += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>";
  s += "</head><body><div class=\"container\"><div class=\"row\"><h1>ESP Color Picker</h1></div>";
  s += "<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change Color</a> ";
  s += "<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>";
  s += "<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);";
  s += "document.getElementById(\"change_color\").href=\"?rgb=r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"z&\";}</script></body></html>";
  // The HTTP response ends with another blank line
  server.send(200, "text/html", s);

  header = "";
  for(int a=0; a<server.args(); a++){
    Serial.print(server.argName(a));
    Serial.print(":");
    Serial.println(server.arg(server.argName(a)));

    header = server.arg(server.argName(a));
    if(server.argName(a) == "rgb"){
      pos1 = header.indexOf('r');
      pos2 = header.indexOf('g');
      pos3 = header.indexOf('b');
      pos4 = header.indexOf('z');
      redString = header.substring(pos1+1, pos2);
      greenString = header.substring(pos2+1, pos3);
      blueString = header.substring(pos3+1, pos4);
      Serial.print(redString);
      Serial.print(":");
      Serial.print(greenString);
      Serial.print(":");
      Serial.println(blueString);

      byte rByte = redString.toInt();
      byte gByte = greenString.toInt();
      byte bByte = blueString.toInt();

      colorWipe(rByte,gByte,bByte, 50);
    }
  }
}

void handlePattern(){
  String s = "<!DOCTYPE html>";
  s += "<html>";
  s += "<body>";
  s += "<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">";
  s += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>";
  s += "<p>Select a light pattern from the list.</p>";
  s += "<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_pattern\" role=\"button\">Change Pattern</a> ";
  s += "<select id=\"mySelect\" onchange=\"myFunction(this)\">";
  s += "  <option value=\"0\">Select Pattern</option>";
  s += "  <option value=\"1\">Cylon</option>";
  s += "  <option value=\"2\">Twinkle</option>";
  s += "  <option value=\"3\">Sparkle</option>";
  s += "  <option value=\"0\">Off</option>";
  s += "</select>";

  s += "<script>";
  s += "function myFunction(selectObject) {";
  s += "  var value = selectObject.value;";
  s += "  document.getElementById(\"change_pattern\").href=\"?pattern=\" + value + \"&\";";
  s += "}";
  s += "</script>";
  
  s += "</body>";
  s += "</html>";
  server.send(200, "text/html", s);
  
  header = "";
  for(int a=0; a<server.args(); a++){
    Serial.print(server.argName(a));
    Serial.print(":");
    Serial.println(server.arg(server.argName(a)));
    pattern = server.arg(server.argName(a)).toInt();
    patternMode = true;
  }
}

void CylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }

  delay(ReturnDelay);

  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
 
  delay(ReturnDelay);
}

void TwinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);
 
  for (int i=0; i<Count; i++) {
     setPixel(random(NUM_LEDS),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }
 
  delay(SpeedDelay);
}

void Sparkle(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(NUM_LEDS);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for(uint16_t i=0; i<NUM_LEDS; i++) {
      setPixel(i, red, green, blue);
      showStrip();
      delay(SpeedDelay);
  }
}

void showStrip() {
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}
