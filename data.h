#ifndef DATA_H
#define DATA_H

#include <time.h>

// ==================== 宏定义 ====================
#define MAX_NAME_LEN 50
#define MAX_PASS_LEN 20
#define MAX_DESC_LEN 200
#define MAX_LOG_LEN 100

// ==================== 枚举定义 ====================

/**
 * @brief 用户权限枚举
 */
typedef enum {
    ROLE_ADMIN = 1,   ///< 管理员（社联）
    ROLE_LEADER,      ///< 社长
    ROLE_MEMBER       ///< 普通成员
} UserRole;

/**
 * @brief 职位枚举
 */
typedef enum {
    POSITION_MEMBER = 1,   ///< 普通成员
    POSITION_STAFF,        ///< 干事
    POSITION_MINISTER      ///< 部长
} PositionType;

/**
 * @brief 财务类型枚举
 */
typedef enum {
    FINANCE_INCOME = 1,    ///< 收入
    FINANCE_EXPENSE        ///< 支出
} FinanceType;

// ==================== 结构体定义 ====================

/**
 * @brief 用户结构体（链表节点）
 */
typedef struct User {
    char username[MAX_NAME_LEN];      ///< 用户名
    char password[MAX_PASS_LEN];      ///< 密码（加密存储）
    UserRole role;                    ///< 用户角色
    char clubName[MAX_NAME_LEN];      ///< 所属社团
    PositionType position;            ///< 职位（成员使用）
    struct User* next;                ///< 下一个节点指针
} User;

/**
 * @brief 社团结构体（链表节点）
 */
typedef struct Club {
    char name[MAX_NAME_LEN];          ///< 社团名称
    char leader[MAX_NAME_LEN];        ///< 社长用户名
    time_t createTime;                ///< 成立时间
    float balance;                    ///< 经费余额
    int memberCount;                  ///< 成员数量
    struct Club* next;                ///< 下一个节点指针
} Club;

/**
 * @brief 财务记录结构体（链表节点）
 */
typedef struct FinanceRecord {
    int id;                           ///< 记录ID
    char clubName[MAX_NAME_LEN];      ///< 社团名称
    FinanceType type;                 ///< 财务类型
    float amount;                     ///< 金额
    char description[MAX_DESC_LEN];   ///< 说明
    time_t time;                      ///< 记录时间
    struct FinanceRecord* next;       ///< 下一个节点指针
} FinanceRecord;

/**
 * @brief 活动结构体（链表节点）
 */
typedef struct Activity {
    int id;                           ///< 活动ID
    char clubName[MAX_NAME_LEN];      ///< 所属社团
    char name[MAX_NAME_LEN];          ///< 活动名称
    char description[MAX_DESC_LEN];   ///< 活动描述
    time_t startTime;                 ///< 开始时间
    time_t endTime;                   ///< 结束时间
    int maxParticipants;              ///< 最大参与人数
    int currentParticipants;          ///< 当前参与人数
    struct Activity* next;            ///< 下一个节点指针
} Activity;

/**
 * @brief 报名记录结构体（链表节点）
 */
typedef struct Enrollment {
    char username[MAX_NAME_LEN];      ///< 报名用户名
    int activityId;                   ///< 活动ID
    time_t enrollTime;                ///< 报名时间
    struct Enrollment* next;          ///< 下一个节点指针
} Enrollment;

/**
 * @brief 日志结构体
 */
typedef struct Log {
    char content[MAX_LOG_LEN];        ///< 日志内容
    time_t time;                      ///< 日志时间
    struct Log* next;                 ///< 下一个节点指针
} Log;

#endif // DATA_H