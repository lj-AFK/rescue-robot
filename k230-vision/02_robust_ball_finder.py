# 导入所需模块
import time, gc
from media.sensor import *
from media.display import *
from media.media import *
import cv_lite

# 图像尺寸 [高, 宽]，16:9 比例，画面无变形
image_shape = [360, 640]

# 初始化摄像头：GC2093 原生 1920x1080 @ 60fps，ISP 下采样到 640x360
sensor = Sensor(id=2, width=1920, height=1080, fps=60)
sensor.reset()
sensor.set_framesize(width=640, height=360)
sensor.set_pixformat(Sensor.RGB888)

# 初始化显示
Display.init(Display.VIRT, to_ide=True)
sensor.run()

# 红色小球 RGB 阈值 [R_min, R_max, G_min, G_max, B_min, B_max]
ball_threshold = [120, 255, 0, 80, 0, 80]

# 画面中心坐标，用于计算球相对画面中心的偏差
CENTER_X = 320
CENTER_Y = 180

# 主循环
while True:
    # 采集一帧图像
    img = sensor.snapshot()
    # 转为 numpy 数组，供 cv_lite 处理
    img_np = img.to_numpy_ref()

    # 高斯滤波去噪
    blurred = cv_lite.rgb888_gaussian_blur_fast(image_shape, img_np, 3)
    # 查找红色色块
    blobs = cv_lite.rgb888_find_blobs(image_shape, blurred, ball_threshold, 100, 1)

    # 初始化"最大球"信息：没找到时 area=0
    best_x, best_y, best_w, best_h, best_area = 0, 0, 0, 0, 0

    # 遍历所有色块，找出最大的圆形（最大 = 最近）
    for i in range(len(blobs) // 4):
        x, y, w, h = blobs[4*i : 4*i+4]
        # 跳过尺寸无效的块
        if w == 0 or h == 0:
            continue
        # 长宽比过滤：只保留接近正方形（即圆形）的块
        if not (0.7 < w / h < 1.4):
            continue
        # 记录面积最大的块
        area = w * h
        if area > best_area:
            best_x, best_y, best_w, best_h, best_area = x, y, w, h, area

    # 如果找到了球
    if best_area > 0:
        # 计算球心坐标
        cx = best_x + best_w // 2
        cy = best_y + best_h // 2
        # 计算偏差（球心相对画面中心），正值表示偏右/偏下
        err_x = cx - CENTER_X
        err_y = cy - CENTER_Y

        # 画圆标记球
        r = min(best_w, best_h) // 2
        img.draw_circle(cx, cy, r, color=(255, 0, 0), thickness=2)
        # 画十字标记球心
        img.draw_cross(cx, cy, color=(0, 255, 0), size=8, thickness=2)

        # 打印结果（后续可通过串口发给小车主控）
        # err_x 控制小车转向，best_h 判断距离（越大越近）
        print("found: err_x={} err_y={} ball_h={}".format(err_x, err_y, best_h))
    else:
        # 没找到球
        print("no ball")

    # 显示画面
    Display.show_image(img)
    gc.collect()