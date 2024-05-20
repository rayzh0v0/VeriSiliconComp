import matplotlib.pyplot as plt  # 导入 matplotlib.pyplot 模块，用于绘图
import numpy as np  # 导入 numpy 模块，用于数值计算
from pathlib import Path  # 导入 pathlib 模块，用于处理文件路径

from util import *  # 导入自定义的 util 模块，包含了一些工具函数
from VAD import VAD  # 导入自定义的 VAD 模块，用于语音活动检测
import os

def cal_voice_segment(pred_class, pred_idx_in_data, raw_data_len):
    """
    根据预测结果确定语音段的起始点和结束点。

    :param pred_class: 预测的语音类别，维度为 (N,1)，0 表示非语音，1 表示语音
    :param pred_idx_in_data: 每个预测结果在原始数据中的坐标，维度为 (N,1)
    :param raw_data_len: 原始数据长度
    :return: 语音段的起始点和结束点，维度为 (M,2)，其中每行代表一个语音段，第一列为起始点，第二列为结束点
    """

    if len(pred_class) != len(pred_idx_in_data):  # 如果预测类别和坐标数量不一致，则抛出异常
        raise Exception("pred_class length must be pred_idx_in_data length!")
    
    all_voice_segment = np.array([])  # 存储所有语音段的数组
    single_voice_segment = []  # 存储单个语音段的起始点和结束点
    diff_value = np.diff(pred_class)  # 计算预测类别相邻元素的差值

    for i in range(len(diff_value)):
        if diff_value[i] == 1:  # 如果差值为 1，表示开始语音段
            single_voice_segment.append(pred_idx_in_data[i+1])

        if diff_value[i] == -1:  # 如果差值为 -1，表示结束语音段
            if len(single_voice_segment) == 0:  # 如果单个语音段为空，则添加 0 作为起始点
                single_voice_segment.append(0)
            single_voice_segment.append(pred_idx_in_data[i+1])  # 添加结束点

        if len(single_voice_segment) == 2:  # 如果单个语音段包含起始点和结束点
            if len(all_voice_segment) == 0:  # 如果所有语音段数组为空，则直接赋值为当前单个语音段
                all_voice_segment = np.array(single_voice_segment).reshape(1,-1)
            else:
                all_voice_segment = np.concatenate((all_voice_segment,np.array(single_voice_segment).reshape(1,-1)),axis=0)  # 否则将当前单个语音段添加到所有语音段数组中
            single_voice_segment = []  # 清空单个语音段数组
    
    if len(single_voice_segment) == 1:  # 如果单个语音段只有起始点，没有结束点
        single_voice_segment.append(raw_data_len-1)  # 添加最后一个数据点作为结束点
        all_voice_segment = np.concatenate((all_voice_segment,np.array(single_voice_segment).reshape(1,-1)),axis=0)  # 添加到所有语音段数组中

    return all_voice_segment  # 返回所有语音段的起始点和结束点

def vad_forward(data_dir: str, model_path: str):
    """
    进行语音活动检测并绘制结果图像。

    :param data_dir: 数据目录路径
    :param model_path: 模型文件路径
    """
    vad_model = VAD(model_path=model_path)  # 加载语音活动检测模型

    for file_dir in Path(data_dir).iterdir():  # 遍历数据目录中的文件
        if file_dir.name[-3:] != "wav":  # 如果不是 wav 文件，则跳过
            continue

        signal, signal_len, sample_rate = read_wav(str(file_dir))  # 读取 wav 文件
        print(file_dir, sample_rate)

        signal, signal_len = sample_rate_to_8K(signal, sample_rate)  # 将采样率转换为 8000 Hz

        total_pred = np.array([])  # 存储总的预测结果
        total_indices = np.array([])  # 存储总的坐标
        for i in range(0, signal_len, int(FRAME_STEP * FS)):  # 按帧进行语音活动检测
            if i + FS * FRAME_T - 1 > signal_len:  # 如果超出信号长度，则跳出循环
                break

            tmp_signal = signal[i:int(i+FS * FRAME_T)]  # 截取一帧数据

            pred = vad_model.process(tmp_signal)  # 对当前帧进行语音活动检测

            if total_indices.size == 0:  # 如果坐标数组为空，则直接赋值为当前坐标
                total_indices = np.array(i)
            else:
                total_indices = np.concatenate((total_indices,i),axis=None )  # 否则将当前坐标添加到坐标数组中

            if total_pred.size == 0:  # 如果预测结果数组为空，则直接赋值为当前预测结果
                total_pred = pred.copy()
            else:
                total_pred = np.concatenate((total_pred,pred),axis=None)  # 否则将当前预测结果添加到预测结果数组中

        voice_segment = cal_voice_segment(total_pred, total_indices, signal_len)  # 根据预测结果确定语音段

        plt.figure(1, figsize=(15,7))  # 创建图像窗口
        plt.clf()  # 清除当前图像
        draw_time_domain_image(signal, nframes=signal_len, framerate=sample_rate, line_style="b-")  # 绘制时域图像
        draw_result(signal, voice_segment)  # 绘制检测结果图像

        plt.grid()  # 显示网格
        plt.show()  # 显示图像

