#include "activity.h"
#include "auth.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量在 main.c 中定义，此处仅使用

/**
 * @brief 创建活动
 */
void createActivity() {
    User* user = getCurrentUser();
    if (!user || user->role != ROLE_LEADER) {
        printf("只有社长可以创建活动！\n");
        return;
    }

    Activity* newActivity = (Activity*)malloc(sizeof(Activity));
    if (!newActivity) {
        printf("内存分配失败！\n");
        return;
    }

    printf("请输入活动名称: ");
    scanf("%s", newActivity->name);

    strcpy(newActivity->clubName, user->clubName);

    printf("请输入活动描述: ");
    getchar();
    fgets(newActivity->description, MAX_DESC_LEN, stdin);
    trimNewline(newActivity->description);

    printf("请输入最大参与人数: ");
    if (scanf("%d", &newActivity->maxParticipants) != 1 || newActivity->maxParticipants <= 0) {
        printf("无效的参与人数！\n");
        free(newActivity);
        return;
    }

    newActivity->currentParticipants = 0;
    newActivity->id = activityIdCounter++;
    newActivity->startTime = time(NULL);
    newActivity->endTime = newActivity->startTime + 3600 * 24; // 默认持续24小时

    // 添加到链表
    newActivity->next = activityList;
    activityList = newActivity;

    addLog("新活动创建成功");
    saveActivitiesToFile();
    printf("活动创建成功！\n");
}

/**
 * @brief 显示所有活动
 */
void displayAllActivities() {
    Activity* activity = activityList;
    printf("\n=== 所有活动列表 ===\n");

    if (!activity) {
        printf("暂无活动\n");
        return;
    }

    while (activity) {
        char timeStr[20];
        formatTime(activity->startTime, "%Y-%m-%d %H:%M", timeStr, sizeof(timeStr));
        
        printf("活动ID: %d\n", activity->id);
        printf("  社团: %s\n", activity->clubName);
        printf("  活动名称: %s\n", activity->name);
        printf("  活动时间: %s\n", timeStr);
        printf("  当前人数: %d/%d\n", 
               activity->currentParticipants,
               activity->maxParticipants);
        printf("  描述: %s\n", activity->description);
        printf("------------------------\n");
        activity = activity->next;
    }
}

/**
 * @brief 显示指定社团的活动
 * @param clubName 社团名称
 */
void displayClubActivities(const char* clubName) {
    Activity* activity = activityList;
    printf("\n=== %s 社团活动列表 ===\n", clubName);

    int count = 0;
    while (activity) {
        if (strcmp(activity->clubName, clubName) == 0) {
            char timeStr[20];
            formatTime(activity->startTime, "%Y-%m-%d", timeStr, sizeof(timeStr));
            
            printf("ID: %d, 活动: %s, 参与人数: %d/%d\n",
                   activity->id, activity->name,
                   activity->currentParticipants,
                   activity->maxParticipants);
            count++;
        }
        activity = activity->next;
    }

    if (count == 0) {
        printf("该社团暂无活动\n");
    }
}

/**
 * @brief 报名活动
 */
void enrollActivity() {
    User* user = getCurrentUser();
    if (!user) {
        printf("请先登录！\n");
        return;
    }

    int activityId;
    printf("请输入活动ID: ");
    if (scanf("%d", &activityId) != 1) {
        printf("无效的活动ID！\n");
        return;
    }

    // 查找活动
    Activity* activity = activityList;
    while (activity) {
        if (activity->id == activityId) {
            break;
        }
        activity = activity->next;
    }

    if (!activity) {
        printf("活动不存在！\n");
        return;
    }

    // 检查是否已满
    if (activity->currentParticipants >= activity->maxParticipants) {
        printf("活动已满！\n");
        return;
    }

    // 检查是否已报名
    Enrollment* enroll = enrollmentList;
    while (enroll) {
        if (strcmp(enroll->username, user->username) == 0 && 
            enroll->activityId == activityId) {
            printf("您已报名此活动！\n");
            return;
        }
        enroll = enroll->next;
    }

    // 添加报名记录
    Enrollment* newEnroll = (Enrollment*)malloc(sizeof(Enrollment));
    if (!newEnroll) {
        printf("内存分配失败！\n");
        return;
    }
    
    strcpy(newEnroll->username, user->username);
    newEnroll->activityId = activityId;
    newEnroll->enrollTime = time(NULL);
    newEnroll->next = enrollmentList;
    enrollmentList = newEnroll;

    // 更新活动参与人数
    activity->currentParticipants++;

    addLog("活动报名成功");
    saveActivitiesToFile();
    saveEnrollmentsToFile();
    printf("报名成功！\n");
}

