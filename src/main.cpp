
#include <ESPmDNS.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include <Adafruit_Sensor.h>

//Librerias para dht11
#include <DHT.h>
#include <DHT_U.h>

//Variables para DHT11
#define DHT_PIN 2
#define DHT_TYPE DHT11

DHT_Unified dht(DHT_PIN, DHT_TYPE);

float humedad = 0;
float temperatura = 0;

float *obtenerHumedadTemperatura();
//Fin de variables para DHT11

const int trigPin = 5;
const int echoPin = 18;
#define SOUND_SPEED 0.034
long duration;
float distanceCm;
const char* ssid = "Deiv";
const char* password = "987654321"; 
AsyncWebServer server(80);
String readDistance(){
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  duration = pulseIn(echoPin, HIGH);
  
  distanceCm = duration * SOUND_SPEED/2;
  Serial.println("Distance:");
Serial.println(distanceCm);
   return String(distanceCm);
   

}
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>Ultrasonic sensor reading on server</h2>
   
  <p>
   <i class="fa fa-road" style="color:#00add6;"></i> 
    <span class="dht-labels">Distance</span> 
    <span id="Distance">%DISTANCE%</span>
    <sup class="units">Cm</sup>
  </p>
</body>
<script>  
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Distance").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/distanceCm", true);
  xhttp.send();
}, 1000 ) ;

</script>
</html>)rawliteral";
String processor(const String& var){
  //Serial.println(var);
   if(var =="DISTANCE"){
   return readDistance();
  }
  return String();
}

void setup(){
  //Inicialización de DHT11
  dht.begin();

  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); 

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("connected to wifi ....");
  if(!MDNS.begin("esp32")) {
     Serial.println("Error starting mDNS");
     return;
}
  
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
   server.on("/distanceCm", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readDistance().c_str());
  });


  // Start server
  server.begin();
}
 
void loop(){
  delay(2000);
  sensors_event_t event;
  float *data_dht11 = obtenerHumedadTemperatura();
  humedad = data_dht11[0];
  temperatura = data_dht11[1];
}

// Funciones

// Función para obtener la humedad y temperatura del sensor DHT
float *obtenerHumedadTemperatura()
{
  static float datos[2]; // Arreglo estático para almacenar los datos

  // Estructura para almacenar los eventos del sensor
  sensors_event_t event;

  // Tomar el evento de humedad
  dht.humidity().getEvent(&event);
  // Acceder al valor de humedad
  datos[0] = event.relative_humidity;

  // Tomar el evento de temperatura
  dht.temperature().getEvent(&event);
  // Tomar el dato de temperatura
  datos[1] = event.temperature;

  return datos; // Devolver los datos en un arreglo
}
