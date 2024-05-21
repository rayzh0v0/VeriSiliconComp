from scipy.io import wavfile  # 导入 scipy 的 wavfile 模块，用于读取 wav 文件
import matplotlib.pyplot as plt  # 导入 matplotlib.pyplot 模块，用于绘图
import numpy as np  # 导入 numpy 模块，用于数值计算

def read_wav(fname):
    """
    读取 wav 文件

    :param fname: 文件路径
    :return: 信号数据、信号长度、采样率
    """
    fs, signal = wavfile.read(fname)  # 读取 wav 文件，返回采样率和信号数据
    if len(signal.shape) != 1:  # 如果信号维度不为 1，表示为立体声信号，则转换为单声道信号
        print("convert stereo to mono")
        signal = signal[:,0]  # 取左声道数据
    signal = signal.flatten()  # 将信号展平
    signal_len = len(signal)  # 计算信号长度
    return signal, signal_len, fs  # 返回信号数据、信号长度和采样率

def read_txt(file_dir):
    """
    读取文本文件

    :param file_dir: 文件路径
    :return: 数据数组
    """
    data = []
    
    # 读取文本文件
    with open(file_dir, 'r') as file:
        lines = file.readlines()  # 读取所有行
        # 处理数据
        for line in lines:
            # 读取每一行的数据，并转换为整数列表
            values = [int(value) for value in line.strip().split(",")]
            data.append(values)  # 添加到数据列表中

    # 将数据转换为 numpy 数组
    data = np.array(data)

    return data  # 返回数据数组

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

def draw_result(raw_data, pred):
    """
    绘制检测结果图像

    :param raw_data: 原始信号数据
    :param pred: 预测结果，包含起始点和结束点
    """
    data_len = len(raw_data)  # 原始数据长度

    x_value = np.zeros(data_len)  # 初始化结果数组
    for i in range(pred.shape[0]):  # 遍历每个预测结果
            x_value[pred[i][0]:pred[i][1]+1] = 1  # 将预测结果范围内的值设为 1
    
    plt.plot(np.arange(0, data_len), x_value * np.max(raw_data), "r-")  # 绘制结果图像

def sample_rate_to_8K(signal, sample_rate):
    """
    将采样率转换为 8000 Hz

    :param signal: 原始信号数据
    :param sample_rate: 原始采样率
    :return: 转换后的信号数据、信号长度
    """
    if sample_rate not in [8000, 16000, 24000, 48000]:  # 如果采样率不在指定范围内，则抛出异常
        raise Exception("sample rate not in [8000,16000,48000]!")
    
    interval = int(sample_rate / 8000)  # 计算采样间隔
    sample_signal = signal[0::interval]  # 根据采样间隔取样，原型是sequence[start:stop:step]
    
    signal_len = len(sample_signal)  # 计算采样后的信号长度

    return sample_signal, signal_len  # 返回采样后的信号数据和长度


# from scipy.io import wavfile
# import matplotlib.pyplot as plt
# import numpy as np

# def read_wav(fname):
#     fs, signal = wavfile.read(fname)
#     if len(signal.shape) != 1:
#         print("convert stereo to mono")
#         signal = signal[:,0]
#     signal = signal.flatten()
#     signal_len = len(signal)
#     return signal,signal_len,fs

# def read_txt(file_dir):
#     data = []
    
#     # read txt
#     with open(file_dir, 'r') as file:
#         lines = file.readlines()
#         # process data
#         for line in lines:
#             # read data
#             values = [int(value) for value in line.strip().split(",")]
#             data.append(values)

#     # set data format as numpy
#     data = np.array(data)

#     return data

# def draw_time_domain_image(waveData, nframes, framerate, line_style):
#     plt.plot(np.arange(0,nframes),waveData,line_style)
#     plt.xlabel('point index')
#     plt.ylabel('am')
#     # plt.show()

# def draw_result(raw_data,pred):
#     """
#     raw_data: (N,)
#     pred: predict result,(M,2),for col:(start point,end point)
#     """

#     data_len = len(raw_data)

#     x_value = np.zeros(data_len)
#     for i in range(pred.shape[0]):
#             x_value[pred[i][0]:pred[i][1]+1] = 1
    
#     plt.plot(np.arange(0,data_len),x_value * np.max(raw_data),"r-")


# def sample_rate_to_8K(signal,sample_rate):
#     if sample_rate not in [8000,16000,24000,48000]:
#         raise Exception("sample rate not in [8000,16000,48000]!")
    
#     interval = int(sample_rate / 8000)
#     sample_signal = signal[0::interval]

#     signal_len = len(sample_signal)

#     return sample_signal,signal_len
