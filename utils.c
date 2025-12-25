#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// 全局日志链表
Log* logList = NULL;

/**
 * @brief 添加系统日志
 * @param content 日志内容
 */
void addLog(const char* content) {
    Log* newLog = (Log*)malloc(sizeof(Log));
    if (!newLog) {
        fprintf(stderr, "内存分配失败\n");
        return;
    }
    
    strncpy(newLog->content, content, MAX_LOG_LEN - 1);
    newLog->content[MAX_LOG_LEN - 1] = '\0';
    newLog->time = time(NULL);
    newLog->next = logList;
    logList = newLog;

    // 同时输出到控制台
    printf("[系统日志] %s\n", content);
}

/**
 * @brief 显示系统日志
 */
void displayLogs() {
    Log* logEntry = logList;
    printf("\n=== 系统日志 ===\n");
    
    while (logEntry) {
        char timeStr[20];
        formatTime(logEntry->time, "%Y-%m-%d %H:%M:%S", timeStr, sizeof(timeStr));
        printf("%s: %s\n", timeStr, logEntry->content);
        logEntry = logEntry->next;
    }
}

/**
 * @brief 清空日志链表
 */
void clearLogs() {
    Log* current = logList;
    while (current) {
        Log* next = current->next;
        free(current);
        current = next;
    }
    logList = NULL;
}

/**
 * @brief 异或加密/解密
 * @param str 要加密/解密的字符串
 * @param key 密钥
 */
void xorEncrypt(char* str, const char* key) {
    int keyLen = strlen(key);
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = str[i] ^ key[i % keyLen];
    }
}

/**
 * @brief 检查字符串是否只包含字母和数字
 * @param str 要检查的字符串
 * @return 1-是字母数字 0-包含其他字符
 */
int isAlphanumeric(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isalnum(str[i])) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 去除字符串末尾的换行符
 * @param str 要处理的字符串
 */
void trimNewline(char* str) {
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') {
        str[len - 1] = '\0';
    }
}

/**
 * @brief 将时间戳格式化为字符串
 * @param time 时间戳
 * @param format 格式字符串
 * @param buf 输出缓冲区
 * @param bufSize 缓冲区大小
 */
void formatTime(time_t time, const char* format, char* buf, size_t bufSize) {
    struct tm* timeinfo = localtime(&time);
    strftime(buf, bufSize, format, timeinfo);
}

/**
 * @brief 解析日期字符串为时间戳
 * @param dateStr 日期字符串（YYYY-MM-DD）
 * @return 时间戳，失败返回-1
 */
time_t parseDate(const char* dateStr) {
    struct tm tm_time = {0};
    
    if (sscanf(dateStr, "%d-%d-%d", 
               &tm_time.tm_year, &tm_time.tm_mon, &tm_time.tm_mday) != 3) {
        return -1;
    }
    
    tm_time.tm_year -= 1900;
    tm_time.tm_mon -= 1;
    tm_time.tm_hour = 0;
    tm_time.tm_min = 0;
    tm_time.tm_sec = 0;
    tm_time.tm_isdst = -1;
    
    return mktime(&tm_time);
}