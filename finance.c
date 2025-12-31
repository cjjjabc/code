
#include "finance.h"
#include "club.h"
#include "utils.h"
#include "auth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 全局变量在 main.c 中定义，此处仅使用

/**
 * @brief 添加财务记录
 */
void addFinanceRecord() {
    User* user = getCurrentUser();
    if (!user || (user->role != ROLE_ADMIN && user->role != ROLE_LEADER)) {
        printf("权限不足！只有管理员和社长可以添加财务记录\n");
        return;
    }

    FinanceRecord* newRecord = (FinanceRecord*)malloc(sizeof(FinanceRecord));
    if (!newRecord) {
        printf("内存分配失败！\n");
        return;
    }

    // 如果是社长，只能添加自己社团的记录
    if (user->role == ROLE_LEADER) {
        strcpy(newRecord->clubName, user->clubName);
    } else {
        printf("请输入社团名称: ");
        scanf("%s", newRecord->clubName);
    }

    printf("选择类型 (1-收入 2-支出): ");
    int type;
    if (scanf("%d", &type) != 1 || (type != 1 && type != 2)) {
        printf("无效的类型选择！\n");
        free(newRecord);
        return;
    }
    newRecord->type = (FinanceType)type;

    printf("请输入金额: ");
    if (scanf("%f", &newRecord->amount) != 1 || newRecord->amount <= 0) {
        printf("无效的金额！\n");
        free(newRecord);
        return;
    }

    printf("请输入说明: ");
    getchar(); // 清除缓冲区
    fgets(newRecord->description, MAX_DESC_LEN, stdin);
    trimNewline(newRecord->description);

    // 更新社团余额
    Club* club = findClubByName(newRecord->clubName);
    if (!club) {
        printf("社团不存在！\n");
        free(newRecord);
        return;
    }

    if (newRecord->type == FINANCE_INCOME) {
        club->balance += newRecord->amount;
    } else {
        if (club->balance < newRecord->amount) {
            printf("余额不足！当前余额: %.2f\n", club->balance);
            free(newRecord);
            return;
        }
        club->balance -= newRecord->amount;
    }

    newRecord->id = financeIdCounter++;
    newRecord->time = time(NULL);

    // 添加到链表
    newRecord->next = financeList;
    financeList = newRecord;

    addLog("财务记录添加成功");
    saveFinanceToFile();
    printf("财务记录添加成功！\n");
}

/**
 * @brief 查看财务记录
 */
void displayFinanceRecords() {
    User* user = getCurrentUser();
    if (!user) {
        printf("请先登录！\n");
        return;
    }

    printf("\n=== 财务记录 ===\n");
    
    // 社长只能查看自己社团的记录
    if (user->role == ROLE_LEADER) {
        printf("社团: %s\n", user->clubName);
        int count = 0;
        FinanceRecord* current = financeList;
        while (current) {
            if (strcmp(current->clubName, user->clubName) == 0) {
                count++;
                char timeStr[20];
                struct tm* timeinfo = localtime(&current->time);
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
                printf("[ID:%d] %s | %s | %.2f元 | %s\n",
                       current->id,
                       timeStr,
                       current->type == FINANCE_INCOME ? "收入" : "支出",
                       current->amount,
                       current->description);
            }
            current = current->next;
        }
        if (count == 0) {
            printf("暂无财务记录\n");
        }
    }
    // 管理员可以查看所有社团的记录
    else if (user->role == ROLE_ADMIN) {
        char clubName[MAX_NAME_LEN];
        printf("请输入社团名称 (留空查看所有): ");
        getchar(); // 清除缓冲区
        fgets(clubName, MAX_NAME_LEN, stdin);
        trimNewline(clubName);
        
        int count = 0;
        FinanceRecord* current = financeList;
        
        if (strlen(clubName) == 0) {
            printf("\n所有财务记录：\n");
            while (current) {
                count++;
                char timeStr[20];
                struct tm* timeinfo = localtime(&current->time);
                strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
                printf("[ID:%d] %s | %s | %.2f元 | %s | %s\n",
                       current->id,
                       timeStr,
                       current->type == FINANCE_INCOME ? "收入" : "支出",
                       current->amount,
                       current->clubName,
                       current->description);
                current = current->next;
            }
        } else {
            printf("\n社团 %s 的财务记录：\n", clubName);
            while (current) {
                if (strcmp(current->clubName, clubName) == 0) {
                    count++;
                    char timeStr[20];
                    struct tm* timeinfo = localtime(&current->time);
                    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);
                    printf("[ID:%d] %s | %s | %.2f元 | %s\n",
                           current->id,
                           timeStr,
                           current->type == FINANCE_INCOME ? "收入" : "支出",
                           current->amount,
                           current->description);
                }
                current = current->next;
            }
        }
        
        if (count == 0) {
            printf("暂无财务记录\n");
        }
    } else {
        printf("权限不足！\n");
    }
}

