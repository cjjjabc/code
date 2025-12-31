#include "club.h"
#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量在 main.c 中定义，此处仅使用

/**
 * @brief 创建社团（管理员功能）
 */
void createClub() {
    if (!getCurrentUser() || getCurrentUser()->role != ROLE_ADMIN) {
        printf("权限不足！只有管理员可以创建社团\n");
        return;
    }

    Club* newClub = (Club*)malloc(sizeof(Club));
    if (!newClub) {
        printf("内存分配失败！\n");
        return;
    }

    printf("请输入社团名称: ");
    scanf("%s", newClub->name);

    printf("请输入社长用户名: ");
    scanf("%s", newClub->leader);

    // 验证社长用户是否存在
    User* leader = findUserByUsername(newClub->leader);
    if (!leader) {
        printf("社长用户不存在！\n");
        free(newClub);
        return;
    }

    // 更新社长信息
    leader->role = ROLE_LEADER;
    strcpy(leader->clubName, newClub->name);

    newClub->createTime = time(NULL);
    newClub->balance = 0.0f;
    newClub->memberCount = 1; // 社长自己

    // 添加到链表
    newClub->next = clubList;
    clubList = newClub;

    addLog("新社团创建成功");
    saveClubsToFile();
    printf("社团创建成功！\n");
}

/**
 * @brief 显示所有社团
 */
void displayAllClubs() {
    Club* current = clubList;
    printf("\n=== 所有社团列表 ===\n");

    if (!current) {
        printf("暂无社团\n");
        return;
    }

    while (current) {
        char timeStr[20];
        formatTime(current->createTime, "%Y-%m-%d", timeStr, sizeof(timeStr));
        
        printf("社团: %s\n", current->name);
        printf("  社长: %s\n", current->leader);
        printf("  成立时间: %s\n", timeStr);
        printf("  成员数: %d\n", current->memberCount);
        printf("  经费余额: %.2f\n", current->balance);
        printf("------------------------\n");
        current = current->next;
    }
}

/**
 * @brief 根据名称查找社团
 * @param name 社团名称
 * @return 社团指针，未找到返回NULL
 */
Club* findClubByName(const char* name) {
    Club* club = clubList;
    while (club) {
        if (strcmp(club->name, name) == 0) {
            return club;
        }
        club = club->next;
    }
    return NULL;
}

/**
 * @brief 成员加入社团
 */
void joinClub() {
    User* user = getCurrentUser();
    if (!user || user->role != ROLE_MEMBER) {
        printf("只有普通成员可以加入社团！\n");
        return;
    }

    char clubName[MAX_NAME_LEN];
    printf("请输入要加入的社团名称: ");
    scanf("%s", clubName);

    // 查找社团
    Club* club = findClubByName(clubName);
    if (!club) {
        printf("社团不存在！\n");
        return;
    }

    // 检查是否已经加入社团
    if (strlen(user->clubName) > 0) {
        printf("您已经加入了社团 %s，不能重复加入！\n", user->clubName);
        return;
    }

    // 更新用户信息
    strcpy(user->clubName, clubName);
    user->position = POSITION_MEMBER;

    // 更新社团成员数
    club->memberCount++;

    addLog("成员加入社团成功");
    saveClubsToFile();
    printf("成功加入社团 %s！\n", clubName);
}

/**
 * @brief 保存社团数据到文件
 */
void saveClubsToFile() {
    FILE* file = fopen("clubs.dat", "wb");
    if (!file) {
        printf("无法打开社团数据文件！\n");
        return;
    }

    Club* current = clubList;
    while (current) {
        if (fwrite(current, sizeof(Club), 1, file) != 1) {
            printf("写入社团数据失败！\n");
            break;
        }
        current = current->next;
    }

    fclose(file);
}

/**
 * @brief 加载社团数据
 */
void loadClubsFromFile() {
    FILE* file = fopen("clubs.dat", "rb");
    if (!file) {
        return;
    }

    Club temp;
    while (fread(&temp, sizeof(Club), 1, file)) {
        Club* newClub = (Club*)malloc(sizeof(Club));
        if (!newClub) {
            printf("内存分配失败！\n");
            break;
        }
        memcpy(newClub, &temp, sizeof(Club));
        newClub->next = clubList;
        clubList = newClub;
    }

    fclose(file);
}

/**
 * @brief 释放社团链表内存
 */
void freeClubList() {
    Club* current = clubList;
    while (current) {
        Club* next = current->next;
        free(current);
        current = next;
    }
    clubList = NULL;
}