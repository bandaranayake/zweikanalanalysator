#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#define CH1_pin 32
#define ULD1_pin 35
#define CH2_pin 33
#define ULD2_pin 25

const char *ssid = "";
const char *password = "";

int ch1Value;
int ch2Value;
int uld1Value;
int uld2Value;

int ch1Output;
int ch2Output;

ESP8266WebServer server(80);

Adafruit_MCP4728 mcp;

void setup() {
  Serial.begin(115200);

  if (!mcp.begin()) {
    Serial.println("Failed to find MCP4728 chip");
    while (1) {
      delay(10);
    }
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  attachInterrupt(CH1_pin, countCH1, FALLING);
  attachInterrupt(ULD1_pin, countULD1, FALLING);
  attachInterrupt(CH2_pin, countCH2, FALLING);
  attachInterrupt(ULD2_pin, countULD2, FALLING);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/calibrate", HTTP_GET, handleCalibrateGet);
  server.on("/calibrate", HTTP_POST, handleCalibratePost);
  server.on("/results", HTTP_GET, handleResultsGet);
  server.on("/configure", HTTP_POST, handleConfigPost);
  server.begin();

  Serial.print("WiFi connected. IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}

void IRAM_ATTR countCH1() {
  ch1Value = ch1Value + 1;
}

void IRAM_ATTR countCH2() {
  ch2Value = ch2Value + 1;
}

void IRAM_ATTR countULD1() {
  uld1Value = uld1Value + 1;
}

void IRAM_ATTR countULD2() {
  uld2Value = uld2Value + 1;
}

void handleRoot() {
  String html = "<title>ZWEIKANALANALYSATOR</title><style>a,button{color:#fff}body{background-color:#f4f4f4;font-family:Arial,sans-serif;margin:0;padding:20px;text-align:center}h1{color:#333}input{margin-left:10px;padding:5px}button{border:none;border-radius:5px;cursor:pointer;margin-right:5px;padding:10px 20px}p{margin:10px 0}a{text-decoration:none}.container{margin-bottom:20px}.output-container{font-size:18px;margin-top:20px}#errorLabel{color:#f44336;font-size:1.5vh}</style><h1>ZWEIKANALANALYSATOR</h1><div class=container><label for=ch1uld>CH1ULD:</label> <input id=ch1uld onchange='validateInput(this.value,\"ch1uld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch1lld>CH1LLD:</label> <input id=ch1lld onchange='validateInput(this.value,\"ch1lld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch2uld>CH2ULD:</label> <input id=ch2uld onchange='validateInput(this.value,\"ch2uld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch2lld>CH2LLD:</label> <input id=ch2lld onchange='validateInput(this.value,\"ch2lld\")'placeholder=\"Enter value\"type=number></div><div class=container><span id=errorLabel></span></div><a href=/calibrate><button style=background-color:#4caf50>Calibrate</button></a> <button style=background-color:#008cba onclick=onStartClick()>Start</button> <button style=background-color:#f44336 onclick=onStopClick()>Stop</button> <button style=background-color:#555 onclick=onResetClick()>Reset</button><div class=output-container><p>CH1: <span id=ch1Output>0</span><p>CH2: <span id=ch2Output>0</span><p>Time: <span id=timeOutput>0</span></div><script>let ch1uld=document.getElementById('ch1uld'),ch1llp=document.getElementById('ch1lld'),ch2ulp=document.getElementById('ch2uld'),ch2llp=document.getElementById('ch2lld'),errorLabel=document.getElementById('errorLabel'),ch1uldValue,ch1lldValue,ch2uldValue,ch2lldValue,ch1Output=0,ch2Output=0,timeOutput=0,interval=null;function validateInput(e,t){document.getElementById(t).value=validate(e)}function validate(e){let t=parseInt(e);return t>4096?4096:t<0?0:t}function onStartClick(){ch1uldValue=validate(ch1uld.value),ch1lldValue=validate(ch1lld.value),ch2uldValue=validate(ch2uld.value),ch2lldValue=validate(ch2lld.value);let e=null;if(ch1uldValue||0===ch1uldValue?ch1lldValue||0===ch1lldValue?ch2uldValue||0===ch2uldValue?ch2lldValue||0===ch2lldValue?null!=interval&&(e='Please terminate the running process before proceeding'):e='Enter a value between 0 and 4096 for CH2LLD':e='Enter a value between 0 and 4096 for CH2ULD':e='Enter a value between 0 and 4096 for CH1LLD':e='Enter a value between 0 and 4096 for CH1ULD',!e){errorLabel.style.display='none',timeOutput=0;let t={ch1uld:ch1uldValue,ch1lld:ch1lldValue,ch2uld:ch2uldValue,ch2lld:ch2lldValue},l={method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(t)};fetch('/configure',l).then(e=>{if(!e.ok)throw Error(`HTTP error! Status: ${e.status}`);return e.json()}).then(t=>{console.log('Response: ',t),interval=setInterval(()=>{fetch('/results',{method:'GET'}).then(e=>{if(!e.ok)throw Error(`HTTP error! Status: ${e.status}`);return e.json()}).then(e=>{ch1Output=e.ch1Output,ch2Output=e.ch2Output,timeOutput+=1,updateOutputs()}).catch(t=>{e=t})},1e3)}).catch(t=>{e=t})}e&&(errorLabel.innerText=e,errorLabel.style.display='inline')}function onStopClick(){clearInterval(interval),interval=null}function onResetClick(){ch1Output=0,ch2Output=0,timeOutput=0,updateOutputs()}function updateOutputs(){document.getElementById('ch1Output').innerText=ch1Output,document.getElementById('ch2Output').innerText=ch2Output,document.getElementById('timeOutput').innerText=timeOutput}</script>";
  server.send(200, "text/html", html);
}

void handleCalibrateGet() {
  String html = "<title>ZWEIKANALANALYSATOR</title><style>a,button{color:#fff}body{background-color:#f4f4f4;font-family:Arial,sans-serif;margin:0;padding:20px;text-align:center}h1{color:#333}input{margin-left:10px;padding:5px}button{border:none;border-radius:5px;cursor:pointer;margin-right:5px;padding:10px 20px}p{margin:10px 0}a{text-decoration:none}table{margin-left:auto;margin-right:auto}td,th{border:1px solid #000;padding:8px;text-align:left}.container{margin-bottom:20px}.output-container{font-size:18px;margin-top:20px}#errorLabel{color:#f44336;font-size:1.5vh}</style><h1>ZWEIKANALANALYSATOR - CALIBRATOR</h1><div class=container><label for=interval>Interval:</label> <input id=interval onchange=validateInput(this.value) placeholder=\"Enter value\"type=number></div><div class=container><span id=errorLabel></span></div><a href=/ ><button style=background-color:#4caf50>Home</button></a> <button style=background-color:#008cba onclick=onStartClick()>Start</button> <button style=background-color:#f44336 onclick=onStopClick()>Stop</button><table class=output-container id=countTable><thead><tr><th>Limit<th>Count<tbody></table><script>let tableBody=document.getElementById('countTable').getElementsByTagName('tbody')[0],interval=document.getElementById('interval'),intervalValue,lowerLimit=0,upperLimit=0,asyncInterval=null;function validateInput(e,t){interval.value=validate(e)}function validate(e){let t=parseInt(e);return t>4096?4096:t<1?1:t}function onStartClick(){intervalValue=validate(interval.value);let e=null;intervalValue?null!=asyncInterval&&(e='Please terminate the running process before proceeding'):e='Enter a value between 1 and 4096',e||(errorLabel.style.display='none',asyncInterval=setInterval(()=>{if(0!==upperLimit?(lowerLimit+=intervalValue,upperLimit+=intervalValue):(tableBody.innerHTML=null,upperLimit+=intervalValue),upperLimit>4096+intervalValue)onStopClick();else{let t={lowerLimit:lowerLimit,upperLimit:upperLimit},n={method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(t)};fetch('/calibrate',n).then(e=>{if(!e.ok)throw Error(`HTTP error! Status: ${e.status}`);return e.json()}).then(e=>{-1!==e.count&&updateOutputs(lowerLimit-intervalValue+' - '+(upperLimit-intervalValue),e.count)}).catch(t=>{e=t})}},1e4)),e&&(errorLabel.innerText=e,errorLabel.style.display='inline')}function onStopClick(){clearInterval(asyncInterval),asyncInterval=null,lowerLimit=0,upperLimit=0}function updateOutputs(e,t){let n=tableBody.insertRow(tableBody.rows.length);n.insertCell(0).innerHTML=e;n.insertCell(1).innerHTML=t}</script>";
  server.send(200, "text/html", html);
}

void handleCalibratePost() {
  String jsonBody = server.arg("plain");

  DynamicJsonDocument jsonDocument(256);
  DeserializationError error = deserializeJson(jsonDocument, jsonBody);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Failed to parse JSON");
    return;
  }

  int lowerLimit = jsonDocument["lowerLimit"].as<int>();
  int upperLimit = jsonDocument["upperLimit"].as<int>();
  int count = calcRealCount(ch1Value, uld1Value);

  if (lowerLimit == 0) {
    count = -1;
  }

  if (upperLimit < 4096) {
    ch1Value = 0;
    uld1Value = 0;
  }

  String response = "{\"count\":" + String(count) + "}";
  server.send(200, "text/plain", response);
}

void handleConfigPost() {
  String jsonBody = server.arg("plain");

  DynamicJsonDocument jsonDocument(256);
  DeserializationError error = deserializeJson(jsonDocument, jsonBody);

  if (error) {
    Serial.print("Failed to parse JSON: ");
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Failed to parse JSON");
    return;
  }

  int ch1uld = jsonDocument["ch1uld"].as<int>();
  int ch1lld = jsonDocument["ch1lld"].as<int>();
  int ch2uld = jsonDocument["ch2uld"].as<int>();
  int ch2lld = jsonDocument["ch2lld"].as<int>();

  mcp.setChannelValue(MCP4728_CHANNEL_A, ch1uld);
  mcp.setChannelValue(MCP4728_CHANNEL_B, ch1lld);
  mcp.setChannelValue(MCP4728_CHANNEL_C, ch2uld);
  mcp.setChannelValue(MCP4728_CHANNEL_D, ch2lld);

  ch1Value = 0;
  ch2Value = 0;
  uld1Value = 0;
  uld2Value = 0;

  String response = "{\"status\":\"Successful\"}";
  server.send(200, "text/plain", response);
}

void handleResultsGet() {
  String response = "{\"ch1Output\":" + String(calcRealCount(ch1Value, uld1Value)) + " , " + "\"ch2Output\":" + String(calcRealCount(ch2Value, uld2Value)) + "}";
  server.send(200, "text/plain", response);
}

int calcRealCount(int cValue, int uldValue) {
  return cValue - (uldValue * 2);
}