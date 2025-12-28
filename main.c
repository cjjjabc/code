#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auth.h"
#include "utils.h"
#include "club.h"
#include "finance.h"
#include "activity.h"

int main(){
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

  return 0;
}