#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h> 

#define DHTPIN 2        // GPIO2 tương ứng với D4
#define DHTTYPE DHT11  // Chọn loại cảm biến DHT11
DHT dht(DHTPIN, DHTTYPE);

int goc;
Servo myServo;

int servo = D0;
int ldrPin = A0;      // Chân analog A0 để kết nối với LDR
int ldrValue = 0;     // Biến để lưu giá trị đọc từ LDR

// Define pin connections for the three devices
#define LIGHT1_PIN 5   // GPIO5 tương ứng với D1 (Đèn)
#define LIGHT2_PIN 0   // GPIO0 tương ứng với D3 (Quạt)

#define LIGHT3_PIN 16   // GPIO16 tương ứng với D2 (Điều hòa)5
// #define SPEAKER_PIN 15 // Chọn GPIO15 cho loa

// --------
#define SS_PIN  D8  // Chân kết nối với SDA của module RFID
#define RST_PIN D2  // Chân kết nối với RST của module RFID


MFRC522 rfid(SS_PIN, RST_PIN);

// Danh sách các UID hợp lệ
const byte validUIDs[][4] = {
  {0x33, 0x54, 0x65, 0x28},  // Thẻ 1
  {0x63, 0xA0, 0xC6, 0x26},  // Thẻ 2
  {0x83, 0x10, 0x3E, 0x14}, 
  {0x23, 0x2C, 0x46, 0x14}
};

const int numCards = sizeof(validUIDs) / sizeof(validUIDs[0]); // Số lượng thẻ đã lưu
// ----------
bool ledState = LOW;  // Trạng thái hiện tại của đèn nhấp nháy

// bool manualControl = false;  // To track manual control of the light
// bool manualControl2 = false;

// Wi-Fi and MQTT information
const char* ssid = "hellocacban"; 
const char* password = "hicacban";
const char* mqtt_server = "192.168.1.107";  // Change this to your MQTT broker IP
const char* mqtt_username = "manh";
const char* mqtt_password = "123";

// Initialize WiFi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Function to connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Function to handle incoming MQTT messages
void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  Serial.println(message);

  // Control devices based on received MQTT message
  if (strcmp(topic, "home/devices/den") == 0) {
    // manualControl = true;  // Turn on manual control when receiving MQTT command
    if (message == "ON") {
      digitalWrite(LIGHT1_PIN, HIGH);
      Serial.println("LED turned ON");
      client.publish("home/devices/den/status", "ON");
    } else if (message == "OFF") {
      digitalWrite(LIGHT1_PIN, LOW);
      Serial.println("LED turned OFF");
      client.publish("home/devices/den/status", "OFF");
    }
  }
  if (strcmp(topic, "home/devices/quat") == 0) {
    if (message == "ON") {
      digitalWrite(LIGHT2_PIN, HIGH);
      Serial.println("Fan turned ON");
      client.publish("home/devices/quat/status", "ON");
    } else if (message == "OFF") {
      digitalWrite(LIGHT2_PIN, LOW);
      Serial.println("Fan turned OFF");
      client.publish("home/devices/quat/status", "OFF");
    }
  }
  if (strcmp(topic, "home/devices/dieuhoa") == 0) {
    // manualControl2 = true;
    if (message == "ON") {
      digitalWrite(LIGHT3_PIN, HIGH);
      Serial.println("Air Conditioner turned ON");
      client.publish("home/devices/dieuhoa/status", "ON");
    } else if (message == "OFF") {
      digitalWrite(LIGHT3_PIN, LOW);
      Serial.println("Air Conditioner turned OFF");
      client.publish("home/devices/dieuhoa/status", "OFF");
    }
  }
  // if (strcmp(topic, "home/devices/loa") == 0) {
  //   if (message == "ON") {
  //     digitalWrite(SPEAKER_PIN, HIGH);
  //     Serial.println("Louder Speaker turned ON");
  //     client.publish("home/devices/loa/status", "ON");
  //   } else if (message == "OFF") {
  //     digitalWrite(SPEAKER_PIN, LOW);
  //     Serial.println("Louder Speaker turned OFF");
  //     client.publish("home/devices/loa/status", "OFF");
  //   }
  // }
  // if (strcmp(topic, "home/devices/all") == 0) {
  //   if (message == "ON") {
  //     digitalWrite(LIGHT1_PIN, HIGH);
  //     digitalWrite(LIGHT2_PIN, HIGH);
  //     digitalWrite(LIGHT3_PIN, HIGH);
  //     client.publish("home/devices/den/status", "ON");
  //     client.publish("home/devices/quat/status", "ON");
  //     client.publish("home/devices/dieuhoa/status", "ON");
  //   } else if (message == "OFF") {
  //     digitalWrite(LIGHT1_PIN, LOW);
  //     digitalWrite(LIGHT2_PIN, LOW);
  //     digitalWrite(LIGHT3_PIN, LOW);
  //     client.publish("home/devices/den/status", "OFF");
  //     client.publish("home/devices/quat/status", "OFF");
  //     client.publish("home/devices/dieuhoa/status", "OFF");
  //   }
  // }
}

