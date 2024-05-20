import torch  # 导入 PyTorch 深度学习框架
from model import CNN  # 从另一个文件中导入 CNN 模型类
from pathlib import Path  # 导入 Path 类，用于处理文件路径

class VAD(object):  # 声明一个声音活动检测器的类
    def __init__(self,model_path=None,sample_rate=8000,frame_len=0.03) -> None:
        """
        初始化声音活动检测器的实例

        :param model_path: 模型文件路径
        :param sample_rate: 采样率，默认为 8000 Hz
        :param frame_len: 每帧的时间长度，单位为秒，默认为 0.03 秒
        """
        if sample_rate != 8000:  # 如果采样率不是 8000 Hz，则抛出异常
            raise Exception("sampel rate must be 8000 Hz!")

        if (model_path == None) or (not Path(model_path).exists()):  # 如果模型文件路径为空或路径不存在，则抛出异常
            raise Exception("model path not exists!")

        self.model = None  # 初始化模型为空
        self.sample_rate = sample_rate  # 设置采样率
        self.frame_len = self.frame_len = int(frame_len * self.sample_rate)  # 设置每帧的长度

        self.__load_model__(model_path=model_path)  # 加载模型

    def __load_model__(self,model_path):
        """
        加载模型

        :param model_path: 模型文件路径
        :return: 加载后的模型
        """
        self.model = CNN()  # 创建一个 CNN 模型实例
        self.model.load_state_dict(torch.load(model_path))  # 加载模型参数
        self.model.eval()  # 设置模型为评估模式

    def process(self,inp_data):
        """
        处理音频数据，判断音频中是否有语音活动

        :param inp_data: 输入音频数据，形状为(sample_N,)
        :return: 预测结果（音频中是否有语音活动）
        """

        if len(inp_data.shape) > 1:  # 如果输入数据维度大于 1，则抛出异常
            raise Exception("data shape should be (sample_N,)!")

        if len(inp_data) < self.frame_len:  # 如果输入数据长度小于每帧的长度，则抛出异常
            raise Exception("input length must be %d".format(self.frame_len))

        frame_data = torch.from_numpy(inp_data).float()  # 将输入数据转换成 PyTorch 张量格式
        frame_data = frame_data.unsqueeze(0).unsqueeze(0).unsqueeze(0)  # 调整张量形状以符合模型输入要求

        model_output = self.model(frame_data)  # 使用模型进行预测
        pred = torch.max(model_output,1)[1].data.numpy()  # 获取预测结果

        return pred  # 返回预测结果

# import torch
# from model import CNN
# from pathlib import Path

# class VAD(object):
#     def __init__(self,model_path=None,sample_rate=8000,frame_len=0.03) -> None:
#         """
#         :param model_path: model file dir
#         :param frame_len: Length of time per frame (s)
#         """
#         if sample_rate != 8000:
#             raise Exception("sampel rate must be 8000 Hz!")

#         if (model_path == None) or (not Path(model_path).exists()):
#             raise Exception("model path not exists!")

#         self.model = None
#         self.sample_rate = sample_rate
#         self.frame_len = self.frame_len = int(frame_len * self.sample_rate)

#         self.__load_model__(model_path=model_path)

#     def __load_model__(self,model_path):
#         """load model

#         :param model_path: model file dir
#         :return: The model after loading
#         """
#         self.model = CNN()
#         self.model.load_state_dict(torch.load(model_path))
#         self.model.eval()

#     def process(self,inp_data):
#         """The actual processing of the Voice Activity Detector, It is processed frame by frame and the results are smoothed

#         :param inp_data: Input audio data,(sample_N,)
#         :return Current output audio data(N,), the label that can be output currently(M,),The position corresponding to the starting point of each frame(M,)
#         """

#         if len(inp_data.shape) > 1:
#             raise Exception("data shape should be (sample_N,)!")

#         if len(inp_data) < self.frame_len:
#             raise Exception("input length must be %d".format(self.frame_len))

#         frame_data = torch.from_numpy(inp_data).float()
#         frame_data = frame_data.unsqueeze(0).unsqueeze(0).unsqueeze(0)

#         model_output = self.model(frame_data)
#         pred = torch.max(model_output,1)[1].data.numpy()

#         return pred
