# 导入所需模块
import time, gc
from media.sensor import *
from media.display import *
from media.media import *
import cv_lite

# 图像尺寸 [高, 宽]
image_shape = [360, 640]
IMG_W = 640
IMG_H = 360

# 初始化摄像头
sensor = Sensor(id=2, width=1920, height=1080, fps=60)
sensor.reset()
sensor.set_framesize(width=IMG_W, height=IMG_H)
sensor.set_pixformat(Sensor.RGB888)

# 初始化显示
Display.init(Display.VIRT, to_ide=True)
sensor.run()

# ==================== 圆形采样区设置 ====================
# 采样圆的圆心：画面正中央
SAMPLE_CX = IMG_W // 2
SAMPLE_CY = IMG_H // 2
# 采样圆的半径（像素）
SAMPLE_R = 25
# 半径平方，用于快速判断点是否在圆内
SAMPLE_R2 = SAMPLE_R * SAMPLE_R

# ==================== 性能优化参数 ====================
# 采样步长：每隔 STEP 个像素采一次，降低计算量
# STEP=3 时，50x50 区域只采样约 280 个点（原来 2500 个），速度快 8 倍
# 对阈值统计来说，稀疏采样和全采样的结果几乎一样
STEP = 3
# 每 N 帧更新一次阈值计算，中间帧只刷新画面，保证显示流畅
UPDATE_INTERVAL = 2

# ==================== 阈值缓冲 ====================
# 在采样 min/max 基础上扩展的余量，应对光照微变
MARGIN = 10

# ==================== 初始化状态变量 ====================
# 保存上一次计算的阈值，未更新的帧直接用这些值显示
r_lo, r_hi = 0, 255
g_lo, g_hi = 0, 255
b_lo, b_hi = 0, 255
r_avg, g_avg, b_avg = 0, 0, 0
frame_count = 0

# 主循环
while True:
    # 采集一帧图像
    img = sensor.snapshot()
    img_np = img.to_numpy_ref()
    frame_count += 1

    # -------- 只在间隔帧做耗时的像素统计 --------
    if frame_count % UPDATE_INTERVAL == 0:
        # 初始化 min/max 和累加器
        r_min, g_min, b_min = 255, 255, 255
        r_max, g_max, b_max = 0, 0, 0
        r_sum, g_sum, b_sum, count = 0, 0, 0, 0

        # 按 STEP 步长稀疏采样圆内的点
        for py in range(SAMPLE_CY - SAMPLE_R, SAMPLE_CY + SAMPLE_R, STEP):
            # 预计算 y 方向偏差平方，减少内循环计算
            dy2 = (py - SAMPLE_CY) ** 2
            for px in range(SAMPLE_CX - SAMPLE_R, SAMPLE_CX + SAMPLE_R, STEP):
                # 判断是否在圆内
                dx = px - SAMPLE_CX
                if dx * dx + dy2 > SAMPLE_R2:
                    continue
                # 取像素 RGB 值
                pixel = img_np[py][px]
                r, g, b = pixel[0], pixel[1], pixel[2]
                # 更新 min/max
                if r < r_min: r_min = r
                if r > r_max: r_max = r
                if g < g_min: g_min = g
                if g > g_max: g_max = g
                if b < b_min: b_min = b
                if b > b_max: b_max = b
                # 累加算均值
                r_sum += r
                g_sum += g
                b_sum += b
                count += 1

        # 计算带 MARGIN 的最终阈值
        if count > 0:
            r_lo = max(0, r_min - MARGIN)
            r_hi = min(255, r_max + MARGIN)
            g_lo = max(0, g_min - MARGIN)
            g_hi = min(255, g_max + MARGIN)
            b_lo = max(0, b_min - MARGIN)
            b_hi = min(255, b_max + MARGIN)
            r_avg = r_sum // count
            g_avg = g_sum // count
            b_avg = b_sum // count

        # 串口打印，方便复制
        print("threshold = [%d, %d, %d, %d, %d, %d]" %
              (r_lo, r_hi, g_lo, g_hi, b_lo, b_hi))

    # -------- 每帧都刷新画面显示（快） --------
    # 采样圆（白色）
    img.draw_circle(SAMPLE_CX, SAMPLE_CY, SAMPLE_R,
                    color=(255, 255, 255), thickness=2)
    # 圆心十字
    img.draw_cross(SAMPLE_CX, SAMPLE_CY, color=(255, 0, 0), size=8, thickness=1)

    # 顶部显示均值
    img.draw_string_advanced(5, 5, 16,
                             "AVG R:%d G:%d B:%d" % (r_avg, g_avg, b_avg),
                             color=(255, 255, 255))

    # 底部显示阈值（复制到主程序用）
    img.draw_string_advanced(5, IMG_H - 50, 16,
                             "threshold = [",
                             color=(0, 255, 0))
    img.draw_string_advanced(5, IMG_H - 28, 16,
                             "%d,%d, %d,%d, %d,%d]" %
                             (r_lo, r_hi, g_lo, g_hi, b_lo, b_hi),
                             color=(0, 255, 0))

    Display.show_image(img)
    gc.collect()

# ============================================================
# 【性能优化说明】
# 1. STEP=3 稀疏采样：采样点数降为原来的 1/9，阈值精度基本无损
# 2. UPDATE_INTERVAL=2 隔帧计算：显示帧率翻倍，阈值数据依然每 2 帧更新
# 3. dy2 预计算：内循环少做一次平方运算
# 4. 局部变量 pixel = img_np[py][px]：减少多维数组索引次数
#
# 如果还是卡，把 STEP 调成 5，UPDATE_INTERVAL 调成 3，进一步降低计算量。
# 如果想要绝对流畅，把 SAMPLE_R 调小到 15（采样区小一点）。
# ============================================================