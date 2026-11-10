
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <DHT.h>

const char* ssid = "ESPTEST";
const char* password = "12345678";

const char* serverURL =
"http://192.168.181.142:5000/api.py";m

#define DHTPIN D4
#define DHTTYPE DHT11

#define VibrationPIN D5
#define GasPIN A0
#define PressurePIN D7
#define RotatoryPIN D1

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(9600);

    dht.begin();

    pinMode(VibrationPIN, INPUT);
    pinMode(PressurePIN, INPUT);
    pinMode(RotatoryPIN, INPUT);

    WiFi.begin(ssid, password);

    Serial.println("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi Connected!");

    Serial.print("ESP8266 IP: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    int vibrationValue = digitalRead(VibrationPIN);
    int gasValue = analogRead(GasPIN);
    int pressureValue = digitalRead(PressurePIN);
    int rotationValue = digitalRead(RotatoryPIN);

    if (isnan(temperature) || isnan(humidity))
    {
        Serial.println("Failed to read DHT11");
        delay(2000);
        return;
    }

    Serial.println("--------------------");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Vibration: ");
    Serial.println(vibrationValue);

    Serial.print("Gas: ");
    Serial.println(gasValue);

    Serial.print("Pressure: ");
    Serial.println(pressureValue);

    Serial.print("Rotation: ");
    Serial.println(rotationValue);

    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;

        http.begin(client, serverURL);

        http.addHeader("Content-Type", "application/json");

        String jsonData = "{";

        jsonData += "\"temperature\":" + String(temperature, 1) + ",";
        jsonData += "\"humidity\":" + String(humidity, 1) + ",";
        jsonData += "\"vibration\":" + String(vibrationValue) + ",";
        jsonData += "\"gas\":" + String(gasValue) + ",";
        jsonData += "\"pressure\":" + String(pressureValue) + ",";
        jsonData += "\"rotation\":" + String(rotationValue);

        jsonData += "}";

        Serial.println("Sending data to Flask...");

        int httpCode = http.POST(jsonData);

        Serial.print("HTTP Response Code: ");
        Serial.println(httpCode);

        if (httpCode > 0)
        {
            String response = http.getString();

            Serial.print("Server Response: ");
            Serial.println(response);
        }
        else
        {
            Serial.println("Failed to connect to Flask");
            Serial.println(http.errorToString(httpCode));
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }

    delay(5000);
}
