// 示例 2：舵机 0度 和 180度 循环切换
// 说明：每2秒切换一次角度，观察舵机旋转效果

#include <ESP32Servo.h>  // 引入 ESP32 专用的舵机控制库

const int servoPin = 14;  // 定义舵机信号线连接的 GPIO 引脚为 14
Servo myServo;            // 创建一个 Servo 类的对象，用于控制舵机

void setup() {
  myServo.setPeriodHertz(50);           // 设置舵机的控制频率为 50Hz，标准舵机工作频率
  myServo.attach(servoPin, 500, 2500);  // 将舵机对象绑定到引脚，并限制 PWM 脉宽范围为 500~2500 微秒
                                        // 500μs 通常代表 0 度，2500μs 代表 180 度
}

void loop() {
  myServo.write(0);     // 将舵机转动到 0 度位置
  delay(2000);          // 停留 2 秒（2000 毫秒）

  myServo.write(180);   // 将舵机转动到 180 度位置
  delay(2000);          // 再停留 2 秒
}
