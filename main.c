#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include <stdarg.h>
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

    print_fmt("系统初始化完成\n");
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

// UTF-8 safe printing on Windows: format into UTF-8 then convert to wide and WriteConsoleW
#ifdef _WIN32
void print_utf8(const char* utf8) {
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    if (h == INVALID_HANDLE_VALUE) {
        printf("%s", utf8);
        return;
    }
    int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    if (wlen <= 0) {
        printf("%s", utf8);
        return;
    }
    wchar_t* wbuf = (wchar_t*)malloc(wlen * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wbuf, wlen);
    DWORD written;
    WriteConsoleW(h, wbuf, wlen - 1, &written, NULL);
    free(wbuf);
}

void print_fmt(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int needed = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);
    if (needed < 0) { return; }
    char* buf = (char*)malloc(needed + 1);
    va_start(ap, fmt);
    vsnprintf(buf, needed + 1, fmt, ap);
    va_end(ap);
    print_utf8(buf);
    free(buf);
}
#else
#define print_fmt(...) printf(__VA_ARGS__)
#endif

/**
 * @brief 主函数
 * @return 程序退出码
 */
int main() {
    // 使程序使用系统区域设置，以便正确输出多字节（中文）
    setlocale(LC_ALL, "");
    // 在 Windows 下切换控制台到 UTF-8 编码，避免中文输出乱码
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    int choice;
    
    // 初始化系统
    initializeSystem();

    // 主菜单循环
    do {
        print_fmt("\n=== 社团管理系统 ===\n");
        print_fmt("1. 注册\n");
        print_fmt("2. 登录\n");
        print_fmt("3. 退出系统\n");
        print_fmt("选择: ");
        
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

