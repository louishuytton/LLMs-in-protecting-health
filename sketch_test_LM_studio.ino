#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include <String.h>


// Thông tin WiFi
const char* ssid = "Louis Huytton's phone";       // Tên WiFi
const char* password = "kochocutdilaotaoquynh";  // Mật khẩu WiFi

// Địa chỉ và cổng của server local
const char* serverHost = "";  // Địa chỉ IP của server (thay thế bằng địa chỉ IP của máy chạy server)
const int serverPort =;                // Cổng của server (899 trong trường hợp này)

// Đường dẫn tới API endpoint
const char* apiEndpoint = "/v1/chat/completions";

float UV;
float Tem;
float Humi;
String response = "";


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
StaticJsonDocument<100> doc;
ESP8266WebServer server(80);
WiFiClient client;

void setup() {
  begin_LCD();
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Kết nối với WiFi
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");

  // Kết nối tới server
  if (!client.connect(serverHost, serverPort)) {
    Serial.println("Connection to server failed!");
    return;
  }
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);  // Trang chủ
  server.on("/send", handleSend);  // Xử lý khi nhấn nút gửi

  server.begin();  // Khởi động server
}

void handleRoot() {
  String html = "<!DOCTYPE html><html>";
html += "<head><meta charset='UTF-8'>";
html += "<title>ESP8266 Dữ liệu theo thời gian thực</title>";
html += "<style>";
html += "body { font-family: Arial, sans-serif; text-align: center; }";
html += "img { width: 200px; height: auto; }";
html += ".container { display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh; }";
html += ".data { margin-top: 20px; font-size: 20px; }";
html += ".button { margin-top: 20px; padding: 10px 20px; font-size: 16px; }";
html += "</style></head><body>";
html += "<div class='container'>";
html += "<h1>ESP8266 Dữ liệu theo thời gian thực</h1>";
html += "<img src='https://scontent.fsgn5-7.fna.fbcdn.net/v/t39.30808-1/453212078_939808054617637_1500066829339149055_n.jpg?stp=dst-jpg_s200x200&_nc_cat=100&ccb=1-7&_nc_sid=f4b9fd&_nc_eui2=AeHZkrF9RyYnjXRb1gg5xqOQBod_Pn84vvAGh38-fzi-8B0U3eb2ObM3owvIFT7L5TbCYbolLYusP_0_JMH6y94a&_nc_ohc=IEc7veQzqRQQ7kNvgEX5BHA&_nc_ht=scontent.fsgn5-7.fna&_nc_gid=ANheM5cHQ4LIqJwCmU4r-td&oh=00_AYDAAPxJijcoIrf8J5ce6nKVS5SZPQ6itkKblxnH0u177g&oe=670816B5' alt='Logo'>";
html += "<div class='data'>";
html += "<p>Dữ liệu UV hiện tại: " + String(UV) + "</p>";
html += "<p>Nhiệt độ hiện tại: " + String(Tem) + "°C</p>";
html += "<p>Độ ẩm hiện tại: " + String(Humi) + "%</p>";
html += "</div>";
html += "<button class='button' onclick='sendData()'>Gửi dữ liệu lên LM Studio</button>";
html += "</div>";
html += "<div id='lmStudioResponse'><p>Kết quả từ LM Studio: </p><p>" + response + "</p></div>";
html += "</div>";

// Script gửi dữ liệu đến server
html += "<script>";
html += "function sendData() {";
html += "  fetch('/send').then(response => response.text()).then(data => {";
html += "    alert('Dữ liệu đã gửi: ' + data);";
html += "  }).catch(error => {";
html += "    alert('Gửi thất bại!');";
html += "  });";
html += "}";
html += "</script>";

html += "</body></html>";
  
  // Trả về nội dung HTML
  server.send(200, "text/html", html);
}


void handleSend() {
  response = "";
  // Xử lý việc gửi dữ liệu tại đây
  String message = "Dữ liệu ESP8266 đã được gửi.";
  server.send(200, "text/plain", message);
  getrequest();
}

