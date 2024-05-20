// 包含头文件
#include "conv.h"

// 定义批量归一化中的小常数
#define BN_EPS (1e-5)

// 定义一个静态函数，用于在输入数据周围填充值
static void padding_value(const Conv2dData *raw_data, uint16_t pad_len, double pad_value,
                          double *paded_data) {
    // 一些局部变量的定义和初始化
    uint16_t row = raw_data->row, col = raw_data->col, chan = raw_data->channel;
    uint16_t paded_data_size = 0;
    uint16_t i = 0, j = 0, k = 0;
    uint16_t pad_idx = 0;

    // 计算填充后的数据大小
    paded_data_size = (row + 2 * pad_len) * (col + 2 * pad_len) * chan;

    // 将填充后的数据全部设置为填充值
    for (i = 0; i < paded_data_size; i++) {
        paded_data[i] = pad_value;
    }

    // 将原始数据复制到填充后的数据中正确的位置
    for (i = 0; i < raw_data->channel; i++) {
        for (j = 0; j < raw_data->row; j++) {
            for (k = 0; k < raw_data->col; k++) {
                pad_idx = k + pad_len + (j + pad_len) * (col + 2 * pad_len) +
                          i * (row + 2 * pad_len) * (col + 2 * pad_len);
                paded_data[pad_idx] = raw_data->data[k + j * col + i * row * col];
            }
        }
    }
}

// 计算卷积层输出特征图的长度
uint16_t cal_conv_out_len(uint16_t raw_len, uint16_t pad_len, uint16_t filter_len, uint16_t stride) {
    return (raw_len + 2 * pad_len - filter_len) / stride + 1;
}

// 实现卷积层和批量归一化操作，不包括偏置
int conv2d_bn_no_bias(Conv2dData *input_feat, Conv2dConfig *param, Conv2dData *output_feat) {
    // 一些局部变量的定义和初始化
    uint16_t i = 0, j = 0, k = 0, ii = 0, jj = 0, kk = 0, group_cnt = 0;
    uint16_t out_row = 0, out_col = 0, out_chan = 0;
    uint16_t paded_row = 0, paded_col = 0, paded_feat_size = 0;
    uint16_t row_start = 0, col_start = 0, filter_idx = 0, feat_idx = 0, output_feat_idx = 0;
    BatchNorm2d *bn = NULL;
    Conv2dFilter *filter = NULL;

    double tmp = 0.0;
    double *paded_feat = NULL;

    // 检查输入参数是否为空，如果为空则返回错误代码
    if (!input_feat || !input_feat->data || !param || !param->bn || !param->bn->mean ||
        !param->bn->var || !param->bn->gamma || !param->bn->beta || !param->filter ||
        !param->filter->data || !output_feat || !output_feat->data) {
        return ALGO_POINTER_NULL;
    }

    // 获取卷积层和批量归一化的参数
    bn = param->bn;
    filter = param->filter;

    // 检查输入参数是否合法，如果不合法则返回错误代码
    if (param->stride < 1 || input_feat->channel != filter->channel ||
        filter->filter_num != bn->size || filter->row > 2 * param->pad + input_feat->row ||
        filter->col > 2 * param->pad + input_feat->col) {
        return ALGO_DATA_EXCEPTION;
    }

    // 计算卷积层输出特征图的大小
    if (input_feat->row == 1) {
        out_row = 1;
    } else {
        out_row = cal_conv_out_len(input_feat->row, param->pad, filter->row, param->stride);
    }

    out_col = cal_conv_out_len(input_feat->col, param->pad, filter->col, param->stride);
    out_chan = filter->filter_num;

    // 如果需要，进行数据填充
    paded_row = input_feat->row + 2 * param->pad;
    paded_col = input_feat->col + 2 * param->pad;
    paded_feat = input_feat->data;
    if (param->pad != 0) {
        paded_feat_size = paded_row * paded_col * input_feat->channel;
        paded_feat = (double *)malloc(sizeof(double) * paded_feat_size);
        if (!paded_feat) {
            return ALGO_MALLOC_FAIL;
        }
        memset((void *)paded_feat, 0, sizeof(double) * paded_feat_size);
        padding_value(input_feat, param->pad, 0.0, paded_feat);
    }

    // 计算卷积操作
    for (i = 0; i < out_chan; i++) {
        for (j = 0; j < out_row; j++) {
            for (k = 0; k < out_col; k++) {
                tmp = 0.0;
                row_start = j * param->stride;
                col_start = k * param->stride;
                for (ii = 0; ii < filter->channel; ii++) {
                    for (jj = 0; jj < filter->row; jj++) {
                        for (kk = 0; kk < filter->col; kk++) {
                            filter_idx = kk + jj * filter->col + ii * filter->row * filter->col +
                                         i * filter->row * filter->col * filter->channel;
                            feat_idx = col_start + kk + (row_start + jj) * paded_col +
                                       ii * paded_row * paded_col;
                            tmp += filter->data[filter_idx] * paded_feat[feat_idx];
                        }
                    }
                }

                // 执行批量归一化操作
                tmp = bn->gamma[i] * (tmp - bn->mean[i]) / sqrt(bn->var[i] + BN_EPS) + bn->beta[i];

                // 将结果保存到输出特征图中
                output_feat_idx = k + j * out_col + i * out_row * out_col;
                output_feat->data[output_feat_idx] = tmp;
            }
        }
    }

    // 设置输出特征图的大小
    output_feat->row = out_row;
    output_feat->col = out_col;
    output_feat->channel = out_chan;

    // 如果进行了数据填充，释放填充后的数据
    if (param->pad != 0) {
        free(paded_feat);
    }

    // 返回正常的错误代码
    return ALGO_NORMAL;
}

