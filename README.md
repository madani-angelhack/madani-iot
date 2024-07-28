# MADANI IoT System with Firebase Integration

This project integrates Firebase with an ESP32 to monitor temperature, humidity, gas leaks, and flood status. It controls a relay and buzzer based on sensor readings and Firebase Realtime Database values.

## Components Used

- ESP32
- DHT11 Temperature and Humidity Sensor
- MQ2 Gas Sensor
- Relay Module
- Buzzer
- LiquidCrystal I2C Display
- Firebase Realtime Database

## Wiring Diagram

| Component      | ESP32 Pin |
| -------------- | --------- |
| DHT11 Data Pin | GPIO 13   |
| MQ2 Data Pin   | GPIO 12   |
| Relay IN Pin   | GPIO 19   |
| Buzzer Pin     | GPIO 4    |
| LCD SDA        | SDA       |
| LCD SCL        | SCL       |

## Setup

1. **Install Libraries**

   Ensure you have the following libraries installed in your Arduino IDE:

   - `Firebase ESP Client` by Mobizt
   - `DHT sensor library` by Adafruit
   - `Adafruit Unified Sensor` by Adafruit
   - `LiquidCrystal I2C` by Frank de Brabander

2. **Configure Firebase**

   - Create a Firebase project in the [Firebase Console](https://console.firebase.google.com/).
   - Add your ESP32 device to the project.
   - Get the Firebase Realtime Database URL and API key.
   - Enable Email/Password authentication in the Firebase console.

3. **Update WiFi and Firebase Credentials**

   Update the `WIFI_SSID`, `WIFI_PASSWORD`, `API_KEY`, and `DATABASE_URL` in the sketch with your credentials.

4. **Upload the Code**

   Upload the provided code to your ESP32 using the Arduino IDE.

## Code Explanation

The provided code performs the following tasks:

1. **Initialize Sensors and Modules**

   Initializes the DHT11 sensor, MQ2 gas sensor, relay, buzzer, and LCD display.

2. **Connect to WiFi**

   Connects the ESP32 to the specified WiFi network.

3. **Initialize Firebase**

   Authenticates with Firebase and sets up data streams for monitoring relay and flood status.

4. **Loop Function**

   The `loop()` function continuously:
   - Reads temperature and humidity from the DHT11 sensor and updates Firebase.
   - Checks the status of the MQ2 gas sensor and updates Firebase.
   - Reads the flood status from Firebase and controls the relay and buzzer accordingly.
   - Reads the siren status from Firebase and controls the relay and buzzer accordingly.

## Functions

- `tempHumiMeasurement()`: Reads temperature and humidity from the DHT11 sensor and updates Firebase.
- `leakGasDetection()`: Reads the gas level from the MQ2 sensor and updates Firebase.
- `updateFloodStatus()`: Reads the flood status from Firebase and controls the relay and buzzer.
- `updateSirensStatus()`: Reads the siren status from Firebase and controls the relay and buzzer.

## Firebase Database Structure

### Sensor Data

```json
[
  {
    "id": "1",
    "lat": -6.294068,
    "lon": 106.784913,
    "relay": {
      "sirens": "OFF"
    },
    "sensor": {
      "flood": "OFF",
      "gas": 0,
      "humidity_data": 59,
      "is_danger": false,
      "temp_data": 22.2
    }
  }
]
```

### Help Center Data
```json
[
  {
    "id": "1",
    "lat": "-6.289000,",
    "lon": 106.775183,
    "name": "Pemadam Kebakaran Lebak Bulus"
  },
  {
    "id": "1",
    "lat": "0",
    "lon": "0",
    "name": "Polsek Cilandak"
  }
]
```
