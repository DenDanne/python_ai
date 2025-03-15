#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

#define LED_PIN 13

// WiFi credentials
const char *WIFI_SSID = "dendanne";
const char *WIFI_PASS = "hej12345";

// Server settings
//const char *SERVER_URL = "http://192.168.0.104:5000/api/data";
const char *SERVER_URL = "https://ed94-193-181-34-101.ngrok-free.app/api/data";

// Function to connect to WiFi
void connectWiFi()
{
    Serial.println("Disconnecting from WiFi...");
    WiFi.disconnect(true);
    delay(1000);

    Serial.print("Connecting to WiFi...");
    Serial.println(SERVER_URL);

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) // 15 seconds timeout
    {
        delay(500);
        Serial.print(".");
        attempts++;
        digitalWrite(LED_PIN, 0); 
    }

    if (WiFi.status() == WL_CONNECTED)
    {
        digitalWrite(LED_PIN, 1); 
        Serial.println("\nConnected to WiFi!");
    }
    else
    {
        Serial.println("\nFailed to connect to WiFi. Restarting...");
        ESP.restart();
    }
}


WiFiClient client;  // Global client object
HTTPClient http;

void sendHTTP(const char* payload) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi not connected. Skipping HTTP send.");
        return;
    }

    http.begin(client, SERVER_URL);  // Use persistent connection
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        // Uncomment for debugging:
        // Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
    } else {
        Serial.print("HTTP Error: "); Serial.println(httpResponseCode);
    }

    http.end();  // Close connection, but keep client open for next request
}

void sendHTTPDataFaster(float *full_sample, int size)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, 0); 
        Serial.println("WiFi not connected! Reconnecting...");
        connectWiFi();
        return;
    }

    WiFiClientSecure client; 
    client.setInsecure();    

    HTTPClient http;  

    http.begin(client, SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String payload = "[";
    for (int i = 0; i < size; i++)
    {
        payload += String(full_sample[i], 3);
        if (i < size - 1) payload += ",";
    }
    payload += "]";

    Serial.println("Sending JSON: " + payload);

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) 
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println("Server Response: " + response);
        digitalWrite(LED_PIN, 1); 
    } 
    else 
    {
        Serial.print("HTTP Request failed. Error: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}


/*void sendHTTPDataFaster(float *full_sample, int size)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        digitalWrite(LED_PIN, 0); 
        Serial.println("WiFi not connected! Reconnecting...");
        connectWiFi();
        return;
    }

   // HTTPClient http;
    client.setInsecure();

    http.begin(client, SERVER_URL);  // Reuse connection
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String payload = "[";
    for (int i = 0; i < size; i++)
    {
        payload += String(full_sample[i], 3);
        if (i < size - 1) payload += ",";
    }
    payload += "]";

   // Serial.println("Sending data...");
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String response = http.getString();
        Serial.println(response);
        digitalWrite(LED_PIN, 1); 
    } 
    else 
    {
        Serial.print("HTTP Request failed. Error: ");
        Serial.println(httpResponseCode);
    }

    //digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    http.end();  // Keep connection open
}*/

// Function to send sensor data via HTTP
bool sendHTTPData(float *full_sample, int size)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected! Reconnecting...");
        connectWiFi();
        return false;
    }

   // HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    // Create JSON payload
    String payload = "[";
    for (int i = 0; i < size; i++)
    {
        payload += String(full_sample[i], 6);
        if (i < size - 1) payload += ",";
    }
    payload += "]";

    Serial.println("Sending data to server: " + payload);
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        http.end();
        return true;
    }
    else
    {
        Serial.print("HTTP Request failed: ");
        Serial.println(httpResponseCode);
        http.end();
        return false;
    }
}

void test_http()
{
    Serial.println("Sending test HTTP message...");

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi not connected! Reconnecting...");
        connectWiFi();
    }

  //  HTTPClient http;
    http.begin(SERVER_URL);  // Replace with your server IP
    http.addHeader("Content-Type", "application/json");

    // Test message
    String payload = "[1.23, 4.56, 7.89]";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
    }
    else
    {
        Serial.print("HTTP Request failed: ");
        Serial.println(httpResponseCode);
    }

    http.end();
}


#endif // HTTP_HANDLER_H
