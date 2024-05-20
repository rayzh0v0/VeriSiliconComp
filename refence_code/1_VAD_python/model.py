import torch  # 导入 PyTorch 深度学习框架
import torch.nn as nn  # 导入 PyTorch 神经网络模块

class CNN(nn.Module):  # 声明一个 CNN（卷积神经网络）模型类，继承自 nn.Module
    def __init__(self, input_channel=1, n_channel=2, kernel_size=2,
                stride=2, dilation=1, padding="valid") -> None:
        """
        初始化 CNN 模型

        :param input_channel: 输入通道数，默认为 1
        :param n_channel: 卷积核数量，默认为 2
        :param kernel_size: 卷积核大小，默认为 2
        :param stride: 卷积步长，默认为 2
        :param dilation: 卷积扩张率，默认为 1
        :param padding: 填充方式，默认为 "valid"
        """
        super(CNN,self).__init__()  # 调用父类的初始化方法

        self.fc_size = 120 * 2  # 全连接层输入大小
        model = nn.Sequential(  # 定义模型结构
            nn.Conv2d(in_channels=input_channel,out_channels=n_channel,kernel_size=(1,kernel_size),
                      stride=stride,dilation=dilation,padding=padding,bias=False),  # 一维卷积层
            nn.BatchNorm2d(num_features=n_channel),  # 批标准化层
            nn.LeakyReLU(inplace=True),  # 激活函数层
        )
        self.model = model  # 保存模型结构
        self.output = nn.Linear(in_features=self.fc_size,out_features=2)  # 全连接输出层

    def forward(self,x):
        """
        CNN 模型前向传播

        :param x: 输入数据
        :return: 模型输出
        """
        x = self.model(x)  # 前向传播
        x = x.view(x.size(0),-1)  # 将输出展平
        output = self.output(x)  # 全连接输出
        return output  # 返回模型输出

# import torch
# import torch.nn as nn

# class CNN(nn.Module):
#     def __init__(self, input_channel=1, n_channel=2, kernel_size=2,
#                 stride=2, dilation=1, padding="valid") -> None:
#         super(CNN,self).__init__()

#         self.fc_size = 120 * 2
#         model = nn.Sequential(
#             nn.Conv2d(in_channels=input_channel,out_channels=n_channel,kernel_size=(1,kernel_size),
#                       stride=stride,dilation=dilation,padding=padding,bias=False),
#             nn.BatchNorm2d(num_features=n_channel),
#             nn.LeakyReLU(inplace=True),
#         )
#         self.model = model
#         self.output = nn.Linear(in_features=self.fc_size,out_features=2)

#     def forward(self,x):
#         x = self.model(x)
#         x = x.view(x.size(0),-1)
#         output = self.output(x)
#         return output
