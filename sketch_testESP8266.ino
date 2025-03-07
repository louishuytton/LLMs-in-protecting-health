#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "Tan Binh Robotics";
const char* password = "88888888";
const char* host = "api.openai.com";  /// URL API

void setup() {
  Serial.begin(115200);
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

void loop() {
 if (Serial.available() > 0)  {
  string data = Serial.readStringUntil("\n");
  sendDataToChatGPT(data);
  }


}
void sendDataToChatGPT(data)  {
  WiFiClientSecure client;
  if (!client.connect(host, 443))  {
    Serial.println("FAILED CONNECTING CHATGPT");
    return;
  }
String url =  "--" // endpoint của API ChatGPT bạn sẽ gửi yêu cầu.
String apikey = "APIKEY"  //chứa API key của bạn để xác thực yêu cầu với máy chủ OpenAI.
String jsondata = "{\"prompt\": \"" + data + "\", \"max_tokens\": 12}";

client.print(string("POST ") + url + " HTTP/1.1\r\n" +
            "Host: " + host + "\r\n" +
            "Authorization: " + apikey + "\r\n" + 
            "Content-Type: application/json\r\n" +
            "Content-Length: " + jsonData.length() + "\r\n" + 
            "\r\n" + 
            jsonData);

while (client.connect())  {
  String line = client.readStringUntil("\n");
  if (line == "\r") {
    Serial.println("Header kết thúc");
    break;
  }
}
String line = client.readStringUntil("\n");
Serial.println("Data from chatGPT: " + line);
client.stop();


}

