#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

const char *ssid = "";
const char *password = "";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/calibrate", HTTP_GET, handleCalibrateGet);
  server.on("/calibrate", HTTP_POST, handleCalibratePost);
  server.on("/configure", HTTP_POST, handleConfigPost);
  server.begin();

  Serial.print("WiFi connected. IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<title>Title</title><style>body{background-color:#f4f4f4;font-family:Arial,sans-serif;margin:0;padding:20px;text-align:center}h1{color:#333}input{margin-left:10px;padding:5px}button{border:none;border-radius:5px;color:#fff;cursor:pointer;margin-right:5px;padding:10px 20px}p{margin:10px 0}.container{margin-bottom:20px}.output-container{font-size:18px;margin-top:20px}#errorLabel{color:#f44336;font-size:1.5vh}</style><h1>Title</h1><div class=container><label for=ch1uld>CH1ULD:</label> <input id=ch1uld onchange='validateInput(this.value,\"ch1uld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch1lld>CH1LLD:</label> <input id=ch1lld onchange='validateInput(this.value,\"ch1lld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch2uld>CH2ULD:</label> <input id=ch2uld onchange='validateInput(this.value,\"ch2uld\")'placeholder=\"Enter value\"type=number></div><div class=container><label for=ch2lld>CH2LLD:</label> <input id=ch2lld onchange='validateInput(this.value,\"ch2lld\")'placeholder=\"Enter value\"type=number></div><div class=container><span id=errorLabel></span></div><button onclick=onStartClick() style=background-color:#008cba>Start</button> <button onclick=onStopClick() style=background-color:#f44336>Stop</button> <button onclick=onResetClick() style=background-color:#555>Reset</button><div class=output-container><p>CH1: <span id=ch1Output>0</span><p>CH2: <span id=ch2Output>0</span><p>Time: <span id=timeOutput>0</span></div><script>let ch1uld=document.getElementById('ch1uld'),ch1llp=document.getElementById('ch1lld'),ch2ulp=document.getElementById('ch2uld'),ch2llp=document.getElementById('ch2lld'),errorLabel=document.getElementById('errorLabel'),ch1uldValue,ch1lldValue,ch2uldValue,ch2lldValue,ch1Output=0,ch2Output=0,timeOutput=0,interval=null;function validateInput(e,l){document.getElementById(l).value=validate(e,l)}function validate(e){let l=parseInt(e);return l>4096?4096:l<0?0:l}function onStartClick(){ch1uldValue=validate(ch1uld.value),ch1lldValue=validate(ch1lld.value),ch2uldValue=validate(ch2uld.value),ch2lldValue=validate(ch2lld.value);let e=null;ch1uldValue||0===ch1uldValue?ch1lldValue||0===ch1lldValue?ch2uldValue||0===ch2uldValue?ch2lldValue||0===ch2lldValue?null!=interval&&(e='Please terminate the running process before proceeding'):e='Enter a value between 0 and 4096 for CH2LLD':e='Enter a value between 0 and 4096 for CH2ULD':e='Enter a value between 0 and 4096 for CH1LLD':e='Enter a value between 0 and 4096 for CH1ULD',e||(errorLabel.style.display='none',interval=setInterval(()=>{let l={ch1uld:ch1uldValue,ch1lld:ch1lldValue,ch2uld:ch2uldValue,ch2lld:ch2lldValue},t={method:'POST',headers:{'Content-Type':'application/json'},body:JSON.stringify(l)};fetch('/configure',t).then(e=>{if(!e.ok)throw Error(`HTTP error! Status: ${e.status}`);return e.json()}).then(e=>{console.log('Response:',e)}).catch(l=>{e=l})},1e3)),e&&(errorLabel.innerText=e,errorLabel.style.display='inline')}function onStopClick(){clearInterval(interval),interval=null}function onResetClick(){ch1Output=0,ch2Output=0,timeOutput=0,updateOutputs()}function updateOutputs(){document.getElementById('ch1Output').innerText=ch1Output,document.getElementById('ch2Output').innerText=ch2Output,document.getElementById('timeOutput').innerText=timeOutput}</script>";
  server.send(200, "text/html", html);
}

void handleCalibrateGet() {
  String html = "<title>ZWEIKANALANALYSATOR</title><style>a,button{color:#fff}body{background-color:#f4f4f4;font-family:Arial,sans-serif;margin:0;padding:20px;text-align:center}h1{color:#333}input{margin-left:10px;padding:5px}button{border:none;border-radius:5px;cursor:pointer;margin-right:5px;padding:10px 20px}p{margin:10px 0}a{text-decoration:none}table{margin-left:auto;margin-right:auto}td,th{border:1px solid #000;padding:8px;text-align:left}.container{margin-bottom:20px}.output-container{font-size:18px;margin-top:20px}#errorLabel{color:#f44336;font-size:1.5vh}</style><h1>ZWEIKANALANALYSATOR - CALIBRATOR</h1><div class=container><label for=interval>Interval:</label> <input id=interval onchange=validateInput(this.value) placeholder=\"Enter value\"type=number></div><div class=container><span id=errorLabel></span></div><button onclick=onStartClick() style=background-color:#008cba>Start</button> <button onclick=onStopClick() style=background-color:#f44336>Stop</button><table class=output-container id=countTable><thead><tr><th>Limit<th>Count<tbody></table><script>let interval=document.getElementById(\"interval\"),intervalValue,lowerLimit=0,upperLimit=0,asyncInterval=null;function validateInput(e,t){interval.value=validate(e)}function validate(e){let t=parseInt(e);return t>4096?4096:t<1?1:t}function onStartClick(){intervalValue=validate(interval.value);let e=null;intervalValue?null!=asyncInterval&&(e=\"Please terminate the running process before proceeding\"):e=\"Enter a value between 1 and 4096\",e||(errorLabel.style.display=\"none\",asyncInterval=setInterval(()=>{if(0!==upperLimit&&(lowerLimit+=intervalValue),(upperLimit+=intervalValue)>4096)onStopClick();else{let t={intervalValue:intervalValue,lowerLimit:lowerLimit,upperLimit:upperLimit},n={method:\"POST\",headers:{\"Content-Type\":\"application/json\"},body:JSON.stringify(t)};fetch(\"/calibrate\",n).then(e=>{if(!e.ok)throw Error(`HTTP error! Status: ${e.status}`);return e.json()}).then(e=>{updateOutputs(lowerLimit+\" - \"+upperLimit,e.count)}).catch(t=>{e=t})}},1e3)),e&&(errorLabel.innerText=e,errorLabel.style.display=\"inline\")}function onStopClick(){clearInterval(asyncInterval),asyncInterval=null,lowerLimit=0,upperLimit=0}function updateOutputs(e,t){let n=document.getElementById(\"countTable\").getElementsByTagName(\"tbody\")[0],l=n.insertRow(n.rows.length);l.insertCell(0).innerHTML=e;l.insertCell(1).innerHTML=t}</script>";
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

  int intervalValue = jsonDocument["intervalValue"].as<int>();
  int lowerLimit = jsonDocument["lowerLimit"].as<int>();
  int upperLimit = jsonDocument["upperLimit"].as<int>();

  String response = "{ \"count\":\"" + String(intervalValue + lowerLimit + upperLimit) + "\" }";
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

  String response = "Parsed JSON: key1=" + String(ch1uld) + ", key2=" + String(ch1lld) + ", key2=" + String(ch2uld) + ", key2=" + String(ch2lld);
  server.send(200, "text/plain", response);
}