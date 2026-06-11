# YOLO 训练数据集

用于训练球体/障碍物检测模型的数据集。

## 数据集概况

| 划分 | 数量 |
|------|------|
| 训练集 (train) | 2226 |
| 验证集 (val) | 523 |
| 测试集 (test) | 28 |
| **总计** | **2777** |

## 标注格式

YOLO 格式 (归一化坐标)：
```
<class_id> <x_center> <y_center> <width> <height>
```

所有坐标归一化到 [0, 1] 区间。

## 类别

| ID | 名称 |
|----|------|
| 0 | background |
| 1 | red_ball |
| 2 | blue_ball |
| 3 | green_ball |
| 4 | yellow_ball |
| 5 | purple_ball |
| 6 | orange_ball |
| 7 | red_block |
| 8 | blue_block |

## 使用方式

配合 `dataset.yaml` 配置文件，可直接用于 YOLOv5/v8/v11 等模型训练：

```bash
# YOLOv8 示例
yolo train data=dataset.yaml model=yolov8n.pt epochs=100 imgsz=640
```