/**
 * @brief 导出财务报表
 */
void exportFinanceReport() {
    char clubName[MAX_NAME_LEN];
    char startDate[11], endDate[11];

    printf("请输入社团名称: ");
    scanf("%s", clubName);

    printf("请输入开始时间 (YYYY-MM-DD): ");
    scanf("%s", startDate);

    printf("请输入结束时间 (YYYY-MM-DD): ");
    scanf("%s", endDate);

    time_t startTime = parseDate(startDate);
    time_t endTime = parseDate(endDate);

    if (startTime == -1 || endTime == -1 || startTime > endTime) {
        printf("无效的日期！\n");
        return;
    }

    // 导出到文件
    FILE* file = fopen("finance_report.txt", "w");
    if (!file) {
        printf("无法创建报表文件！\n");
        return;
    }

    fprintf(file, "=== 财务报表 ===\n");
    fprintf(file, "社团: %s\n", clubName);
    
    char startStr[20], endStr[20];
    formatTime(startTime, "%Y-%m-%d", startStr, sizeof(startStr));
    formatTime(endTime, "%Y-%m-%d", endStr, sizeof(endStr));
    fprintf(file, "时间段: %s 到 %s\n", startStr, endStr);
    fprintf(file, "------------------------\n");

    FinanceRecord* current = financeList;
    float totalIncome = 0, totalExpense = 0;
    int count = 0;

    while (current) {
        if (strcmp(current->clubName, clubName) == 0 &&
            current->time >= startTime &&
            current->time <= endTime) {

            char timeStr[20];
            formatTime(current->time, "%Y-%m-%d", timeStr, sizeof(timeStr));
            
            fprintf(file, "记录ID: %d\n", current->id);
            fprintf(file, "时间: %s\n", timeStr);
            fprintf(file, "类型: %s\n",
                   current->type == FINANCE_INCOME ? "收入" : "支出");
            fprintf(file, "金额: %.2f\n", current->amount);
            fprintf(file, "说明: %s\n", current->description);
            fprintf(file, "------------------------\n");

            if (current->type == FINANCE_INCOME) {
                totalIncome += current->amount;
            } else {
                totalExpense += current->amount;
            }
            count++;
        }
        current = current->next;
    }

    if (count == 0) {
        fprintf(file, "该时间段内无财务记录\n");
    } else {
        fprintf(file, "统计信息:\n");
        fprintf(file, "  总收入: %.2f\n", totalIncome);
        fprintf(file, "  总支出: %.2f\n", totalExpense);
        fprintf(file, "  净收入: %.2f\n", totalIncome - totalExpense);
        fprintf(file, "  记录总数: %d\n", count);
    }

    fclose(file);
    addLog("财务报表导出成功");
    printf("财务报表已导出到 finance_report.txt\n");
}

/**
 * @brief 保存财务数据到文件
 */
void saveFinanceToFile() {
    FILE* file = fopen("finance.dat", "wb");
    if (!file) {
        printf("无法打开财务数据文件！\n");
        return;
    }

    // 保存ID计数器
    if (fwrite(&financeIdCounter, sizeof(int), 1, file) != 1) {
        printf("写入ID计数器失败！\n");
        fclose(file);
        return;
    }

    // 保存记录
    FinanceRecord* current = financeList;
    while (current) {
        if (fwrite(current, sizeof(FinanceRecord), 1, file) != 1) {
            printf("写入财务记录失败！\n");
            break;
        }
        current = current->next;
    }

    fclose(file);
}

/**
 * @brief 加载财务数据
 */
void loadFinanceFromFile() {
    FILE* file = fopen("finance.dat", "rb");
    if (!file) {
        return;
    }

    // 加载ID计数器
    if (fread(&financeIdCounter, sizeof(int), 1, file) != 1) {
        fclose(file);
        return;
    }

    FinanceRecord temp;
    while (fread(&temp, sizeof(FinanceRecord), 1, file)) {
        FinanceRecord* newRecord = (FinanceRecord*)malloc(sizeof(FinanceRecord));
        if (!newRecord) {
            printf("内存分配失败！\n");
            break;
        }
        // 逐个字段复制，避免复制无效的指针
        newRecord->id = temp.id;
        strcpy(newRecord->clubName, temp.clubName);
        newRecord->type = temp.type;
        newRecord->amount = temp.amount;
        strcpy(newRecord->description, temp.description);
        newRecord->time = temp.time;
        newRecord->next = financeList;
        financeList = newRecord;
    }

    fclose(file);
}

/**
 * @brief 释放财务记录链表内存
 */
void freeFinanceList() {
    FinanceRecord* current = financeList;
    while (current) {
        FinanceRecord* next = current->next;
        free(current);
        current = next;
    }
    financeList = NULL;
}