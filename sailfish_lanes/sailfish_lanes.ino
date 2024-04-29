#include <Arduino.h>
#include <TimeLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ESP8266WiFiMulti WiFiMulti;

// change these to correct values
const char* SSID = "sample";
const char* PASSWD = "pass";
String lane_number = "1";

const char* starterSetDateURI       = "http://10.3.141.1:8000/set_date";
const char* starterRaceStartTimeURI = "http://10.3.141.1:8000/race_start_time";
const char* starterResultsURI       = "http://10.3.141.1:8000/results";

// default values, get overwritten
String header = "Tue, 23 Apr 2024 16:55:33";
unsigned long lastTime = 0;
String starterWeekday = "Sun";
String starterMonth = "Jan";
String post_result;
String start_time;
const int buttonPin = 2; //D4
int lastButtonState = HIGH;

String httpPOST(const char* starterResultsURI, String result) {
  WiFiClient client;
  HTTPClient http;
  String httpCode;
  char json_blob[50];
  sprintf(json_blob, "{\"%s\": \"%s\"}", lane_number.c_str(), result.c_str());
  if (http.begin(client, starterResultsURI)) {
      http.addHeader("Content-Type", "plain/text");
      httpCode = http.POST(json_blob);
  } else {
   Serial.printf("[ERROR] ... failed POST/results\n");
  }
  http.end();
  return httpCode;
}

String httpGet(const char* starterRaceStartTimeURI) {
  WiFiClient client;
  HTTPClient http;
  String race_start_time;
  Serial.print("[INFO] in httpGet...\n");
  if (http.begin(client, starterRaceStartTimeURI)) {
    const char* headerNames[] = { "startTime" };
    http.collectHeaders(headerNames, sizeof(headerNames)/sizeof(headerNames[0]));
    int httpCode = http.sendRequest("HEAD");
    Serial.println(http.getString());

    if (httpCode > 0) {
      Serial.printf("[INFO] HEAD %s: %d\n", starterRaceStartTimeURI, httpCode);
      if (http.hasHeader("startTime")) {
        http.end();
        Serial.println(http.header("startTime"));
        return http.header("startTime");
        }
      }
    } else {
      Serial.printf("[ERROR] ... failed HEAD/RaceStartTime\n");
    }
}

time_t parseStartTime(const char* startTimeStr) {
  // pi zero 2 w
  const char* format = "%a, %d %b %Y %H:%M:%S";
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));
  if (strptime(startTimeStr, format, &tm) == NULL) {
    Serial.println("Error parsing start time!");
    return 0; // Return 0 to indicate an error
  }
  return mktime(&tm);
}

time_t parseHeaderDate(String header) {
  int year, month, day, hour, minute, second;
  char monthStr[3];
  // format is Fri, 16 Feb 2024 22:54:33
  Serial.println("[INFO] about to run parseHeaderDate...");

  sscanf(header.c_str(), "%3s, %d %3s %d %d:%d:%d", &starterWeekday, &day, &monthStr, &year, &hour, &minute, &second);
  Serial.println("[INFO] in successfully ran parseHeaderDate...");
  // Convert month string to a number
  const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  for (int i = 0; i < 12; ++i) {
    if (strcmp(monthStr, months[i]) == 0) {
      month = i + 1; // Month numbers are 1-based in TimeLib
      break;
    }
  }
  // Create a tm struct to hold the parsed values
  tm tm;
  tm.tm_year = year - 1900; // Years since 1900
  tm.tm_mon = month - 1;     // Months are 0-based in struct tm
  tm.tm_mday = day;
  tm.tm_hour = hour;
  tm.tm_min = minute;
  tm.tm_sec = second;
  
  // Convert tm struct to time_t
  return mktime(&tm);
}

void printParsedHeaderDate(time_t parsedHeaderDate) {
  // Convert parsedHeaderDate to a human-readable format
  struct tm * timeinfo;
  timeinfo = localtime(&parsedHeaderDate);
  
  // Format the time as a string
  char buffer[80];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
  
  // Print the parsed header date
  Serial.print("[INFO] setting time: ");
  Serial.println(buffer);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(SSID, PASSWD);

  while((WiFiMulti.run() != WL_CONNECTED)) {
    for (uint8_t t = 4; t > 0; t--) {
      Serial.printf("[INFO] waiting to connect %d...\n", t);
      Serial.flush();
      delay(1000);
    }
  
    if ((WiFiMulti.run() == WL_CONNECTED)) {
      Serial.printf("[INFO] wifi connected...\n");
      header = httpGet(starterSetDateURI);
      time_t parsedHeaderDate = parseHeaderDate(header);

      if (parsedHeaderDate != 1) {
        printParsedHeaderDate(parsedHeaderDate);
        setTime(parsedHeaderDate);
        Serial.printf("[INFO] ran setTime in setup.......\n");
      } else {
        Serial.printf("[ERROR] failed to run parseHeaderDate in setup...\n");
      }
    } else {
        Serial.printf("[HTTP} Unable to connect in setup\n");
    }
  }
  pinMode(buttonPin, INPUT);
  Serial.printf("[INFO] completed setup\n");
}

char* getElapsedTime(time_t startTime, time_t endTime) {
  // calc diff to hundredths
  int minutes, seconds;
  
  // Calculate the elapsed time in milliseconds
  unsigned long currentTime = millis();
  unsigned long hundredths = (currentTime / 10) % 100;
  
  unsigned long elapsedMilliseconds = (endTime - startTime) * 1000UL;
  minutes = elapsedMilliseconds / (60UL * 1000UL);
  elapsedMilliseconds %= (60UL * 1000UL);
  seconds = elapsedMilliseconds / 1000UL;
  
  // Print the elapsed time
  Serial.print("[INFO] Elapsed time: ");
  Serial.print(minutes);
  Serial.print(" minutes, ");
  Serial.print(seconds);
  Serial.print(" seconds, ");
  Serial.print(hundredths);
  Serial.println(" hundredths of a second");

  static char result[20];
  sprintf(result, "%d:%d:%d", minutes, seconds, hundredths);
  return result;
}

// create a default time which gets overwritten
String race_start_time = "Tue, 23 Apr 2024 17:00:33";
char* result;

void loop() {
  int buttonState = digitalRead(buttonPin);
    if (buttonState != lastButtonState) {
      Serial.printf("[INFO] Button pressed!\n");

      race_start_time = httpGet(starterRaceStartTimeURI);
      time_t start_time = parseHeaderDate(race_start_time);
      time_t end_time = now();
      printParsedHeaderDate(start_time);

      result = getElapsedTime(start_time, end_time);
      httpPOST(starterResultsURI, result);
      
      // make sure the loop doesn't catch same button press
      delay(10000);

      /////////////////////////////////////////////////
      // set the lane clock again, ensure it's in sync
      header = httpGet(starterSetDateURI);
      time_t parsedHeaderDate = parseHeaderDate(header);
      if (parsedHeaderDate != 1) {
        printParsedHeaderDate(parsedHeaderDate);
        setTime(parsedHeaderDate);
        Serial.printf("[INFO] ran setTime.......\n");
      } else {
        Serial.printf("[ERROR] failed to run parseHeaderDate...\n");
      }
  }
  delay(10);
  }
