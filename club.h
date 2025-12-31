#ifndef CLUB_H
#define CLUB_H

#include "data.h"

// 全局变量（在 main.c 中定义）
extern Club* clubList;

// ==================== 社团管理 ====================

/**
 * @brief 创建社团（管理员功能）
 */
void createClub();

/**
 * @brief 显示所有社团
 */
void displayAllClubs();

/**
 * @brief 根据名称查找社团
 * @param name 社团名称
 * @return 社团指针，未找到返回NULL
 */
Club* findClubByName(const char* name);

/**
 * @brief 成员加入社团
 */
void joinClub();

/**
 * @brief 保存社团数据到文件
 */
void saveClubsToFile();

/**
 * @brief 加载社团数据
 */
void loadClubsFromFile();

/**
 * @brief 释放社团链表内存
 */
void freeClubList();

#endif // CLUB_