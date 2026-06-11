import time, gc
from media.sensor import *
from media.display import *
from media.media import *
import cv_lite

# 庐山派标准配置
image_shape = [480, 640]
sensor = Sensor(id=2, width=1280, height=720, fps=90)
sensor.reset()
sensor.set_framesize(width=image_shape[1], height=image_shape[0])
sensor.set_pixformat(Sensor.RGB888)
Display.init(Display.VIRT, to_ide=True)
sensor.run()

clock = time.clock()
exposure_gain = 1.5 # 调节此参数以适应不同室内光线

while True:
    clock.tick()
    img = sensor.snapshot()
    img_np = img.to_numpy_ref()

    # 1. 调整曝光以增强画面亮度
    exposed_np = cv_lite.rgb888_adjust_exposure_fast(image_shape, img_np, exposure_gain)
    
    # 2. 应用快速白平衡，消除环境光偏色
    balanced_np = cv_lite.rgb888_white_balance_gray_world_fast(image_shape, exposed_np)

    img_out = image.Image(image_shape[1], image_shape[0], image.RGB888, alloc=image.ALLOC_REF, data=balanced_np)
    Display.show_image(img_out)
    gc.collect()