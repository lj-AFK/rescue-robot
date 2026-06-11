# ESP32 电机驱动板代码

基于 Arduino 框架的 ESP32 四轮驱动控制代码。

## 目录结构

```
esp32-control/
├── motor/                          # 电机基础控制
│   └── Motor.ino                   # 双引脚直流电机正反转示例
├── servo/                          # 舵机控制
│   └── Servo.ino                   # ESP32Servo 库 0-180° 控制
├── wifi_control/                   # WiFi 网页遥控
│   └── wifi.ino                    # AP模式 + WebSocket 网页摇杆
└── app_control/                    # Android APP 控制
    ├── fixed_wifi/                 # 固定WiFi版本
    │   └── esp32-arduino.ino       # 连接固定路由器 + APP遥控
    └── wifi_manager/               # 配网版本
        └── esp32-arduino-manager.ino # WiFiManager动态配网 + APP遥控
```

## 引脚定义

### 电机引脚 (L9110S 驱动)

| 电机 | 正极引脚 | 负极引脚 |
|------|---------|---------|
| M1 (左前) | 27 | 26 |
| M2 (左后) | 25 | 33 |
| M3 (右前) | 32 | 16 |
| M4 (右后) | 17 | 18 |

### 舵机引脚

| 舵机 | 引脚 |
|------|------|
| Servo1 | 13 |
| Servo2 | 2 |
| Servo3 | 4 |

## 依赖库

在 Arduino IDE 库管理器中安装：

- `ESP32Servo` — 舵机控制
- `WebSocketsServer` — WebSocket 通信
- `ArduinoJson` — JSON 解析
- `WiFiManager` (仅 wifi_manager 版本) — 网页配网

## 控制方式对比

| 方式 | 目录 | 适用场景 |
|------|------|---------|
| WiFi热点 + 网页 | `wifi_control/` | 调试阶段，无需APP |
| 固定WiFi + APP | `app_control/fixed_wifi/` | 固定场地比赛 |
| 动态配网 + APP | `app_control/wifi_manager/` | 更换场地灵活配网 |

## WebSocket 通信协议

### 摇杆控制 (JSON)
```json
{"dp":"direction", "value":"forward"}      // 前进
{"dp":"direction", "value":"backward"}     // 后退
{"dp":"direction", "value":"left"}         // 左转
{"dp":"direction", "value":"right"}        // 右转
{"dp":"direction", "value":"stop"}         // 停止
{"dp":"direction", "value":"clockwise"}    // 顺时针旋转
{"dp":"direction", "value":"counterclockwise"} // 逆时针旋转
```

### 舵机控制
```json
{"dp":"servo", "value":90}     // 舵机角度 0-180
```

### 速度控制
```json
{"dp":"speed", "value":200}    // PWM速度 0-255
```

### K230 串口控制协议 (自动模式)

K230 通过串口发送以下格式数据，ESP32 解析后自动控制：
```
found: err_x=15 err_y=-8 ball_h=45
no ball
```
