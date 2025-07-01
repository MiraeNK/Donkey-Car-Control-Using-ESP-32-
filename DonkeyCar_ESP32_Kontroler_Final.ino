#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>


const char* ap_ssid = "ESP32-Mobil-RC";       
const char* ap_password = "password123";      

// Konfigurasi alamat IP statis untuk ESP32
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
// --------------------------------------------------------

// create WebServer object on port 80 (HTTP)
WebServer server(80);

// Create object Servo for ESC (throttle) and Servo (steering)
Servo esc;
Servo steeringServo;

// --- Pinout Configuration ---
const int ESC_PIN = 22;
const int STEERING_PIN = 21;

// --- Throttle ESC Configuration ---
const int NEUTRAL_THROTTLE = 1500;
const int MAX_FORWARD_THROTTLE = 2000;
const int MAX_REVERSE_THROTTLE = 1000;

// --- Steering Servo Angle Configuration (may need to be adjusted) ---
const int STEERING_CENTER = 90;  
const int STEERING_LEFT = 45;    
const int STEERING_RIGHT = 135;  


int currentThrottlePercentage = 50; // Default Throttle 50%

// == WEB PAGE CODE (HTML, CSS, JavaScript) ==
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 Web Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <style>
    body { font-family: Arial, sans-serif; text-align: center; background-color: #2c3e50; color: white; touch-action: none; padding: 10px; }
    h1 { margin-bottom: 10px; }
    .container { display: flex; flex-direction: column; align-items: center; justify-content: center; }
    .btn { background-color: #3498db; border: none; color: white; padding: 25px 50px; text-align: center; font-size: 20px; margin: 10px; cursor: pointer; border-radius: 12px; width: 180px; -webkit-user-select: none; user-select: none; }
    .btn-reverse { background-color: #e74c3c; }
    .btn-steer { background-color: #f1c40f; color: #2c3e50; }
    .btn:active { transform: scale(0.95); }
    .grid-container { display: grid; grid-template-columns: 1fr 1fr; grid-gap: 10px; margin-top: 20px; }
    .slider-container { margin-top: 20px; width: 90%; max-width: 300px; }
    #speed-slider { width: 100%; }
    #speed-label { font-size: 18px; margin-top: 5px; }
  </style>
</head>
<body>
  <h1>ESP32 RC Car Controller</h1>
  <div class="container">
    <button id="btn-maju" class="btn">MAJU</button>
    <div class="grid-container">
      <button id="btn-kiri" class="btn btn-steer">KIRI</button>
      <button id="btn-kanan" class="btn btn-steer">KANAN</button>
    </div>
    <button id="btn-mundur" class="btn btn-reverse">MUNDUR</button>
    <div class="slider-container">
      <label id="speed-label" for="speed-slider">Kecepatan: <span id="speed-value">50</span>%</label>
      <input type="range" min="20" max="100" value="50" class="slider" id="speed-slider">
    </div>
  </div>
  <script>
    function sendCommand(command) { fetch('/' + command).catch(error => console.error('Error:', error)); }
    function sendSpeed(speed) { fetch('/setSpeed?value=' + speed).catch(error => console.error('Error:', error)); }
    const majuButton = document.getElementById('btn-maju');
    const mundurButton = document.getElementById('btn-mundur');
    const kiriButton = document.getElementById('btn-kiri');
    const kananButton = document.getElementById('btn-kanan');
    const speedSlider = document.getElementById('speed-slider');
    const speedValue = document.getElementById('speed-value');
    function stopThrottle() { sendCommand('berhenti'); }
    function centerSteering() { sendCommand('lurus'); }
    majuButton.addEventListener('mousedown', () => sendCommand('maju'));
    majuButton.addEventListener('touchstart', (e) => { e.preventDefault(); sendCommand('maju'); });
    mundurButton.addEventListener('mousedown', () => sendCommand('mundur'));
    mundurButton.addEventListener('touchstart', (e) => { e.preventDefault(); sendCommand('mundur'); });
    majuButton.addEventListener('mouseup', stopThrottle);
    majuButton.addEventListener('mouseleave', stopThrottle);
    majuButton.addEventListener('touchend', stopThrottle);
    mundurButton.addEventListener('mouseup', stopThrottle);
    mundurButton.addEventListener('mouseleave', stopThrottle);
    mundurButton.addEventListener('touchend', stopThrottle);
    kiriButton.addEventListener('mousedown', () => sendCommand('kiri'));
    kiriButton.addEventListener('touchstart', (e) => { e.preventDefault(); sendCommand('kiri'); });
    kananButton.addEventListener('mousedown', () => sendCommand('kanan'));
    kananButton.addEventListener('touchstart', (e) => { e.preventDefault(); sendCommand('kanan'); });
    kiriButton.addEventListener('mouseup', centerSteering);
    kiriButton.addEventListener('mouseleave', centerSteering);
    kiriButton.addEventListener('touchend', centerSteering);
    kananButton.addEventListener('mouseup', centerSteering);
    kananButton.addEventListener('mouseleave', centerSteering);
    kananButton.addEventListener('touchend', centerSteering);
    speedSlider.addEventListener('input', () => {
      const newSpeed = speedSlider.value;
      speedValue.textContent = newSpeed;
      sendSpeed(newSpeed);
    });
  </script>
</body>
</html>
)rawliteral";


// == HANDLER FUNCTIONS FOR WEB SERVER ==

void handleRoot() { server.send(200, "text/html", index_html); }
void handleNotFound() { server.send(404, "text/plain", "404: Not Found"); }
void handleMaju() { long forwardValue = map(currentThrottlePercentage, 0, 100, NEUTRAL_THROTTLE, MAX_FORWARD_THROTTLE); esc.writeMicroseconds(forwardValue); server.send(200, "text/plain", "Maju"); }
void handleMundur() { long reverseValue = map(currentThrottlePercentage, 0, 100, NEUTRAL_THROTTLE, MAX_REVERSE_THROTTLE); esc.writeMicroseconds(reverseValue); server.send(200, "text/plain", "Mundur"); }
void handleBerhenti() { esc.writeMicroseconds(NEUTRAL_THROTTLE); server.send(200, "text/plain", "Berhenti"); }
void handleKiri() { steeringServo.write(STEERING_LEFT); server.send(200, "text/plain", "Kiri"); }
void handleKanan() { steeringServo.write(STEERING_RIGHT); server.send(200, "text/plain", "Kanan"); }
void handleLurus() { steeringServo.write(STEERING_CENTER); server.send(200, "text/plain", "Lurus"); }
void handleSetSpeed() { if (server.hasArg("value")) { int speed = server.arg("value").toInt(); if (speed >= 20 && speed <= 100) { currentThrottlePercentage = speed; Serial.print("Kecepatan diatur ke: "); Serial.println(currentThrottlePercentage); server.send(200, "text/plain", "Speed Set"); } else { server.send(400, "text/plain", "Invalid Speed Value"); } } else { server.send(400, "text/plain", "No Speed Value"); } }

void setup() {
  Serial.begin(115200);

  // --- Setup ESC dan Servo ---
  esc.attach(ESC_PIN);
  steeringServo.attach(STEERING_PIN);

  Serial.println("Arming ESC & Centering Steering...");
  esc.writeMicroseconds(NEUTRAL_THROTTLE);
  steeringServo.write(STEERING_CENTER);
  delay(3000);
  Serial.println("Sistem Siap!");

  // --- Access Point (Hotspot) ---
  Serial.print("Membuat Access Point dengan nama: ");
  Serial.println(ap_ssid);
  
  // Configure static IP and start Access Point
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ap_ssid, ap_password);

  Serial.print("AP Berhasil dibuat. Alamat IP untuk kontroler: http://");
  Serial.println(WiFi.softAPIP()); // Cetak IP address AP

  // --- Setup Web Server Handlers 
  server.on("/", HTTP_GET, handleRoot);
  server.on("/maju", HTTP_GET, handleMaju);
  server.on("/mundur", HTTP_GET, handleMundur);
  server.on("/berhenti", HTTP_GET, handleBerhenti);
  server.on("/kiri", HTTP_GET, handleKiri);
  server.on("/kanan", HTTP_GET, handleKanan);
  server.on("/lurus", HTTP_GET, handleLurus);
  server.on("/setSpeed", HTTP_GET, handleSetSpeed);
  server.onNotFound(handleNotFound);

  // Start web server
  server.begin();
  Serial.println("Web server dimulai.");
}

void loop() {
  server.handleClient();
}