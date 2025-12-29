#ifndef FINANCE_H
#define FINANCE_H

#include "data.h"

// ==================== 财务管理 ====================

/**
 * @brief 添加财务记录
 */
void addFinanceRecord();

/**
 * @brief 导出财务报表
 */
void exportFinanceReport();

/**
 * @brief 保存财务数据到文件
 */
void saveFinanceToFile();

/**
 * @brief 加载财务数据
 */
void loadFinanceFromFile();

/**
 * @brief 释放财务记录链表内存
 */
void freeFinanceList();

#endif // FINANCE_H