if __name__ == "__main__":
    FS = 8000  # 采样率为 8000 Hz
    FRAME_T = 0.03  # 帧时长为 30 毫秒
    FRAME_STEP = 0.015  # 帧步长为 15 毫秒

    WORK_DIR = os.path.dirname(__file__)
    model_path = WORK_DIR + "/model/model_microphone.pth"  # 模型文件路径
    data_dir = WORK_DIR + "/data"  # 数据目录路径

    vad_forward(data_dir=data_dir, model_path=model_path)  # 执行语音活动检测


# import matplotlib.pyplot as plt
# import numpy as np
# from pathlib import Path

# from util import *
# from VAD import VAD

# def cal_voice_segment(pred_class,pred_idx_in_data,raw_data_len):
#     """
#     Determine the start and end points of the vocal segment based on the prediction results.

#     param: pre_calss: (N,1),Predicted calss of human voice. 0: unvoice, 1:voice
#     param: pred_idx_in_data: (N,1),Each prediction result corresponds to the coordinates on the original data
#     param: raw_data_len: the length of raw data
#     return: (M,2), voice segment. (...,0):start point, (...,1):end point
#     """

#     if len(pred_class) != len(pred_idx_in_data):
#         raise Exception("pred_class length must be pred_idx_in_data length!")
    
#     all_voice_segment = np.array([])
#     single_voice_segment = []
#     diff_value = np.diff(pred_class)

#     for i in range(len(diff_value)):
#         if diff_value[i] == 1:
#             single_voice_segment.append(pred_idx_in_data[i+1])

#         if diff_value[i] == -1:
#             if len(single_voice_segment) == 0:
#                 single_voice_segment.append(0)
#             single_voice_segment.append(pred_idx_in_data[i+1])

#         if len(single_voice_segment) == 2:
#             if len(all_voice_segment) == 0:
#                 all_voice_segment = np.array(single_voice_segment).reshape(1,-1)
#             else:
#                 all_voice_segment = np.concatenate((all_voice_segment,np.array(single_voice_segment).reshape(1,-1)),axis=0)
#             single_voice_segment = []
    
#     if len(single_voice_segment) == 1:
#         single_voice_segment.append(raw_data_len-1)
#         all_voice_segment = np.concatenate((all_voice_segment,np.array(single_voice_segment).reshape(1,-1)),axis=0)

#     return all_voice_segment

# def vad_forward(data_dir:str,model_path:str):
#     vad_model = VAD(model_path=model_path)

#     for file_dir in Path(data_dir).iterdir():
#         if file_dir.name[-3:] != "wav":
#             continue

#         signal,signal_len,sample_rate = read_wav(str(file_dir))
#         print(file_dir,sample_rate)

#         signal,signal_len = sample_rate_to_8K(signal,sample_rate)

#         total_pred = np.array([])
#         total_indices = np.array([])
#         for i in range(0,signal_len,int(FRAME_STEP * FS)):
#             if i + FS * FRAME_T - 1 > signal_len:
#                 break

#             tmp_signal = signal[i:int(i+FS * FRAME_T)]

#             pred = vad_model.process(tmp_signal)

#             if total_indices.size == 0:
#                 total_indices = np.array(i)
#             else:
#                 total_indices = np.concatenate((total_indices,i),axis=None )

#             if total_pred.size == 0:
#                 total_pred = pred.copy()
#             else:
#                 total_pred = np.concatenate((total_pred,pred),axis=None)

#         voice_segment = cal_voice_segment(total_pred,total_indices,signal_len)

#         plt.figure(1,figsize=(15,7))
#         plt.clf()
#         draw_time_domain_image(signal,nframes=signal_len,framerate=sample_rate,line_style="b-")
#         draw_result(signal,voice_segment)

#         plt.grid()
#         plt.show()

# if __name__ == "__main__":
#     FS = 8000
#     FRAME_T = 0.03
#     FRAME_STEP = 0.015
#     VOICE_FRAME = 4
#     UNVOICE_FRAME = 8

#     model_path = "./model/model_microphone.pth"
#     data_dir = "./data"

#     vad_forward(data_dir=data_dir,model_path=model_path)