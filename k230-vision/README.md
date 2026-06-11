# K230 视觉板代码

基于 CanMV MicroPython 的 K230 视觉识别代码，运行在庐山派开发板上。

## 文件说明

| 文件 | 功能 | 使用阶段 |
|------|------|---------|
| `01_env_calibration.py` | 曝光增益 + 白平衡校准 | 环境调试 |
| `02_robust_ball_finder.py` | 色块检测 + 球体识别主程序 | 正式比赛 |
| `03_pnp_distance_link.py` | PnP 距离估算 (cm) | 测距验证 |
| `04_color_threshold_tool.py` | 交互式色阈值调试工具 | 颜色校准 |

## 工作流程

1. **环境校准** → 运行 `01_env_calibration.py`，调节 `exposure_gain` 参数适应场地光照
2. **颜色校准** → 运行 `04_color_threshold_tool.py`，把球放在画面中央采样区，获得 RGB 阈值
3. **阈值填入** → 将获得的阈值更新到 `02_robust_ball_finder.py` 的 `ball_threshold` 变量
4. **距离验证** → 运行 `03_pnp_distance_link.py`，验证 PnP 测距精度，必要时重新标定相机内参
5. **正式运行** → 部署 `02_robust_ball_finder.py`

## 串口输出格式

```
found: err_x=15 err_y=-8 ball_h=45     # 检测到球
no ball                                  # 未检测到球
distance = 35.2 cm                       # PnP测距结果
```

## 硬件依赖

- 庐山派 K230 开发板
- GC2093 摄像头 (1920×1080 @ 60fps)
- CanMV IDE (Windows: `tools/canmv-ide-windows-4.0.9.exe`)
