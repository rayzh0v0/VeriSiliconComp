/*
 * Copyright (c) 2024, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include "vs_conf.h"
#include "platform.h"
#if CONFIG_QEMU_PLATFORM
#include "e203_uart.h"
#else
#include "hal_uart.h"
#endif

#undef putchar
#ifndef CONFIG_QEMU_PLATFORM
static UartDevice *uart_dev;
#endif
/**
 * @todo Replace uart sending function
 */
int putchar(int dat)
{
    if (dat == '\n') {
#if CONFIG_QEMU_PLATFORM
        uart_write(UART0, '\r');
#else
        hal_uart_put_char(uart_dev, '\r');
#endif
    }
#if CONFIG_QEMU_PLATFORM
    uart_write(UART0, dat);
#else
    hal_uart_put_string(uart_dev, (const char *)&dat);
#endif
    return dat;
}

__WEAK ssize_t _write(int fd, const void *ptr, size_t len)
{
    if (!isatty(fd)) {
        return -1;
    }
#ifndef CONFIG_QEMU_PLATFORM
    if (__rv_unlikely(!uart_dev))
        uart_dev = hal_uart_get_device(UART_DEV_ID_0);
#endif

    const uint8_t *writebuf = (const uint8_t *)ptr;
    for (size_t i = 0; i < len; i++) {
        putchar((int)writebuf[i]);
    }
    return len;
}

__WEAK int _close(int fd)
{
    return -1;
}

__WEAK int _fstat(int file, void *st)
{
    return -1;
}

__WEAK int _getpid(void)
{
    return 1;
}

__WEAK int _isatty(int fd)
{
    return 1;
}

__WEAK int _kill(int pid, int sig)
{
    return -1;
}

__WEAK int _lseek(int file, int offset, int whence)
{
    return 0;
}

__WEAK ssize_t _read(int fd, void *ptr, size_t len)
{
    return -1;
}

__WEAK int _gettimeofday(struct timeval *tp, void *tzp)
{
    return 0;
}
