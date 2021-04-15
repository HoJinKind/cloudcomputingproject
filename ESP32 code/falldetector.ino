#include <ArduinoJson.h>
#include <AWS_IOT.h>
#include <WiFi.h>
#include <Wire.h>

AWS_IOT hornbill;

// WIFI settings
char WIFI_SSID[] = "****";
char WIFI_PASSWORD[] = "****";

// Thing details
char HOST_ADDRESS[] = "aau9wqdiasibl-ats.iot.ap-southeast-1.amazonaws.com";
char CLIENT_ID[] = "ESP32";

// Topic names
char TOPIC_NAME_ACTIVE[] = "cloudcomputing/active";
char TOPIC_NAME_ACCESS[] = "cloudcomputing/access";
char TOPIC_NAME_AG[] = "cloudcomputing/accessgranted";
char TOPIC_NAME_FALL[] = "cloudcomputing/deviceFall";

char MSG_ACTIVE[] = "{\"id\":\"1\",\"status\":\"on\"}";
char MSG_FALL[] = "{\"id\":\"1\",\"status\":\"fall\"}";

int status = WL_IDLE_STATUS;
int msgReceived = 0;
char payload[512];
char rcvdPayload[512];

const int MPU_addr = 0x68; // I2C address of the MPU-6050
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

boolean fall = false;
int angleChange = 0;

const int ledPin = 23; // the number of the LED pin
const int buttonPin = 2; // the number of the Button pin

void mySubCallBackHandler(char * topicName, int payloadLen, char * payLoad) {
    strncpy(rcvdPayload, payLoad, payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}

void mpu_read() {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
    AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    pinMode(ledPin, OUTPUT);

    // Connect to wifi
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(WIFI_SSID);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    // wait 5 seconds for connection:
    delay(5000);
    Serial.println("Connected to wifi");

    // Connect to aws
    if (hornbill.connect(HOST_ADDRESS, CLIENT_ID) == 0) {
        Serial.println("Connected to AWS");
        delay(1000);

        // Publish to cc/active
        int activated = 0;
        while (activated == 0) {
            if (hornbill.publish(TOPIC_NAME_ACTIVE, MSG_ACTIVE) == 0) {
                activated = 1;
                Serial.println("Published to active");
            } else {
                Serial.println("Publish to active fail try again");
                delay(1000);
            }
        }

        // Subscribe to cc/access
        if (0 == hornbill.subscribe(TOPIC_NAME_ACCESS, mySubCallBackHandler)) {
            Serial.println("Subscribe Successfull");
        } else {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while (1);
        }

        // Wait for message from access
        while (msgReceived == 0) {
            Serial.println("waiting for telegram request");
            delay(1000);
        }

        Serial.print("Received Message:");
        Serial.println(rcvdPayload);

        digitalWrite(ledPin, HIGH);
        while (digitalRead(buttonPin) == LOW);

        // Publish to ag
        int ag = 0;
        while (ag == 0) {
            if (hornbill.publish(TOPIC_NAME_AG, rcvdPayload) == 0) {
                ag = 1;
                Serial.println("Published to access granted");
            } else {
                Serial.println("Publish to ag failed try again");
                delay(1000);
            }
        }

        digitalWrite(ledPin, LOW);
    } else {
        Serial.println("AWS connection failed, Check the HOST Address");
        while (1);
    }
    delay(2000);

    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B); // PWR_MGMT_1 register
    Wire.write(0); // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);

}

void loop() {
    mpu_read();
    //2050, 77, 1947 are values for calibration of accelerometer
    // values may be different for you
    ax = (AcX - 2050) / 16384.00;
    ay = (AcY - 77) / 16384.00;
    az = (AcZ - 1947) / 16384.00;

    //270, 351, 136 for gyroscope
    gx = (GyX + 270) / 131.07;
    gy = (GyY - 351) / 131.07;
    gz = (GyZ + 136) / 131.07;

    // calculating Amplitute vactor for 3 axis
    float Raw_AM = pow(pow(ax, 2) + pow(ay, 2) + pow(az, 2), 0.5);
    int AM = Raw_AM * 10;
    Serial.println(AM);
    //Serial.println(PM);
    //delay(500);

    if (trigger3 == true) {
        trigger3count++;
        //Serial.println(trigger3count);
        if (trigger3count >= 10) {
            angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
            //delay(10);
            Serial.println(angleChange);
            if ((angleChange >= 0) && (angleChange <= 10)) { //if orientation changes remains between 0-10 degrees
                fall = true;
                trigger3 = false;
                trigger3count = 0;
                Serial.println(angleChange);
            } else { //user regained normal orientation
                trigger3 = false;
                trigger3count = 0;
                Serial.println("TRIGGER 3 DEACTIVATED");
            }
        }
    }
    if (fall == true) { //in event of a fall detection
        Serial.println("FALL DETECTED");
        // Publish to fall
        int f = 0;
        while (f == 0) {
            if (hornbill.publish(TOPIC_NAME_FALL, MSG_FALL) == 0) {
                f = 1;
                Serial.println("Published to fall");
            } else {
                Serial.println("Publish to fall failed try again");
                delay(1000);
            }
        }
        fall = false;
        delay(10000);
    }
    if (trigger2count >= 6) { //allow 0.5s for orientation change
        trigger2 = false;
        trigger2count = 0;
        Serial.println("TRIGGER 2 DECACTIVATED");
    }
    if (trigger1count >= 6) { //allow 0.5s for AM to break upper threshold
        trigger1 = false;
        trigger1count = 0;
        Serial.println("TRIGGER 1 DECACTIVATED");
    }
    if (trigger2 == true) {
        trigger2count++;
        angleChange = pow(pow(gx, 2) + pow(gy, 2) + pow(gz, 2), 0.5);
        Serial.println(angleChange);
        if (angleChange >= 30 && angleChange <= 400) { //if orientation changes by between 80-100 degrees
            trigger3 = true;
            trigger2 = false;
            trigger2count = 0;
            Serial.println(angleChange);
            Serial.println("TRIGGER 3 ACTIVATED");
        }
    }
    if (trigger1 == true) {
        trigger1count++;
        if (AM >= 12) { //if AM breaks upper threshold (3g)
            trigger2 = true;
            Serial.println("TRIGGER 2 ACTIVATED");
            trigger1 = false;
            trigger1count = 0;
        }
    }
    if (AM <= 2 && trigger2 == false) { //if AM breaks lower threshold (0.4g)
        trigger1 = true;
        Serial.println("TRIGGER 1 ACTIVATED");
    }
    //It appears that delay is needed in order not to clog the port
    delay(100);
}