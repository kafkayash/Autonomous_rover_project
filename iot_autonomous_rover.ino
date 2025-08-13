#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <DHT.h>
#include <NewPing.h>
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "*****"; //place your wifi name
const char* password = "*****"; //place youe wifi password

WebServer server(80);
Adafruit_MPU6050 mpu;
DHT dht(13, DHT11);
Servo servo_motor;

// Motor pins
const int LeftMotorForward = 27;
const int LeftMotorBackward = 26;
const int RightMotorForward = 25;
const int RightMotorBackward = 33;
const int enA = 14;
const int enB = 32;

// Ultrasonic sensor pins
#define trig_pin 5
#define echo_pin 18
#define maximum_distance 200
NewPing sonar(trig_pin, echo_pin, maximum_distance);

// Variables
float ax, ay, az, vx = 0, vy = 0, vz = 0;
float temperature = 0, humidity = 0;
int distance = 0;
unsigned long prevTime = 0;
boolean goesForward = false;
float ax_offset = 0, ay_offset = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);

  server.begin();

  Wire.begin();
  mpu.begin();
  dht.begin();
  servo_motor.attach(19);
  servo_motor.write(115);
  float ax_sum = 0, ay_sum = 0, az_sum = 0;
  for (int i = 0; i < 100; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    ax_sum += a.acceleration.x;
    ay_sum += a.acceleration.y;
    az_sum += a.acceleration.z;
    delay(10);
  }
  ax_offset = ax_sum / 100.0;
  ay_offset = ay_sum / 100.0;

  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorForward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
}
void loop() {
  server.handleClient();
  distance = sonar.ping_cm();
  getMPUData();
  updateVelocity();
  handleObstacleAvoidance();
  delay(50);
}

void handleObstacleAvoidance() {
  if (distance <= 15) {
    moveStop();
    delay(300);
    moveBackward();
    delay(300);
    moveStop();
    delay(300);

    int distanceRight = lookRight();
    int distanceLeft = lookLeft();

    if (distanceRight >= distanceLeft) {
      turnRight();
      moveForward();
    } else {
      turnLeft();
      moveForward();
    }
  } else {
    moveForward();
  }
}
void moveForward() {
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}

void moveBackward() {
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
}

void turnRight() {
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  delay(300);
}

void turnLeft() {
  digitalWrite(RightMotorForward, HIGH);
  digitalWrite(LeftMotorBackward, HIGH);
  delay(300);
}

void moveStop() {
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}
int lookRight() {
  servo_motor.write(50);
  delay(300);
  int dist = sonar.ping_cm();
  servo_motor.write(115);
  return dist;
}
int lookLeft() {
  servo_motor.write(170);
  delay(300);
  int dist = sonar.ping_cm();
  servo_motor.write(115);
  return dist;
}
void getMPUData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  ax = a.acceleration.x - ax_offset;
  ay = a.acceleration.y - ay_offset;
  az = a.acceleration.z;
}

void updateVelocity() {
  unsigned long currentTime = millis();
  float deltaTime = (currentTime - prevTime) / 1000.0;
  prevTime = currentTime;

  if (abs(ax) > 0.1) vx += ax * deltaTime;
  if (abs(ay) > 0.1) vy += ay * deltaTime;
  if (abs(az) > 0.1) vz += az * deltaTime;
}
void handleData() {
  String data = "{";
  data += "\"distance\": " + String(distance) + ",";
  data += "\"temperature\": " + String(dht.readTemperature()) + ",";
  data += "\"humidity\": " + String(dht.readHumidity()) + ",";
  data += "\"vx\": " + String(vx) + ",";
  data += "\"vy\": " + String(vy) + ",";
  data += "\"vz\": " + String(vz) + ",";
  data += "\"ax\": " + String(ax) + ",";
  data += "\"ay\": " + String(ay) + ",";
  data += "\"az\": " + String(az);
  data += "}";
  server.send(200, "application/json", data);
}

