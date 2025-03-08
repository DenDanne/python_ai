#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

// WiFi credentials
const char *WIFI_SSID = "dendanne";
const char *WIFI_PASS = "hej12345";

// MQTT broker settings
const char *MQTT_SERVER = "broker.emqx.io";
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "projekt";

// WiFi and MQTT clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_PORT);
Adafruit_MQTT_Publish projekt_pub = Adafruit_MQTT_Publish(&mqtt, MQTT_TOPIC);


// Function to connect to MQTT broker with retries
void connectMQTT()
{
    Serial.print("Connecting to MQTT...");
    int8_t ret;
    while ((ret = mqtt.connect()) != 0)
    {
        Serial.println(mqtt.connectErrorString(ret));
        Serial.println("Retrying MQTT connection in 5 seconds...");
        delay(5000);
    }
    Serial.println("Connected to MQTT broker!");
}

// Function to ensure MQTT connection is active
void ensureMQTTConnection()
{
    if (!mqtt.connected())
    {
        Serial.println("MQTT disconnected! Reconnecting...");
        connectMQTT();
    }
}

// Function to publish messages with reconnection logic
bool publishMQTT(const char *message)
{
    ensureMQTTConnection();
    
    Serial.println("Publishing message to MQTT...");
    if (!projekt_pub.publish(message))
    {
        Serial.println("MQTT publish failed! Trying to reconnect...");
        connectMQTT();
        
        // Retry publishing after reconnect
        if (!projekt_pub.publish(message))
        {
            Serial.println("MQTT publish still failed after reconnect.");
            return false;
        }
    }

    Serial.println("MQTT publish successful!");
    return true;
}

// Function to test MQTT connection
void test_mqtt()
{
    publishMQTT("Hi");
}

// Function to send full_sample via MQTT
void sendMQTTData(float *full_sample, int size)
{
    Serial.println("Publishing full_sample data...");
    String payload = "[";

    for (int i = 0; i < size; i++)
    {
        payload += String(full_sample[i], 3);
        if (i < size - 1)
            payload += ",";
    }
    payload += "]";

    publishMQTT(payload.c_str());
}

// Function to maintain MQTT connection
void maintainMQTT()
{
    mqtt.processPackets(1000);
    if (!mqtt.ping())
    {
        Serial.println("MQTT ping failed! Reconnecting...");
        mqtt.disconnect();
        connectMQTT();
    }
}

#endif // MQTT_HANDLER_H
