#ifndef ACTIVITY_H
#define ACTIVITY_H

#include "data.h"

// ==================== 活动管理 ====================

/**
 * @brief 创建活动
 */
void createActivity();

/**
 * @brief 显示所有活动
 */
void displayAllActivities();

/**
 * @brief 显示指定社团的活动
 * @param clubName 社团名称
 */
void displayClubActivities(const char* clubName);

/**
 * @brief 报名活动
 */
void enrollActivity();

/**
 * @brief 显示用户报名记录
 * @param username 用户名
 */
void displayUserEnrollments(const char* username);

/**
 * @brief 保存活动数据到文件
 */
void saveActivitiesToFile();

/**
 * @brief 加载活动数据
 */
void loadActivitiesFromFile();

/**
 * @brief 保存报名数据到文件
 */
void saveEnrollmentsToFile();

/**
 * @brief 加载报名数据
 */
void loadEnrollmentsFromFile();

/**
 * @brief 释放活动链表内存
 */
void freeActivityList();

/**
 * @brief 释放报名链表内存
 */
void freeEnrollmentList();

#endif // ACTIVITY_H