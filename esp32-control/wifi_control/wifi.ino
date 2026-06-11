// 示例 3：网页摇杆和滑块控制（仅串口输出）
// 说明：使用 WebSocket 接收网页数据并通过串口输出，方便调试

#include <WiFi.h>  // 引入 WiFi 库
#include <WebSocketsServer.h>  // 引入 WebSocket 服务器库

const char* ssid = "CarESP32";      // 设置 Wi-Fi 热点名称
const char* password = "12345678";  // 设置 Wi-Fi 热点密码

WiFiServer server(80);                // 创建 HTTP 服务器对象
WebSocketsServer webSocket(81);      // 创建 WebSocket 服务器对象，监听端口 81

void setup() {
  Serial.begin(115200);              // 初始化串口，用于调试输出
  WiFi.softAP(ssid, password);       // 启用 ESP32 AP 模式（创建热点）
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());   // 输出热点 IP 地址

  server.begin();        // 启动 HTTP 服务
  webSocket.begin();     // 启动 WebSocket 服务
  webSocket.onEvent(webSocketEvent); // 注册 WebSocket 事件处理函数
}

void loop() {
  webSocket.loop();                // 处理 WebSocket 消息
  WiFiClient client = server.available();  // 检查是否有网页连接
  if (client) {
    sendHTML(client);  // 发送网页 HTML 页面
    client.stop();     // 关闭客户端连接
  }
}

// WebSocket 接收到消息时的事件处理函数
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {  // 如果接收到的是文本类型数据
    String data = String((char*)payload);  // 将 payload 转为字符串
    if (data.startsWith("servo:")) {       // 判断是否为舵机角度指令
      int angle = data.substring(6).toInt();   // 提取角度数值
      Serial.printf("[网页滑块] 舵机角度: %d°\n", angle);  // 输出到串口
    } else {  // 否则为摇杆指令
      int comma = data.indexOf(',');  // 查找逗号分隔符
      if (comma != -1) {
        float x = data.substring(0, comma).toFloat();           // 提取 X 坐标
        float y = data.substring(comma + 1).toFloat();          // 提取 Y 坐标
        Serial.printf("[网页摇杆] X: %.2f, Y: %.2f\n", x, y);   // 输出到串口
      }
    }
  }
}

// 向网页客户端发送 HTML 页面
void sendHTML(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");                 // HTTP 协议头
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();

  client.println(R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">  <!-- 设置网页字符集 -->
  <meta name="viewport" content="width=device-width, initial-scale=1.0">  <!-- 移动端适配 -->
  <title>遥控界面</title>
  <style>
    body { text-align: center; font-family: Arial; margin: 0; background: #f0f0f0; }
    canvas { border: 2px solid #aaa; touch-action: none; margin-top: 20px; }
    #servoControl { margin-top: 30px; }
    input[type=range] { width: 80%; max-width: 400px; }
  </style>
</head>
<body>
  <h2>ESP32 遥控界面</h2>
  <canvas id="joystick" width="300" height="300"></canvas> <!-- 摇杆区域 -->
  <div id="servoControl">
    <label for="servoSlider">舵机角度: <span id="servoAngle">90</span>°</label><br>
    <input type="range" id="servoSlider" min="0" max="180" value="90"> <!-- 滑块 -->
  </div>

  <script>
    const canvas = document.getElementById('joystick');  // 获取摇杆画布
    const ctx = canvas.getContext('2d');                 // 获取绘图上下文
    const radius = 100;                                  // 摇杆最大半径
    const centerX = canvas.width / 2;                    // 中心点X
    const centerY = canvas.height / 2;                   // 中心点Y
    let posX = centerX, posY = centerY;                  // 初始位置
    let isDragging = false;                              // 是否拖动中
    const ws = new WebSocket('ws://' + location.hostname + ':81/'); // 建立 WebSocket 连接

    // 绘制摇杆界面
    function drawJoystick() {
      ctx.clearRect(0, 0, canvas.width, canvas.height);  // 清空画布
      ctx.beginPath();
      ctx.arc(centerX, centerY, radius, 0, 2 * Math.PI); // 绘制背景圈
      ctx.strokeStyle = '#aaa';
      ctx.stroke();
      ctx.beginPath();
      ctx.arc(posX, posY, 20, 0, 2 * Math.PI);           // 绘制控制球
      ctx.fillStyle = '#4CAF50';
      ctx.fill();
    }

    // 计算摇杆位置（相对中心坐标归一化）
    function calculatePosition(e) {
      let rect = canvas.getBoundingClientRect();
      let x = (e.clientX - rect.left - centerX) / radius;
      let y = (e.clientY - rect.top - centerY) / radius;
      let dist = Math.sqrt(x * x + y * y);
      if (dist > 1) { x /= dist; y /= dist; }  // 限制在圆圈范围内
      posX = centerX + x * radius;
      posY = centerY + y * radius;
      return { x, y };
    }

    // 发送摇杆控制指令
    function sendControl(x, y) {
      if (ws.readyState === WebSocket.OPEN) {
        ws.send(x + "," + y);  // 格式：x,y
      }
    }

    // 鼠标事件绑定
    canvas.addEventListener('mousedown', e => {
      isDragging = true;
      let pos = calculatePosition(e);
      drawJoystick();
      sendControl(pos.x, pos.y);
    });

    canvas.addEventListener('mousemove', e => {
      if (isDragging) {
        let pos = calculatePosition(e);
        drawJoystick();
        sendControl(pos.x, pos.y);
      }
    });

    canvas.addEventListener('mouseup', () => {
      isDragging = false;
      posX = centerX; posY = centerY;
      drawJoystick();
      sendControl(0, 0);  // 复位
    });

    // 移动端触摸事件绑定
    canvas.addEventListener('touchstart', e => {
      isDragging = true;
      let pos = calculatePosition(e.touches[0]);
      drawJoystick();
      sendControl(pos.x, pos.y);
      e.preventDefault();
    });

    canvas.addEventListener('touchmove', e => {
      if (isDragging) {
        let pos = calculatePosition(e.touches[0]);
        drawJoystick();
        sendControl(pos.x, pos.y);
      }
      e.preventDefault();
    });

    canvas.addEventListener('touchend', () => {
      isDragging = false;
      posX = centerX; posY = centerY;
      drawJoystick();
      sendControl(0, 0);  // 复位
    });

    // 滑块控制舵机
    const slider = document.getElementById("servoSlider");
    const angleText = document.getElementById("servoAngle");
    slider.addEventListener("input", () => {
      angleText.textContent = slider.value;  // 显示当前角度
      if (ws.readyState === WebSocket.OPEN) {
        ws.send("servo:" + slider.value);  // 格式：servo:90
      }
    });

    drawJoystick();  // 初始绘制摇杆
  </script>
</body>
</html>
)rawliteral");
}
