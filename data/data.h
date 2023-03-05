/*
 * FileName: data/data.h
 * Author: 陈若珂
 * Description: 包含数据表示与访问的相关定义和声明
 * */

#ifndef DATA_H
#define DATA_H

/*
 * Name: Staff
 * Description: 表示一个员工
 * */
struct Staff {
    // 员工编号
    int id;

    // 员工是否可用
    int isEnabled;

    // 员工姓名
    const char *name;

    // 员工密码
    const char *password;

    // 联系方式
    const char *contact;
};


/*
 * Name: PermissionEntry
 * Description: 表示员工的权限信息
 * */
struct PermissionEntry {
    // 员工编号
    int staffId;

    // 表示员工是否拥有指定权限
    int *hasPermission;
};

/*
 * Name: JournalEntry
 * Description: 表示一个日志条目
 * */
struct JournalEntry {
    // 员工编号
    int staffId;

    // 时间
    Time time;

    // 执行的操作
    Operation operation;

    // 操作相关参数
    const char **arguments;
};

#endif