String getrequest() {
  // Tạo dữ liệu JSON để gửi đi
  String jsonPayload = "{"
    "\"model\": \"uonlp/Vistral-7B-chat-gguf\","
    "\"messages\": ["
      "{\"role\": \"system\", \"content\": \"Bạn là một trợ lý AI thông minh. Bạn luôn đem lại những lời khuyên về sức khỏe từ thông tin UV, nhiệt độ và độ ẩm. Bạn giúp tôi có thể biết mình nên làm gì khi gặp các yếu tố liên quan đến thời tiết.\"},"
      "{\"role\": \"user\", \"content\": \"Bây giờ tôi có những chỉ số sau: UV:" + String(UV) + ", Nhiệt độ: " + String(Tem) +", Độ ẩm: " + String(Humi) + ". Bạn hãy cho tôi những lời khuyên bổ ích để có thể tự giữ gìn sức khỏe và cho tôi biết nếu ở ngoài trời với điều kiện này tôi sẽ có nguy cơ mắc các loại bệnh nào?\"}"
    "],"
    "\"temperature\": 0.6,"
    "\"max_tokens\": 1450,"
    "\"stream\": true"
    
  "}";

  // Gửi yêu cầu HTTP POST
  client.println("POST " + String(apiEndpoint) + " HTTP/1.1");
  client.println("Host: " + String(serverHost));
  client.println("Content-Type: application/json");
  client.println("Content-Length: " + String(jsonPayload.length()));
  client.println();
  client.println(jsonPayload);

  
  String chunk = "";
  while (client.connected()) {
    while (client.available()) {
      chunk = client.readStringUntil('\n');  // Đọc từng dòng

      // Kiểm tra nếu dòng chứa dữ liệu JSON có "data: "
      if (chunk.startsWith("data: ")) {
        // Loại bỏ phần "data: " để lấy JSON thực tế
        String jsonData = chunk.substring(6);

        // Kiểm tra nếu không phải [DONE]
        if (jsonData != "[DONE]") {
          // Tìm phần "content" trong JSON
          int contentStart = jsonData.indexOf("\"content\":\"") + 11;
          int contentEnd = jsonData.indexOf("\"", contentStart);

          if (contentStart > 0 && contentEnd > contentStart) {
            // Trích xuất và ghép phần content
            String content = jsonData.substring(contentStart, contentEnd);
            response += content + "";  // Ghép nội dung lại với khoảng trắng
          }
        } else {
          break;  // Nếu gặp [DONE] thì dừng quá trình đọc
        }
      }
    }
  }

  Serial.println("Response from server:");
  Serial.println(response);  // Loại bỏ khoảng trắng dư thừa

  return response;
}
void begin_LCD() {
  u8g2.begin();
  u8g2.clearBuffer();          // Xóa bộ đệm
  u8g2.setFont(u8g2_font_ncenB08_tr); // Chọn font chữ
  u8g2.drawStr(0,10,"Hello World!");  // Vẽ chuỗi ký tự tại vị trí (0,10)
  delay(700);
  u8g2.sendBuffer();           // Gửi bộ đệm lên màn hình
  u8g2.clearBuffer();
  u8g2.drawStr(0, 17, "Welcome");
  delay(700);
  u8g2.sendBuffer();
}

void loop() {
  getDataFromSeeed();
  controlOled();
  
  server.handleClient();
  handleRoot();

}

void getDataFromSeeed()  {
  if (Serial.available()) {
    // Đọc một dòng từ cổng nối tiếp ảo
    String line = Serial.readStringUntil('\n');

    // Chuyển đổi chuỗi JSON thành đối tượng JsonDocument
    DeserializationError error = deserializeJson(doc, line);
  
    //String 
    Tem = doc["temperature"];
    Humi = doc["humidity"];
    UV = doc["UV"];

  }
}

void controlOled()  {
  u8g2.clearBuffer();          // Xóa bộ đệm
  u8g2.setFont(u8g2_font_ncenB08_tr); // Chọn font chữ
  u8g2.setCursor(3,13);
  u8g2.print("AI GLOVE"); 
  
  u8g2.setFont(u8g2_font_t0_14_te);
  u8g2.setCursor(5,24);
  u8g2.print(Tem);
  u8g2.setCursor(50, 24);
  u8g2.print("*C");

  u8g2.setCursor(5, 37);
  u8g2.print(Humi);
  u8g2.setCursor(50, 37);
  u8g2.print("%");
  
  u8g2.setCursor(5, 49);
  u8g2.print(UV);
  u8g2.setCursor(50, 49);
  u8g2.print("(UV index)");
  u8g2.sendBuffer();
  
}