int leaky_relu(double neg_slope, double *inp, uint16_t inp_size, double *out)
{
    uint16_t i = 0;

    if (!inp || !out) {
        return ALGO_POINTER_NULL;
    }

    for (i = 0; i < inp_size; i++) {
        out[i] = inp[i];

        if (inp[i] < 0) {
            out[i] = neg_slope * inp[i];
        }
    }

    return ALGO_NORMAL;
}

int linear_layer(double *inp, LinearParam *linear_config, double *out)
{
    uint16_t i, j;

    // 检查输入指针是否为空，如果为空则返回错误代码 ALGO_POINTER_NULL
    if (!inp || !linear_config || !linear_config->weight || !linear_config->bias || !out) {
        return ALGO_POINTER_NULL;
    }

    // 对每个特征进行线性变换
    for (i = 0; i < linear_config->fea_size; i++) {
        // 初始化输出为偏置项
        out[i] = linear_config->bias[i];
        // 对输入向量的每个元素进行线性组合
        for (j = i * linear_config->inp_size; j < (i + 1) * linear_config->inp_size; j++) {
            // 对输入和权重进行点乘，然后累加到输出中
            out[i] += inp[j - i * linear_config->inp_size] * linear_config->weight[j];
        }
    }

    return ALGO_NORMAL; // 返回正常执行代码
}


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

// #include "conv.h"

// #define BN_EPS (1e-5)

// static void padding_value(const Conv2dData *raw_data, uint16_t pad_len, double pad_value,
//                           double *paded_data)
// {
//     uint16_t row = raw_data->row, col = raw_data->col, chan = raw_data->channel;
//     uint16_t paded_data_size = 0;
//     uint16_t i = 0, j = 0, k = 0;
//     uint16_t pad_idx = 0;

//     paded_data_size = (row + 2 * pad_len) * (col + 2 * pad_len) * chan;

//     for (i = 0; i < paded_data_size; i++) {
//         paded_data[i] = pad_value;
//     }

//     for (i = 0; i < raw_data->channel; i++) {
//         for (j = 0; j < raw_data->row; j++) {
//             for (k = 0; k < raw_data->col; k++) {
//                 pad_idx = k + pad_len + (j + pad_len) * (col + 2 * pad_len) +
//                           i * (row + 2 * pad_len) * (col + 2 * pad_len);
//                 paded_data[pad_idx] = raw_data->data[k + j * col + i * row * col];
//             }
//         }
//     }
// }

// uint16_t cal_conv_out_len(uint16_t raw_len, uint16_t pad_len, uint16_t filter_len, uint16_t stride)
// {
//     return (raw_len + 2 * pad_len - filter_len) / stride + 1;
// }

// int conv2d_bn_no_bias(Conv2dData *input_feat, Conv2dConfig *param, Conv2dData *output_feat)
// {
//     uint16_t i = 0, j = 0, k = 0, ii = 0, jj = 0, kk = 0, group_cnt = 0;
//     uint16_t out_row = 0, out_col = 0, out_chan = 0;
//     uint16_t paded_row = 0, paded_col = 0, paded_feat_size = 0;
//     uint16_t row_start = 0, col_start = 0, filter_idx = 0, feat_idx = 0, output_feat_idx = 0;
//     BatchNorm2d *bn      = NULL;
//     Conv2dFilter *filter = NULL;

