#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 全局变量在 main.c 中定义，此处仅使用

/**
 * @brief 用户注册
 */
void registerUser() {
    User* newUser = (User*)malloc(sizeof(User));
    if (!newUser) {
        printf("内存分配失败！\n");
        return;
    }

    printf("请输入用户名: ");
    scanf("%s", newUser->username);

    // 检查用户名是否已存在
    User* existingUser = findUserByUsername(newUser->username);
    if (existingUser) {
        printf("用户名已存在！\n");
        free(newUser);
        return;
    }

    printf("请输入密码: ");
    scanf("%s", newUser->password);

    // 加密密码
    xorEncrypt(newUser->password, "secret_key");

    printf("选择角色 (1-管理员 2-社长 3-普通成员): ");
    int role;
    if (scanf("%d", &role) != 1 || role < 1 || role > 3) {
        printf("无效的角色选择！\n");
        free(newUser);
        return;
    }
    newUser->role = (UserRole)role;

    // 初始化其他字段
    memset(newUser->clubName, 0, MAX_NAME_LEN);
    newUser->position = POSITION_MEMBER;

    // 添加到链表头部
    newUser->next = userList;
    userList = newUser;

    addLog("新用户注册成功");
    saveUsersToFile();
    printf("注册成功！\n");
}

/**
 * @brief 用户登录
 */
void login() {
    char username[MAX_NAME_LEN];
    char password[MAX_PASS_LEN];

    printf("用户名: ");
    scanf("%s", username);
    printf("密码: ");
    scanf("%s", password);

    User* user = findUserByUsername(username);
    if (!user) {
        printf("用户不存在！\n");
        return;
    }

    // 验证密码
    char tempPass[MAX_PASS_LEN];
    strcpy(tempPass, user->password);
    xorEncrypt(tempPass, "secret_key"); // 解密

    if (strcmp(tempPass, password) != 0) {
        printf("密码错误！\n");
        return;
    }

    currentUser = user;
    printf("登录成功！欢迎 %s\n", username);
    addLog("用户登录成功");
}

/**
 * @brief 保存用户数据到文件
 */
void saveUsersToFile() {
    FILE* file = fopen("users.dat", "wb");
    if (!file) {
        printf("无法打开用户数据文件！\n");
        return;
    }

    User* current = userList;
    while (current) {
        if (fwrite(current, sizeof(User), 1, file) != 1) {
            printf("写入用户数据失败！\n");
            break;
        }
        current = current->next;
    }

    fclose(file);
}

/**
 * @brief 加载用户数据
 */
void loadUsersFromFile() {
    FILE* file = fopen("users.dat", "rb");
    if (!file) {
        // 文件不存在是正常情况
        return;
    }

    User temp;
    while (fread(&temp, sizeof(User), 1, file)) {
        User* newUser = (User*)malloc(sizeof(User));
        if (!newUser) {
            printf("内存分配失败！\n");
            break;
        }
        // 逐个字段复制，避免复制无效的指针
        strcpy(newUser->username, temp.username);
        strcpy(newUser->password, temp.password);
        newUser->role = temp.role;
        strcpy(newUser->clubName, temp.clubName);
        newUser->position = temp.position;
        newUser->next = userList;
        userList = newUser;
    }

    fclose(file);
}

/**
 * @brief 获取当前登录用户
 * @return 当前用户指针
 */
User* getCurrentUser() {
    return currentUser;
}

/**
 * @brief 设置当前用户
 * @param user 用户指针
 */
void setCurrentUser(User* user) {
    currentUser = user;
}

/**
 * @brief 退出登录
 */
void logout() {
    if (currentUser) {
        printf("用户 %s 已退出登录\n", currentUser->username);
    }
    currentUser = NULL;
}

/**
 * @brief 根据用户名查找用户
 * @param username 用户名
 * @return 用户指针，未找到返回NULL
 */
User* findUserByUsername(const char* username) {
    User* current = userList;
    while (current) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief 释放用户链表内存
 */
void freeUserList() {
    User* current = userList;
    while (current) {
        User* next = current->next;
        free(current);
        current = next;
    }
    userList = NULL;
}