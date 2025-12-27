#ifndef AUTH_H
#define AUTH_H

#include "data.h"

// ==================== 用户管理 ====================

/**
 * @brief 用户注册
 */
void registerUser();

/**
 * @brief 用户登录
 */
void login();

/**
 * @brief 保存用户数据到文件
 */
void saveUsersToFile();

/**
 * @brief 加载用户数据
 */
void loadUsersFromFile();

/**
 * @brief 获取当前登录用户
 * @return 当前用户指针
 */
User* getCurrentUser();

/**
 * @brief 设置当前用户
 * @param user 用户指针
 */
void setCurrentUser(User* user);

/**
 * @brief 退出登录
 */
void logout();

/**
 * @brief 根据用户名查找用户
 * @param username 用户名
 * @return 用户指针，未找到返回NULL
 */
User* findUserByUsername(const char* username);

/**
 * @brief 释放用户链表内存
 */
void freeUserList();

#endif // AUTH_H