// Function to reconnect to the MQTT broker
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");

      // Subscribe to the topics for controlling devices
      client.subscribe("home/devices/den");
      client.subscribe("home/devices/quat");
      client.subscribe("home/devices/dieuhoa");
      client.subscribe("home/devices/loa");
      client.subscribe("home/devices/all");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1884);
  client.setCallback(callback);

  // Initialize DHT sensor
  dht.begin();

  //servo
  myServo.attach(servo); 

  // Set pin modes for the devices
  pinMode(LIGHT1_PIN, OUTPUT);  // Đèn
  pinMode(LIGHT2_PIN, OUTPUT);  // Quạt
  pinMode(LIGHT3_PIN, OUTPUT);  // Điều hòa
  // pinMode(SPEAKER_PIN, OUTPUT); // Loa


  // .......

  SPI.begin();        // Khởi tạo giao tiếp SPI
  rfid.PCD_Init();    // Khởi tạo module RFID
  digitalWrite(LIGHT1_PIN, LOW); // Mặc định tắt đèn
  Serial.println("Đưa thẻ RFID lên đầu đọc...");
  // >.........
}

void loop() {
  if (!client.connected()) {
    reconnect();  // Kết nối lại nếu cần
  }
  client.loop();  // Kiểm tra và xử lý các tin nhắn MQTT
  // Read sensor data
  ldrValue = analogRead(ldrPin);
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  float dash = random(0,1000);
  // Check if any reading failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // if (dash > 800) {
  //   ledState = !ledState;  // Chuyển đổi trạng thái LED nhấp nháy
  //   digitalWrite(SPEAKER_PIN, ledState);

  //   // Gửi trạng thái MQTT
  //   if (ledState) {
  //     client.publish("home/devices/loa/status", "ON");
  //     Serial.println("Cảnh báo nhấp nháy - Trạng thái: ON");
  //   } else {
  //     client.publish("home/devices/loa/status", "OFF");
  //     Serial.println("Cảnh báo nhấp nháy - Trạng thái: OFF");
  //   }
  // } else {
  //   digitalWrite(SPEAKER_PIN, LOW);  // Tắt LED nếu ánh sáng < 500
  //   client.publish("home/devices/loa/status", "OFF");
  //   Serial.println("Cảnh báo nhấp nháy - Trạng thái: OFF");
  // }

  // if (!manualControl && ldrValue > 500) {
  //   ledState = !ledState;  // Chuyển đổi trạng thái LED nhấp nháy
  //   digitalWrite(LIGHT1_PIN, ledState);

  //   // Gửi trạng thái MQTT
  //   if (ledState) {
  //     client.publish("home/devices/den/status", "ON");
  //     Serial.println("LED nhấp nháy - Trạng thái: ON");
  //   } else {
  //     client.publish("home/devices/den/status", "OFF");
  //     Serial.println("LED nhấp nháy - Trạng thái: OFF");
  //   }
  // } else if (!manualControl) {
  //   digitalWrite(LIGHT1_PIN, LOW);  // Tắt LED nếu ánh sáng < 500
  //   client.publish("home/devices/den/status", "OFF");
  //   Serial.println("LED nhấp nháy - Trạng thái: OFF");
  // }

  // if (!manualControl2){
  //   if (ldrValue > 700) {
  //     digitalWrite(SPEAKER_PIN, HIGH);  // Bật loa
  //     Serial.println("Loa đã được bật do ánh sáng lớn hơn 700.");
  //     client.publish("home/devices/loa/status", "ON");
  //   } else {
  //     digitalWrite(SPEAKER_PIN, LOW);   // Tắt loa
  //     Serial.println("Loa đã tắt do ánh sáng thấp hơn 700.");
  //     client.publish("home/devices/loa/status", "OFF");
  //   }
  // }





// ......
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID nhận được: ");
    
    for (int i = 0; i < rfid.uid.size; i++) {
      Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(rfid.uid.uidByte[i], HEX);
    }
    Serial.println();

    if (isValidCard(rfid.uid.uidByte, rfid.uid.size)) {
      Serial.println(" Thẻ hợp lệ! Bật đèn LED.");
      digitalWrite(LIGHT1_PIN, HIGH);
      
      Serial.println("LED turned ON");
      client.publish("home/devices/den/status", "ON");
      myServo.write(180);
      delay(5000);
      myServo.write(0);
    } else {
      Serial.println(" Thẻ không hợp lệ!");
      Serial.println("LED turned OFF");
      client.publish("home/devices/den/status", "OFF");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

// ........






  // Prepare JSON payload
  String payload = "{\"light\":";
  payload += String(ldrValue);
  payload += ",\"humidity\":";
  payload += String(humidity);
  payload += ",\"temperature\":";
  payload += String(temperature);
  // payload += ",\"Dash\":";
  // payload += String(dash);
  payload += "}";

  // Publish the sensor data to MQTT topic
  client.publish("home/sensors", payload.c_str());
  Serial.print("Published data: ");
  Serial.println(payload);

  // Delay between readings
  delay(2000);
}

// Hàm kiểm tra xem UID có nằm trong danh sách hợp lệ không
bool isValidCard(byte *uid, byte size) {
  for (int i = 0; i < numCards; i++) {
    bool match = true;
    for (int j = 0; j < size; j++) {
      if (uid[j] != validUIDs[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return true;
  }
  return false;
}