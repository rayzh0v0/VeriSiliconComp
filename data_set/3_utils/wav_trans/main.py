import scipy
from scipy.io import wavfile
import os
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

current_file_directory = os.path.dirname(__file__)
print(current_file_directory)

def draw_time_domain_image(waveData, nframes, framerate, line_style):
    """
    绘制时域图像

    :param waveData: 信号数据
    :param nframes: 帧数
    :param framerate: 帧率
    :param line_style: 曲线样式
    """
    plt.plot(np.arange(0, nframes), waveData, line_style)  # 绘制曲线
    plt.xlabel('point index')  # 设置 x 轴标签
    plt.ylabel('am')  # 设置 y 轴标签
    # plt.show()  # 显示图像（可选）

# 读取wav文件
sample_rate, data = wavfile.read(current_file_directory + '\\input.wav')

# print(sample_rate, data)

# 将立体声更改为单声道
if data.ndim > 1:
    data = data[:, 0]  # 只保留左声道或右声道

# 将采样率变成8000
new_sample_rate = 8000
if new_sample_rate != sample_rate:
    data = scipy.signal.resample(data, int(len(data) * new_sample_rate / sample_rate))

# 保存为新的wav文件
wavfile.write(current_file_directory + '\\output.wav', new_sample_rate, data)

sample_rate, signal = wavfile.read(current_file_directory + '\\output.wav')
signal_len = len(signal)

plt.figure(1, figsize=(15,7))  # 创建图像窗口
plt.clf()  # 清除当前图像
draw_time_domain_image(signal, nframes=signal_len, framerate=sample_rate, line_style="b-")  # 绘制时域图像

plt.grid()  # 显示网格
plt.show()  # 显示图像

############################### 注释以下代码可不生成Excel文件 ###############################
df = pd.DataFrame(signal, columns=['Audio Data'])
df.to_excel('audio_data.xlsx', index=False)

