/*
 * FileName: data/data.h
 * Author: 陈若珂
 * Description: 包含数据表示与访问的相关定义和声明
 * */

#ifndef DATA_H
#define DATA_H

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