void handleRoot() {
String page = "<html><body style='background-color:#2b2b2b; color:white; font-family:sans-serif;'>";
  page += "<div style='background-color:#d0312d; padding: 15px; text-align: center; margin-bottom: 20px;'>";
  page += "<h1>Rover Monitor</h1>";
  page += "</div>";
  page += "<div style='margin-bottom: 8px;'><span style='background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px; display: inline-block; min-width: 120px;'>Distance:</span> <span id='distance'>0</span> cm</div>";
  page += "<div style='margin-bottom: 8px;'><span style='background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px; display: inline-block; min-width: 120px;'>Temperature:</span> <span id='temperature'>0</span> &deg;C</div>";
  page += "<div style='margin-bottom: 8px;'><span style='background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px; display: inline-block; min-width: 120px;'>Humidity:</span> <span id='humidity'>0</span> %</div>";
  page += "<button onclick='toggleDetails()'>Toggle Details</button><div id='details' style='display:none;'>";
  page += "<div style='margin-bottom: 8px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Velocity X:</span> <span id='vx'>0</span> m/s</div>";
  page += "<div style='margin-bottom: 8px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Velocity Y:</span> <span id='vy'>0</span> m/s</div>";
  page += "<div style='margin-bottom: 16px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Velocity Z:</span> <span id='vz'>0</span> m/s</div>";
  page += "<div style='margin-bottom: 8px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Acceleration X:</span> <span id='ax'>0</span> m/s&sup2;</div>";
  page += "<div style='margin-bottom: 8px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Acceleration Y:</span> <span id='ay'>0</span> m/s&sup2;</div>";
  page += "<div style='margin-bottom: 8px;'><span style='display: inline-block; min-width: 120px; background-color: #d0312d; color: white; padding: 5px 10px; border-radius: 5px;'>Acceleration Z:</span> <span id='az'>0</span> m/s&sup2;</div>";
  page += "</div>";
  page += "<canvas id='chart' width='400' height='200'></canvas>";
  page += "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>";
  page += "<script>";
  page += "function toggleDetails() {";
  page += "  var details = document.getElementById('details');";
  page += "  details.style.display = (details.style.display === 'none') ? 'block' : 'none';";
  page += "}";
  page += "var ctx = document.getElementById('chart').getContext('2d');";
  page += "var chart = new Chart(ctx, { type: 'line', data: { labels: [], datasets: [";
  page += "{ label: 'Velocity X', data: [], borderColor: 'red', fill: false },";
  page += "{ label: 'Velocity Y', data: [], borderColor: 'blue', fill: false },";
  page += "{ label: 'Velocity Z', data: [], borderColor: 'green', fill: false },";
  page += "{ label: 'Acceleration X', data: [], borderColor: 'orange', fill: false },";
  page += "{ label: 'Acceleration Y', data: [], borderColor: 'purple', fill: false },";
  page += "{ label: 'Acceleration Z', data: [], borderColor: 'yellow', fill: false }";
  page += "] }, options: { scales: { y: { min: -50, max: 100 } } } });";

  page += "setInterval(function() { fetch('/data').then(res => res.json()).then(data => {";
  page += "var currentTime = new Date().toLocaleTimeString();";
  page += "chart.data.labels.push(currentTime);";
  page += "chart.data.datasets[0].data.push(data.vx);";
  page += "chart.data.datasets[1].data.push(data.vy);";
  page += "chart.data.datasets[2].data.push(data.vz);";
  page += "chart.data.datasets[3].data.push(data.ax);";
  page += "chart.data.datasets[4].data.push(data.ay);";
  page += "chart.data.datasets[5].data.push(data.az);";

  page += "document.getElementById('distance').innerText = data.distance;";
  page += "document.getElementById('temperature').innerText = data.temperature;";
  page += "document.getElementById('humidity').innerText = data.humidity;";
  page += "document.getElementById('vx').innerText = data.vx.toFixed(2);";
  page += "document.getElementById('vy').innerText = data.vy.toFixed(2);";
  page += "document.getElementById('vz').innerText = data.vz.toFixed(2);";
  page += "document.getElementById('ax').innerText = data.ax.toFixed(2);";
  page += "document.getElementById('ay').innerText = data.ay.toFixed(2);";
  page += "document.getElementById('az').innerText = data.az.toFixed(2);";

  page += "if (chart.data.labels.length > 20) { chart.data.labels.shift(); chart.data.datasets.forEach(dataset => dataset.data.shift()); }";
  page += "chart.update();";
  page += "}); }, 2000);";

  page += "</script></body></html>";
  server.send(200, "text/html", page);
}
