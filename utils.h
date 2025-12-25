#ifndef UTILS_H
#define UTILS_H

#include "data.h"

// ==================== 日志管理 ====================

/**
 * @brief 添加系统日志
 * @param content 日志内容
 */
void addLog(const char* content);

/**
 * @brief 显示系统日志
 */
void displayLogs();

/**
 * @brief 清空日志链表
 */
void clearLogs();

// ==================== 字符串处理 ====================

/**
 * @brief 异或加密/解密
 * @param str 要加密/解密的字符串
 * @param key 密钥
 */
void xorEncrypt(char* str, const char* key);

/**
 * @brief 检查字符串是否只包含字母和数字
 * @param str 要检查的字符串
 * @return 1-是字母数字 0-包含其他字符
 */
int isAlphanumeric(const char* str);

/**
 * @brief 去除字符串末尾的换行符
 * @param str 要处理的字符串
 */
void trimNewline(char* str);

// ==================== 时间处理 ====================

/**
 * @brief 将时间戳格式化为字符串
 * @param time 时间戳
 * @param format 格式字符串
 * @param buf 输出缓冲区
 * @param bufSize 缓冲区大小
 */
void formatTime(time_t time, const char* format, char* buf, size_t bufSize);

/**
 * @brief 解析日期字符串为时间戳
 * @param dateStr 日期字符串（YYYY-MM-DD）
 * @return 时间戳，失败返回-1
 */
time_t parseDate(const char* dateStr);

#endif