/**
 * @brief 显示用户报名记录
 * @param username 用户名
 */
void displayUserEnrollments(const char* username) {
    Enrollment* enroll = enrollmentList;
    printf("\n=== 我的报名记录 ===\n");

    int count = 0;
    while (enroll) {
        if (strcmp(enroll->username, username) == 0) {
            char timeStr[20];
            formatTime(enroll->enrollTime, "%Y-%m-%d %H:%M", timeStr, sizeof(timeStr));
            
            printf("活动ID: %d, 报名时间: %s\n",
                   enroll->activityId, timeStr);
            count++;
        }
        enroll = enroll->next;
    }

    if (count == 0) {
        printf("暂无报名记录\n");
    }
}

/**
 * @brief 保存活动数据到文件
 */
void saveActivitiesToFile() {
    FILE* file = fopen("activities.dat", "wb");
    if (!file) {
        printf("无法打开活动数据文件！\n");
        return;
    }

    // 保存ID计数器
    if (fwrite(&activityIdCounter, sizeof(int), 1, file) != 1) {
        printf("写入ID计数器失败！\n");
        fclose(file);
        return;
    }

    // 保存活动
    Activity* current = activityList;
    while (current) {
        if (fwrite(current, sizeof(Activity), 1, file) != 1) {
            printf("写入活动数据失败！\n");
            break;
        }
        current = current->next;
    }

    fclose(file);
}

/**
 * @brief 加载活动数据
 */
void loadActivitiesFromFile() {
    FILE* file = fopen("activities.dat", "rb");
    if (!file) {
        return;
    }

    // 加载ID计数器
    if (fread(&activityIdCounter, sizeof(int), 1, file) != 1) {
        fclose(file);
        return;
    }

    Activity temp;
    while (fread(&temp, sizeof(Activity), 1, file)) {
        Activity* newActivity = (Activity*)malloc(sizeof(Activity));
        if (!newActivity) {
            printf("内存分配失败！\n");
            break;
        }
        memcpy(newActivity, &temp, sizeof(Activity));
        newActivity->next = activityList;
        activityList = newActivity;
    }

    fclose(file);
}

/**
 * @brief 保存报名数据到文件
 */
void saveEnrollmentsToFile() {
    FILE* file = fopen("enrollments.dat", "wb");
    if (!file) {
        printf("无法打开报名数据文件！\n");
        return;
    }

    Enrollment* current = enrollmentList;
    while (current) {
        if (fwrite(current, sizeof(Enrollment), 1, file) != 1) {
            printf("写入报名数据失败！\n");
            break;
        }
        current = current->next;
    }

    fclose(file);
}

/**
 * @brief 加载报名数据
 */
void loadEnrollmentsFromFile() {
    FILE* file = fopen("enrollments.dat", "rb");
    if (!file) {
        return;
    }

    Enrollment temp;
    while (fread(&temp, sizeof(Enrollment), 1, file)) {
        Enrollment* newEnroll = (Enrollment*)malloc(sizeof(Enrollment));
        if (!newEnroll) {
            printf("内存分配失败！\n");
            break;
        }
        memcpy(newEnroll, &temp, sizeof(Enrollment));
        newEnroll->next = enrollmentList;
        enrollmentList = newEnroll;
    }

    fclose(file);
}

/**
 * @brief 释放活动链表内存
 */
void freeActivityList() {
    Activity* current = activityList;
    while (current) {
        Activity* next = current->next;
        free(current);
        current = next;
    }
    activityList = NULL;
}

/**
 * @brief 释放报名链表内存
 */
void freeEnrollmentList() {
    Enrollment* current = enrollmentList;
    while (current) {
        Enrollment* next = current->next;
        free(current);
        current = next;
    }
    enrollmentList = NULL;
}