//     double tmp         = 0.0;
//     double *paded_feat = NULL;

//     if (!input_feat || !input_feat->data || !param || !param->bn || !param->bn->mean ||
//         !param->bn->var || !param->bn->gamma || !param->bn->beta || !param->filter ||
//         !param->filter->data || !output_feat || !output_feat->data) {
//         return ALGO_POINTER_NULL;
//     }

//     bn     = param->bn;
//     filter = param->filter;

//     if (param->stride < 1 || input_feat->channel != filter->channel ||
//         filter->filter_num != bn->size || filter->row > 2 * param->pad + input_feat->row ||
//         filter->col > 2 * param->pad + input_feat->col) {
//         return ALGO_DATA_EXCEPTION;
//     }

//     if (input_feat->row == 1) {
//         out_row = 1;
//     } else {
//         out_row = cal_conv_out_len(input_feat->row, param->pad, filter->row, param->stride);
//     }

//     out_col  = cal_conv_out_len(input_feat->col, param->pad, filter->col, param->stride);
//     out_chan = filter->filter_num;

//     // padding 0
//     paded_row  = input_feat->row + 2 * param->pad;
//     paded_col  = input_feat->col + 2 * param->pad;
//     paded_feat = input_feat->data;
//     if (param->pad != 0) {
//         paded_feat_size = paded_row * paded_col * input_feat->channel;
//         paded_feat      = (double *)malloc(sizeof(double) * paded_feat_size);
//         if (!paded_feat) {
//             return ALGO_MALLOC_FAIL;
//         }
//         memset((void *)paded_feat, 0, sizeof(double) * paded_feat_size);
//         padding_value(input_feat, param->pad, 0.0, paded_feat);
//     }

//     // conv calculate
//     for (i = 0; i < out_chan; i++) {
//         for (j = 0; j < out_row; j++) {
//             for (k = 0; k < out_col; k++) {
//                 tmp       = 0.0;
//                 row_start = j * param->stride;
//                 col_start = k * param->stride;
//                 for (ii = 0; ii < filter->channel; ii++) {
//                     for (jj = 0; jj < filter->row; jj++) {
//                         for (kk = 0; kk < filter->col; kk++) {
//                             filter_idx = kk + jj * filter->col + ii * filter->row * filter->col +
//                                          i * filter->row * filter->col * filter->channel;
//                             feat_idx = col_start + kk + (row_start + jj) * paded_col +
//                                        ii * paded_row * paded_col;
//                             tmp += filter->data[filter_idx] * paded_feat[feat_idx];
//                         }
//                     }
//                 }

//                 tmp = bn->gamma[i] * (tmp - bn->mean[i]) / sqrt(bn->var[i] + BN_EPS) + bn->beta[i];

//                 output_feat_idx                    = k + j * out_col + i * out_row * out_col;
//                 output_feat->data[output_feat_idx] = tmp;
//             }
//         }
//     }

//     output_feat->row     = out_row;
//     output_feat->col     = out_col;
//     output_feat->channel = out_chan;

//     if (param->pad != 0) {
//         free(paded_feat);
//     }

//     return ALGO_NORMAL;
// }

// int leaky_relu(double neg_slope, double *inp, uint16_t inp_size, double *out)
// {
//     uint16_t i = 0;

//     if (!inp || !out) {
//         return ALGO_POINTER_NULL;
//     }

//     for (i = 0; i < inp_size; i++) {
//         out[i] = inp[i];

//         if (inp[i] < 0) {
//             out[i] = neg_slope * inp[i];
//         }
//     }

//     return ALGO_NORMAL;
// }

// int linear_layer(double *inp, LinearParam *linear_config, double *out)
// {
//     uint16_t i, j;

//     if (!inp || !linear_config || !linear_config->weight || !linear_config->bias || !out) {
//         return ALGO_POINTER_NULL;
//     }

//     for (i = 0; i < linear_config->fea_size; i++) {
//         out[i] = linear_config->bias[i];
//         for (j = i * linear_config->inp_size; j < (i + 1) * linear_config->inp_size; j++) {
//             out[i] += inp[j - i * linear_config->inp_size] * linear_config->weight[j];
//         }
//     }

//     return ALGO_NORMAL;
// }
