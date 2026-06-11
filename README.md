# 救援机器人项目 (Rescue Robot)

## 项目概述

金工实习课设 —— 自动救援机器人。通过 **K230 视觉板** 识别特定颜色的球体，**ESP32 驱动板** 控制四轮小车行驶到球前，舵机降下框架将球框住，再推送到指定位置。

## 系统架构

```
┌─────────────────────┐     UART/串口      ┌──────────────────────┐
│   K230 视觉板        │ ◄──────────────► │   ESP32 电机驱动板    │
│   (CanMV / MicroPython)│                  │   (Arduino)           │
│                      │                   │                       │
│  • 摄像头采集        │                   │  • 4路直流电机控制     │
│  • 色块检测/球识别   │                   │  • 3路舵机控制        │
│  • PnP 距离估算      │                   │  • WiFi WebSocket通信  │
│  • 偏差计算          │                   │  • Android APP遥控     │
└─────────────────────┘                   └──────────────────────┘
```

## 项目结构

```
rescue-robot/
├── README.md                    # 项目主文档
├── .gitignore                   # Git忽略规则
├── k230-vision/                 # K230 视觉板代码 (MicroPython)
│   ├── 01_env_calibration.py    # 相机环境光校准
│   ├── 02_robust_ball_finder.py # 鲁棒球体检测
│   ├── 03_pnp_distance_link.py  # PnP距离估算
│   └── 04_color_threshold_tool.py # 色阈值调试工具
├── esp32-control/               # ESP32 电机驱动板代码 (Arduino)
│   ├── motor/                   # 电机正反转基础示例
│   ├── servo/                   # 舵机控制示例
│   ├── wifi_control/            # WiFi热点 + 网页摇杆遥控
│   └── app_control/             # Android APP 控制方式
│       ├── fixed_wifi/          # 固定WiFi账号密码版本
│       └── wifi_manager/        # 网页配网版本 (WiFiManager)
├── dataset/                     # YOLO 训练数据集
│   ├── images/                  # 图片 (train/val/test)
│   ├── labels/                  # 标注 (YOLO格式)
│   └── dataset.yaml             # 数据集配置文件
├── docs/                        # 文档资料
│   ├── ESP32教程.pdf
│   ├── 电机驱动原理.pdf
│   ├── 原理图.pdf
│   ├── BOM清单.xlsx
│   ├── 答辩PPT.pptx
│   └── 配网教程.pdf
└── tools/                       # 工具软件
    ├── 小车遥控APP.apk
    ├── canmv-ide-windows.exe    # K230 IDE
    └── K230固件.img.gz
```

## 硬件清单

| 组件 | 型号/规格 | 数量 |
|------|----------|------|
| 视觉板 | K230 (庐山派/CanMV) | 1 |
| 电机驱动板 | ESP32-WROOM 4WD | 1 |
| 电机驱动芯片 | L9110S | 2 |
| 直流电机 | 双电源线小电机 | 4 |
| 舵机 | SG90/MG996R | 3 |
| 摄像头 | GC2093 | 1 |

## 快速开始

### 1. K230 视觉板

1. 使用 CanMV IDE 连接 K230 开发板
2. 刷入固件 (`tools/CanMV_K230_*.img.gz`)
3. 运行 `04_color_threshold_tool.py` 校准目标颜色的阈值
4. 将校准后的阈值填入 `02_robust_ball_finder.py`
5. 部署 `02_robust_ball_finder.py` 进行球体识别

### 2. ESP32 电机驱动板

1. 使用 Arduino IDE 安装 ESP32 开发板支持
2. 安装依赖库：`ESP32Servo`, `WebSocketsServer`, `ArduinoJson`
3. 根据需求选择控制方式：
   - 调试阶段：`wifi_control/wifi.ino` (自建热点 + 网页遥控)
   - 比赛阶段：`app_control/` (配合Android APP使用)

### 3. 通信协议

K230 → ESP32 通过串口发送控制指令：

```
err_x: 球心相对画面中心的水平偏差 (正值偏右，负值偏左)
err_y: 球心相对画面中心的垂直偏差
ball_h: 球的像素高度 (越大越近)
```

ESP32 根据偏差值执行：前进/后退/左转/右转/停车/降框

## 数据集

YOLO 格式标注数据集，用于训练球体检测模型：
- 训练集: 2226 张
- 验证集: 523 张
- 测试集: 28 张
- 类别数: 8 类

## 往届参考资料

本项目整理自往届金工实习救援机器人比赛的代码和资料，包含：
- 视觉识别核心算法 (色块检测 + PnP测距)
- 四轮差速运动控制
- WiFi/蓝牙遥控方案
- YOLO目标检测训练数据集

## 许可证

Educational Use Only
