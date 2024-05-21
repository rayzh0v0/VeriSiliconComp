import matplotlib.pyplot as plt  # 导入 matplotlib.pyplot 模块，用于绘图
import numpy as np  # 导入 numpy 模块，用于数值计算
from util import *  # 导入自定义的 util 模块，包含了一些工具函数
import os
import glob

WORK_DIR = os.path.dirname(__file__)
WAV_NAME = glob.glob(os.path.join(WORK_DIR, '*.wav'))
TXT_NAME = glob.glob(os.path.join(WORK_DIR, '*.txt'))

print(WAV_NAME[0], TXT_NAME[0])

if __name__ == "__main__":

    signal, signal_len, sample_rate = read_wav(WAV_NAME[0])  # 读取 wav 文件
    print(signal_len, sample_rate)

    # 读取数据
    data = np.genfromtxt(TXT_NAME[0], delimiter=',', dtype=int)

    # 转换为(M,2)维数组
    voice_segment = data.reshape(-1, 2)

    plt.figure(1, figsize=(15,7))  # 创建图像窗口
    plt.clf()  # 清除当前图像
    draw_time_domain_image(signal, nframes=signal_len, framerate=sample_rate, line_style="b-")  # 绘制时域图像
    draw_result(signal, voice_segment)  # 绘制检测结果图像

    plt.grid()  # 显示网格
    plt.show()  # 显示图像
