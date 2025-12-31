#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "utils.h"
#include "club.h"
#include "finance.h"
#include "activity.h"

    User* userList = NULL;
Club* clubList = NULL;
FinanceRecord* financeList = NULL;
Activity* activityList = NULL;
Enrollment* enrollmentList = NULL;
Log* logList = NULL;
User* currentUser = NULL;
int financeIdCounter = 1000;
int activityIdCounter = 2000;
       void adminMenu() {
    int choice;
    do {
        printf("\n=== 管理员菜单 ===\n");
        printf("1. 创建社团\n");
        printf("2. 查看所有社团\n");
        printf("3. 导出财务报表\n");
        printf("4. 查看系统日志\n");
        printf("0. 退出登录\n");
        printf("选择: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createClub(); break;
            case 2: displayAllClubs(); break;
            case 3: exportFinanceReport(); break;
            case 4: displayLogs(); break;
            case 0: 
                logout();
                printf("已退出登录\n");
                break;
            default:
                printf("无效的选择！\n");
        }
    } while (choice != 0 && getCurrentUser() != NULL);
}


void leaderMenu() {
    int choice;
    do {
        printf("\n=== 社长菜单 ===\n");
        printf("1. 创建活动\n");
        printf("2. 查看本社团活动\n");
        printf("3. 添加财务记录\n");
        printf("4. 查看社团余额\n");
        printf("0. 退出登录\n");
        printf("选择: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createActivity(); break;
            case 2: 
                if (getCurrentUser()) {
                    displayClubActivities(getCurrentUser()->clubName);
                }
                break;
            case 3: addFinanceRecord(); break;
            case 4: {
                if (getCurrentUser()) {
                    Club* club = findClubByName(getCurrentUser()->clubName);
                    if (club) {
                        printf("社团 %s 余额: %.2f\n", club->name, club->balance);
                    }
                }
                break;
            }
            case 0: 
                logout();
                printf("已退出登录\n");
                break;
            default:
                printf("无效的选择！\n");
        }
    } while (choice != 0 && getCurrentUser() != NULL);
}

/**
 * @brief 成员菜单
 */
void memberMenu() {
    int choice;
    do {
        printf("\n=== 成员菜单 ===\n");
        printf("1. 加入社团\n");
        printf("2. 查看可报名活动\n");
        printf("3. 报名活动\n");
        printf("4. 查看我的报名记录\n");
        printf("0. 退出登录\n");
        printf("选择: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: joinClub(); break;
            case 2: displayAllActivities(); break;
            case 3: enrollActivity(); break;
            case 4: 
                if (getCurrentUser()) {
                    displayUserEnrollments(getCurrentUser()->username);
                }
                break;
            case 0: 
                logout();
                printf("已退出登录\n");
                break;
            default:
                printf("无效的选择！\n");
        }
    } while (choice != 0 && getCurrentUser() != NULL);
}

/**
 * @brief 初始化系统
 */
void initializeSystem() {
    // 加载所有数据
    loadUsersFromFile();
    loadClubsFromFile();
    loadFinanceFromFile();
    loadActivitiesFromFile();
    loadEnrollmentsFromFile();

    // 创建默认管理员账户（如果没有）
    if (findUserByUsername("admin") == NULL) {
        User* admin = (User*)malloc(sizeof(User));
        if (admin) {
            strcpy(admin->username, "admin");
            strcpy(admin->password, "admin123");
            xorEncrypt(admin->password, "secret_key");
            admin->role = ROLE_ADMIN;
            memset(admin->clubName, 0, MAX_NAME_LEN);
            admin->position = POSITION_MEMBER;
            admin->next = userList;
            userList = admin;
            
            addLog("系统初始化完成，创建默认管理员账户");
            saveUsersToFile();
        }
    }

    printf("系统初始化完成\n");
}

/**
 * @brief 清理系统资源
 */
void cleanupSystem() {
    // 保存所有数据
    saveUsersToFile();
    saveClubsToFile();
    saveFinanceToFile();
    saveActivitiesToFile();
    saveEnrollmentsToFile();

    // 释放所有内存
    freeUserList();
    freeClubList();
    freeFinanceList();
    freeActivityList();
    freeEnrollmentList();
    clearLogs();
}

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main() {
    int choice;
    
    // 初始化系统
    initializeSystem();

    // 主菜单循环
    do {
        printf("\n=== 社团管理系统 ===\n");
        printf("1. 注册\n");
        printf("2. 登录\n");
        printf("3. 退出系统\n");
        printf("选择: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("无效的输入！\n");
            while (getchar() != '\n'); // 清空输入缓冲区
            continue;
        }

        switch (choice) {
            case 1:
                registerUser();
                break;
            case 2:
                login();
                if (getCurrentUser() != NULL) {
                    switch (getCurrentUser()->role) {
                        case ROLE_ADMIN: 
                            adminMenu(); 
                            break;
                        case ROLE_LEADER: 
                            leaderMenu(); 
                            break;
                        case ROLE_MEMBER: 
                            memberMenu(); 
                            break;
                    }
                }
                break;
            case 3:
                cleanupSystem();
                printf("系统已退出，数据已保存\n");
                break;
            default:
                printf("无效的选择！\n");
        }
    } while (choice != 3);

    return 0;
}

