// 定义宏，防止头文件被重复包含
#ifndef __CONV_H__
#define __CONV_H__

// 包含必要的头文件
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "algo_error_code.h" // 包含算法错误代码的头文件

// 定义卷积层输入和输出数据的结构体
typedef struct _Conv2dData {
    uint16_t row; // 行数
    uint16_t col; // 列数
    uint16_t channel; // 通道数
    double *data; // 数据
} Conv2dData;

// 定义卷积层权重的结构体
typedef struct _Conv2dFilter {
    uint16_t row; // 行数
    uint16_t col; // 列数
    uint16_t channel; // 通道数
    uint16_t filter_num; // 过滤器数量
    double *data; // 数据
} Conv2dFilter;

// 定义批量归一化的结构体
typedef struct _BatchNorm2d {
    uint16_t size; // 规模大小
    double *mean; // 均值
    double *var; // 方差
    double *gamma; // 伽马参数
    double *beta; // 贝塔参数
} BatchNorm2d;

// 定义卷积层配置的结构体，包括卷积权重和批量归一化参数
typedef struct _Conv2dConfig {
    uint16_t stride; // 步长
    uint16_t pad; // 填充
    Conv2dFilter *filter; // 过滤器
    BatchNorm2d *bn; // 批量归一化参数
} Conv2dConfig;

// 定义线性层配置的结构体，包括权重和偏置
typedef struct _LinearConfig {
    uint16_t inp_size; // 输入大小
    uint16_t fea_size; // 特征大小
    double *weight; // 权重
    double *bias; // 偏置
} LinearParam;

// 定义卷积层函数，包括卷积操作和批量归一化，不包括偏置
int conv2d_bn_no_bias(Conv2dData *input_feat, Conv2dConfig *param, Conv2dData *output_feat);

// 定义Leaky ReLU激活函数
int leaky_relu(double neg_slope, double *inp, uint16_t inp_size, double *out);

// 定义线性层函数
int linear_layer(double *inp, LinearParam *linear_config, double *out);

// 计算卷积层输出特征图的不同维度的长度
uint16_t cal_conv_out_len(uint16_t raw_len, uint16_t pad_len, uint16_t filter_len, uint16_t stride);

// 结束宏定义
#endif


// /*
//  * Copyright (c) 2024, VeriSilicon Holdings Co., Ltd. All rights reserved
//  *
//  * Redistribution and use in source and binary forms, with or without
//  * modification, are permitted provided that the following conditions are met:
//  *
//  * 1. Redistributions of source code must retain the above copyright notice,
//  * this list of conditions and the following disclaimer.
//  *
//  * 2. Redistributions in binary form must reproduce the above copyright notice,
//  * this list of conditions and the following disclaimer in the documentation
//  * and/or other materials provided with the distribution.
//  *
//  * 3. Neither the name of the copyright holder nor the names of its contributors
//  * may be used to endorse or promote products derived from this software without
//  * specific prior written permission.
//  *
//  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//  */

// #ifndef __CONV_H__
// #define __CONV_H__

// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>
// #include <math.h>

// #include "algo_error_code.h"

// /**
//  * Input and output data of the convolutional layer
//  */
// typedef struct _Conv2dData {
//     uint16_t row;
//     uint16_t col;
//     uint16_t channel;
//     double *data;
// } Conv2dData;

// /**
//  * convolutional layer weights
//  */
// typedef struct _Conv2dFilter {
//     uint16_t row;
//     uint16_t col;
//     uint16_t channel;
//     uint16_t filter_num;
//     double *data;
// } Conv2dFilter;

// /**
//  * Batch Normalization
//  */
// typedef struct _BatchNorm2d {
//     uint16_t size;
//     double *mean;
//     double *var;
//     double *gamma;
//     double *beta;
// } BatchNorm2d;

// /**
//  * configuration of convolutional layers, including convolution weights and BN
//  */
// typedef struct _Conv2dConfig {
//     uint16_t stride;
//     uint16_t pad;
//     Conv2dFilter *filter;
//     BatchNorm2d *bn;
// } Conv2dConfig;

// /**
//  * configuration of the linear layer, including weights and biases
//  */
// typedef struct _LinearConfig {
//     uint16_t inp_size;
//     uint16_t fea_size;
//     double *weight;
//     double *bias;
// } LinearParam;

// /**
//  * @brief conv2d with BN layer without bias
//  *
//  * @param[in] input_feat: input feature map
//  * @param[in] param: configuration of the convolutional layer
//  * @param[out] output_feat: output feature map
//  * @return error code
//  */
// int conv2d_bn_no_bias(Conv2dData *input_feat, Conv2dConfig *param, Conv2dData *output_feat);

// /**
//  * @brief leak_relu activation function
//  *
//  * @param[in] neg_slope: controls the angle of the negative slope
//  * @param[in] inp: input data
//  * @param[in] inp_size: input data size
//  * @param[out] out: output data
//  * @return error code
//  */
// int leaky_relu(double neg_slope, double *inp, uint16_t inp_size, double *out);

// /**
//  * @brief linear layer
//  *
//  * @param[in] inp: input data
//  * @param[in] linear_config: configuration of the linear layer
//  * @param[out] out: output data
//  * @return error code
//  */
// int linear_layer(double *inp, LinearParam *linear_config, double *out);

// /**
//  * @brief calculate the length of different dimensions of the convolutional layer output feature map
//  *
//  * @param[in] raw_len: the length of input feature map
//  * @param[in] pad_len: the length of padding
//  * @param[in] filter_len: kernel size
//  * @param[in] stride: the stride length of conv
//  * @return the length of output feature map
//  */
// uint16_t cal_conv_out_len(uint16_t raw_len, uint16_t pad_len, uint16_t filter_len, uint16_t stride);

// #endif