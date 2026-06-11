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
 
# 相机内参矩阵（庐山派官方标定值，用于 PnP 距离估算）
# 注：如果测距不准，主要调整这个矩阵 —— 需要用标定板重新标定获得本机专属参数
camera_matrix = [1601.79998, 0.0, 960.2537, 0.0, 1600.6784, 496.5050, 0.0, 0.0, 1.0]
# 相机畸变系数（同样来自标定，测距不准时与内参矩阵一起更新）
dist_coeffs = [0.16096, -0.73425, -0.01634, -0.00896, 0.41294]
# 小球实际直径（单位：cm），必须和实物一致，填错会等比例偏差
BALL_REAL_SIZE = 4.3
 
# 主循环
while True:
    # 采集一帧图像
    img = sensor.snapshot()
    # 转为 numpy 数组，供 cv_lite 处理
    img_np = img.to_numpy_ref()
 
    # 查找红色色块
    blobs = cv_lite.rgb888_find_blobs(image_shape, img_np, ball_threshold, 100, 1)
 
    # 如果找到了色块
    if len(blobs) > 0:
        # 取第一个色块作为目标
        x, y, w, h = blobs[0], blobs[1], blobs[2], blobs[3]
        roi = [x, y, w, h]
 
        # 使用 PnP 算法估算球到摄像头的距离（单位：cm）
        distance = cv_lite.rgb888_pnp_distance(image_shape, img_np, roi,
                                               camera_matrix, dist_coeffs, 5,
                                               BALL_REAL_SIZE, BALL_REAL_SIZE)/3
 
        # 计算球心与半径，画圆标记
        cx = x + w // 2
        cy = y + h // 2
        r = min(w, h) // 2
        img.draw_circle(cx, cy, r, color=(0, 255, 0), thickness=2)
        # 在球上方显示距离
        img.draw_string_advanced(x, y - 25, 20,
                                 "%.1f cm" % distance, color=(0, 255, 0))
        # 串口打印（方便调试对比实际距离）
        print("distance = %.1f cm" % distance)
 
    # 显示画面
    Display.show_image(img)
    gc.collect()