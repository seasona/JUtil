/**
 * @file os.h
 * @author Ji Jie (396438446@qq.com)
 * @brief The different API in linux and windows
 * @version 0.1
 * @date 2019-09-27
 *
 * @copyright Copyright (c) 2019
 *
 */

#pragma once

#ifdef _WIN32
#include <io.h>
#include <process.h>
#include <windows.h>
#elif __linux__
#include <sys/types.h>
#include <syscall.h>
#include <unistd.h>
#endif

namespace OS {

/**
 * @brief Get the Process Id object
 *
 * @return unsigned long Return current process id as unsigned long
 */
inline unsigned long getProcessId() {
#ifdef _WIN32
    return static_cast<unsigned long>(::GetCurrentProcessId());
#elif __linux__
    return static_cast<unsigned long>(::getpid());
#endif
}

/**
 * @brief It exists because the std::this_thread::get_id() is much slower
 *
 * @return size_t Return current thread id as size_t
 */
inline size_t getThreadId() noexcept {
#ifdef _WIN32
    return static_cast<size_t>(::GetCurrentThreadId());
#elif __linux__
    return static_cast<size_t>(syscall(SYS_gettid));
#endif
}

/**
 * @brief The original localtime in C is much slower and non thread safe,
 * so use the os'function instead
 *
 * @param t The current time, usually got by now()
 * @return std::tm The tm struct
 */
inline std::tm localTime(const time_t &t) {
#ifdef _WIN32
    std::tm p_time;
    localtime_s(&p_time, &t);
#elif __linux__
    std::tm p_time;
    localtime_r(&t, &p_time);
#endif
    return p_time;
}

/**
 * @brief BMP format picture called bitmap, it just occurred in windows.
 * You can use it in c/c++ file like:
 *
 * {
 *      BITMAPFILEHEADER file_head;
 *      file_head.bfType = 0x4D42; //bmp format
 *      ...
 * }
 *
 * You can also watch, read and write BMP file in linux, but there
 * is no BITMAPFILEHEADER and BITMAPINFOHEADER in linux, use the struct 
 * defined, you can use it like in windows
 *
 */
#ifdef __linux__
typedef struct BITMAPFILEHEADER {
    u_int16_t bfType;
    u_int32_t bfSize;
    u_int16_t bfReserved1;
    u_int16_t bfReserved2;
    u_int32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER {
    u_int32_t biSize;
    u_int32_t biWidth;
    u_int32_t biHeight;
    u_int16_t biPlanes;
    u_int16_t biBitCount;
    u_int32_t biCompression;
    u_int32_t biSizeImage;
    u_int32_t biXPelsPerMeter;
    u_int32_t biYPelsPerMeter;
    u_int32_t biClrUsed;
    u_int32_t biClrImportant;
} BITMAPINFODEADER;
#endif

}  // namespace OS
