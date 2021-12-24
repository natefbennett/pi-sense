#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"
#include "time.h"

// Replace with your network credentials (STATION)
const char* ssid        = "YOUR NETWORK NAME";
const char* password    = "YOUR PASSWORD";
const char* server_ip   = "FLASK SERVER IP";
const char* server_port = "5000";
const char* sensor_id   = "temp-humi-0";

#define DHTPIN 4                // digital pin connected to the DHT sensor
const int poll_rate = 60*15;    // sensor reading frequency in seconds

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

// time ntp server
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;
const int   daylightOffset_sec = 3600;

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

String getTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return "Null";
  }
  // YYYY-MM-DD hh:mm:ss
  char time_str[20];
  strftime(time_str, 20, "%Y-%m-%d+%H:%M:%S", &timeinfo);
  return String(time_str);
}

void postHumidityData(float humid) {

    WiFiClient client;
    HTTPClient http;
    String humidity_endpoint = "http://" + String(server_ip)   + ":" 
                                         + String(server_port) + "/api/humidity";
    http.begin(client, humidity_endpoint);
  
    // build data string
    String data = "sensor_id=" + String( sensor_id ) + 
                  "&time="     + String( getTime() ) + 
                  "&value="    + String( (int)humid );

    // add headers
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    Serial.println("sending humidity data -> " + data);
    int httpResponseCode = http.POST(data);
    Serial.println("response code: " + String(httpResponseCode));
    
    http.end();
}

void postTemperatureData(float temp) {

    WiFiClient client;
    HTTPClient http;
    String temperature_endpoint = "http://" + String(server_ip)   + ":" 
                                            + String(server_port) + "/api/temperature";
    http.begin(client, temperature_endpoint);
  
    // build data string
    String data = "sensor_id=" + String( sensor_id ) + 
                  "&time="     + String( getTime() ) + 
                  "&value="    + String( (int)temp );

    // add headers
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    Serial.println("sending temperature data -> " + data);
    int httpResponseCode = http.POST(data);
    Serial.println("response code: " + String(httpResponseCode));
    
    http.end();
}

void setup() {
    // bring up wifi   
    Serial.begin(115200);
    initWiFi();
    Serial.print("RRSI (signal strength): ");
    Serial.println(WiFi.RSSI());

    // Init and get the time 
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    Serial.println(getTime());

    // bring up sensor
    Serial.println(F("Starting sensor..."));
    dht.begin();
}

void loop() {

    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();

    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(f)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }
    
    postHumidityData(h);
    postTemperatureData(f);

    delay(1000*poll_rate);
}