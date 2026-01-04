#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

const char* ssid = " wifi ";
const char* password = " ps ";

TinyGPSPlus gps;
HardwareSerial SerialGPS(2);
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 GPS Altitude Tracker</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css" />
    <script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script> <style>
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background: #eceff1; margin: 0; padding: 20px; }
        .container { max-width: 900px; margin: auto; background: white; padding: 20px; border-radius: 15px; box-shadow: 0 10px 25px rgba(0,0,0,0.1); }
        #map { height: 350px; width: 100%; border-radius: 10px; margin-top: 20px; }
        .chart-container { width: 100%; margin-top: 20px; height: 300px; }
        .stats { display: flex; justify-content: space-around; margin-bottom: 20px; flex-wrap: wrap; }
        .card { background: #0288d1; color: white; padding: 15px; border-radius: 10px; min-width: 120px; margin: 5px; }
        h2 { color: #333; }
    </style>
</head>
<body>
    <div class="container">
        <h2>Live GPS Altitude Tracker</h2>
        <div class="stats">
            <div class="card"><h3><span id="alt">0.0</span></h3><p>Altitude (m)</p></div>
            <div class="card"><h3><span id="sats">0</span></h3><p>Satellites</p></div>
            <div class="card"><h3><span id="lat">0.0</span></h3><p>Latitude</p></div>
            <div class="card"><h3><span id="lng">0.0</span></h3><p>Longitude</p></div>
        </div>

        <div class="chart-container">
            <canvas id="altitudeChart"></canvas>
        </div>

        <div id="map"></div>
    </div>

    <script>
        // Graph එක Initialize කිරීම
        var ctx = document.getElementById('altitudeChart').getContext('2d');
        var altitudeChart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'Altitude (Meters)',
                    data: [],
                    borderColor: '#0288d1',
                    backgroundColor: 'rgba(2, 136, 209, 0.2)',
                    fill: true,
                    tension: 0.4
                }]
            },
            options: {
                responsive: true,
                maintainAspectRatio: false,
                scales: { y: { beginAtZero: false } }
            }
        });

        // Map එක Initialize කිරීම
        var map = L.map('map').setView([0, 0], 2);
        L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png').addTo(map);
        var marker = L.marker([0, 0]).addTo(map);

        function updateData() {
            fetch('/data').then(response => response.json()).then(data => {
                if(data.sats > 0) {
                    document.getElementById('alt').innerHTML = data.alt.toFixed(1);
                    document.getElementById('sats').innerHTML = data.sats;
                    document.getElementById('lat').innerHTML = data.lat.toFixed(4);
                    document.getElementById('lng').innerHTML = data.lng.toFixed(4);
                    
                    // Map Update
                    var newPos = [data.lat, data.lng];
                    marker.setLatLng(newPos);
                    map.setView(newPos, 15);

                    // Graph Update
                    var now = new Date().toLocaleTimeString();
                    if (altitudeChart.data.labels.length > 20) { // අන්තිම point 20 විතරක් පෙන්වීමට
                        altitudeChart.data.labels.shift();
                        altitudeChart.data.datasets[0].data.shift();
                    }
                    altitudeChart.data.labels.push(now);
                    altitudeChart.data.datasets[0].data.push(data.alt);
                    altitudeChart.data.update();
                }
            });
        }

        setInterval(updateData, 3000); // තත්පර 3කට වරක් update වේ
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  SerialGPS.begin(9600, SERIAL_8N1, 16, 17);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(1000); Serial.println("Connecting..."); }
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    String json = "{";
    json += "\"alt\":" + String(gps.altitude.meters()) + ",";
    json += "\"lat\":" + String(gps.location.lat(), 6) + ",";
    json += "\"lng\":" + String(gps.location.lng(), 6) + ",";
    json += "\"sats\":" + String(gps.satellites.value());
    json += "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

void loop() {
  while (SerialGPS.available() > 0) {
    gps.encode(SerialGPS.read());
  }
}
