// 示例 1：电机正反转控制
// 说明：使用两个引脚控制一个电机正转2秒，反转2秒，循环往复

const int motorPin1 = 26; // 定义连接电机正极的GPIO引脚为26
const int motorPin2 = 27; // 定义连接电机负极的GPIO引脚为27

void setup() {
  pinMode(motorPin1, OUTPUT); // 设置motorPin1为输出模式
  pinMode(motorPin2, OUTPUT); // 设置motorPin2为输出模式
}

void loop() {
  // 电机正转：将motorPin1设置为高电平，motorPin2设置为低电平
  // 使电流从Pin1流向Pin2，从而带动电机正转
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
  delay(2000); // 保持正转状态2秒（2000毫秒）

  // 电机反转：将motorPin1设置为低电平，motorPin2设置为高电平
  // 使电流方向反过来，从而带动电机反转
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
  delay(2000); // 保持反转状态2秒

  // 循环继续，从正转开始
}
