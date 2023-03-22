#include "journalService.h"
#include "../utils.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

/*读取=之前的字符*/
char *getStringBefore(char *ch, char *string)
{
    int i = 0;
    while (string[i] != '\0')
    {
        if (string[i] != '=')
            ch[i] = string[i];
        else
            break;
        i++;
    }
    ch[i] = '\0';
    return ch;
}

/*写一条进货日志*/
Table *AddJournal(Table *table, int staffId, Operation operation)
{
    char **arguments = NULL;
    int argumentCount = 0;
    Time time = GetSystemTime(); // 获取系统时间
    if (table != NULL)
    {
        TableRow *row = GetRowByIndex(table, 1); // 获取信息行

        TableRow *titleTableRow = GetTableTitle(table); // 获取标题行
        argumentCount = titleTableRow->columnCount; // 获取表格中有几个标题（arguments字符串的个数）
        arguments = (char **)malloc(argumentCount * sizeof(char *)); // 给arguments地方来存储信息  //陈若珂写的
        int i = 0;                                                   // 用来设置 argument[i]

        LinkedList *title = titleTableRow->items;
        LinkedList *data = row->items;
        while (title != NULL)
        { // 遍历获取标题
            int len1 = strlen(title->data);
            int len2 = strlen(data->data);
            char *result = malloc((len1 + len2 + 4) * sizeof(char));

            strcpy(result, title->data);
            strcat(result, " = ");
            strcat(result, data->data);
            arguments[i] = result;

            i++;
            title = title->next;
            data = data->next;
        }
    }

    JournalEntry *newJournal =
        NewJournalEntry(staffId, &time, operation, (const char **)arguments, argumentCount); // 开创新日志条目
                                                                                             //
    for (int i = 0; i < argumentCount; i++)
    {
        free(arguments[i]);
    }
    if (arguments != NULL) {
        free(arguments);
    }

    int judge = AppendJournalEntry(newJournal); // 判断是否添加成功
    if (judge == 0)
        JournalSave(); // 若输入正确，返回该表格
    else
    {                                                                      // 否，返回空表格
        TableRow *blank_row = NewTableRow();                               // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "添加日志条目时出错 !"); // 指明错误信息
        return remark_table;                                               // 返回空表格
    }

    return table;
}

/*读取某员工的所有某操作日志*/
Table *GetAllJournalOfOneStaffOfOneOperation(Table *table)
{
    if (table == NULL)
    {
        TableRow *blank_row = NewTableRow();                             // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有提供任何信息 !"); // 指明错误信息
        return remark_table;                                             // 返回空表格
    }

    TableRow *row = GetRowByIndex(table, 1);
    const char *idString = GetRowItemByColumnName(table, row, "id");
    int staffid = atoi(idString); // 获取员工工号
    const char *operationString = GetRowItemByColumnName(table, row, "operation");
    Operation operation = StringToOperation(operationString); // 获取操作

    LinkedList *staffAllJournal = GetJournalsByStaffId(staffid); // 根据staffid获取该员工的全部日志
    if (staffAllJournal == NULL)
    {                                                                        // 若该员工没有操作日志
        TableRow *blank_row = NewTableRow();                                 // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有该员工的操作日志 !"); // 指明错误信息
        return remark_table;                                                 // 返回空表格
    }

    /*开创新表格，放入标题title，给予备注*/
    TableRow *title = NewTableRow();
    AppendTableRow(title, "time");
    Table *oneStaffAllStockJournal = NewTable(title, "某员工的所有操作日志");

    LinkedList *p = staffAllJournal;
    const char *givenOperation = OperationToString(operation);
    while (p != NULL)
    {                                                                       // 遍历员工日志
        JournalEntry *journal = (JournalEntry *)p->data;                    // 获取journal
        Operation staffThisOperation = GetJournalEntryOperation(journal);   // 获取该条日志的operation
        const char *staffOperation = OperationToString(staffThisOperation); // 转化operation为string
        if (strcmp(givenOperation, staffOperation) == 0)
        {                                                              // 给出的操作和该日志中的操作相同
            Time time = GetJournalEntryTime(journal);                  // 获取该日志的时间
            char **arguments = GetJournalEntryArguments(journal);      // 获取多个操作
            int argumentCount = GetJournalEntryArgumentCount(journal); // 获取操作个数

            TableRow *item = NewTableRow();                                  // 创建具体信息表格行
            free(AppendTableRow(item, TimeToString(GetTimeInfo(&time, 1)))); // 将时间放进去
            for (int i = 0; i < argumentCount; i++)
            { // 遍历所有操作
                char temp[50];
                AppendTableRow(title, getStringBefore(temp, arguments[i])); // 将操作名放入title标题行
                AppendTableRow(item, arguments[i]);                         // 将具体操作放入信息行
            }

            /*将这一个具体日志信息表格行加入表格中*/
            int judge = AppendTable(oneStaffAllStockJournal, item);
            if (judge != 0)
            {
                TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
                Table *remark_table = NewTable(blank_row, "读取某员工所有操作日志时出错 !"); // 指明错误信息
                return remark_table;                                                         // 返回空表格
            }
        }
        p = p->next; // 指向下一个日志
    }

    return oneStaffAllStockJournal; // 返回表格
}

/*读取一条日志（根据员工id，操作，时间）*/
Table *GetOneJournalByIdOperationTime(Table *table)
{
    if (table == NULL)
    {                                                                    // 若没有传递信息
        TableRow *blank_row = NewTableRow();                             // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有提供任何信息 !"); // 指明错误信息
        return remark_table;                                             // 返回空表格
    }

    TableRow *row = GetRowByIndex(table, 1);
    const char *givenOperation = GetRowItemByColumnName(table, row, "operation");
    const char *idString = GetRowItemByColumnName(table, row, "id");
    int staffid = atoi(idString); // 获取员工工号
    const char *givenTime = GetRowItemByColumnName(table, row, "time");

    LinkedList *staffJournalLinkedList = GetJournalsByStaffId(staffid);
    if (staffJournalLinkedList == NULL)
    {                                                                      // 若该员工没有操作日志
        TableRow *blank_row = NewTableRow();                               // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "该员工没有操作日志 !"); // 指明错误信息
        return remark_table;                                               // 返回空表格
    }
    LinkedList *p = staffJournalLinkedList;
    while (p != NULL)
    {
        JournalEntry *journal = (JournalEntry *)p->data;
        const char *oriOperation = OperationToString(GetJournalEntryOperation(journal));
        Time tempTime = GetJournalEntryTime(journal);            // 获取该日志条目中的时间
        char *oriTime = TimeToString(GetTimeInfo(&tempTime, 1)); // 将时间转化为字符串
        /*若是要查询的那条日志*/
        if (strcmp(givenOperation, oriOperation) == 0 && strcmp(givenTime, oriTime) == 0)
        { // 根据操作和时间寻找该员工的日志
            TableRow *title = NewTableRow();
            AppendTableRow(title, "time");
            Table *oneJournal = NewTable(title, "one journal");
            char **arguments = GetJournalEntryArguments(journal);      // 获取多个操作
            int argumentCount = GetJournalEntryArgumentCount(journal); // 获取操作个数

            TableRow *item = NewTableRow();                     // 创建具体信息表格行
            free(AppendTableRow(item, CloneString(givenTime))); // 将时间放进去
            for (int i = 0; i < argumentCount; i++)
            { // 遍历所有操作
                char temp[50];
                free(AppendTableRow(title, getStringBefore(temp, arguments[i]))); // 将操作名放入title标题行
                AppendTableRow(item, arguments[i]);                               // 将具体操作放入信息行
            }

            /*将这一个具体日志信息表格行加入表格中*/
            int judge = AppendTable(oneJournal, item);
            if (judge != 0)
            {
                TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
                Table *remark_table = NewTable(blank_row, "读取某个员工某个时间的某个操作日志时出错 !"); // 指明错误信息
                return remark_table; // 返回空表格
            }

            return oneJournal; // 不用再找了
        }
        free(oriTime);
        p = p->next;
    }
    /*找完了还没找到要查询的日志*/
    TableRow *blank_row = NewTableRow();                             // 创建一个空的表格行
    Table *remark_table = NewTable(blank_row, "没有要查询的日志 !"); // 指明错误信息
    return remark_table;                                             // 返回空表格
}

/*读取某操作的全部日志*/
Table *GetAllJournalOfOneOperation(Table *table)
{
    TableRow *row = GetRowByIndex(table, 1);
    const char *operationString = GetRowItemByColumnName(table, row, "operation");
    Operation operation = StringToOperation(operationString);

    LinkedList *allJournalLink = GetJournalsByOperation(operation); // 获取某个操作的链表
    if (allJournalLink == NULL)
    {                                                            // 若没有该操作的日志
        TableRow *blank_row = NewTableRow();                     // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有日志 !"); // 指明错误信息
        return remark_table;                                     // 返回空表格
    }
    TableRow *title = NewTableRow(); // 开创标题行
    AppendTableRow(title, "id");
    AppendTableRow(title, "time");
    AppendTableRow(title, "operation");
    Table *allJournalOfOneOperation = NewTable(title, "某操作的所有日志"); // 备注待修改

    JournalEntry *journal = (JournalEntry *)allJournalLink->data; // 获取第一个日志信息
    int argumentCount = GetJournalEntryArgumentCount(journal);
    char **arguments = GetJournalEntryArguments(journal);
    for (int i = 0; i < argumentCount; i++)
    {
        char temp[50];
        AppendTableRow(title, getStringBefore(temp, arguments[i])); // 向title中加入：操作的各个标题
    }

    int staffid;
    Time time;
    int judge;
    /*遍历日志信息，将信息放入table中*/
    while (allJournalLink != NULL)
    {
        TableRow *item = NewTableRow();                 // 创建具体信息行
        journal = (JournalEntry *)allJournalLink->data; // 获取journal
        staffid = GetJournalEntryStaffId(journal);
        time = GetJournalEntryTime(journal);
        arguments = GetJournalEntryArguments(journal); // 获取多个操作
        /*向table信息行中放入具体信息*/

        free(AppendTableRow(item, LongLongToString(staffid)));
        free(AppendTableRow(item, TimeToString(GetTimeInfo(&time, 1))));
        free(AppendTableRow(item, CloneString(OperationToString(operation))));
        for (int i = 0; i < argumentCount; i++)
        {
            AppendTableRow(item, arguments[i]);
        }
        /*将信息行加入表格中*/
        judge = AppendTable(allJournalOfOneOperation, item);
        if (judge != 0)
        {
            TableRow *blank_row = NewTableRow();                                     // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "读取某操作所有日志时出错 !"); // 指明错误信息
            return remark_table;                                                     // 返回空表格
        }

        allJournalLink = allJournalLink->next;
    }
    return allJournalOfOneOperation;
}

/*读某一段时间内的某操作的全部记录
** 这里参数Table*a 要有一个初始时间，一个末尾时间*/
/*需要我判断这个输入的时间是否正确吗*/
Table *GetAllJournalOfOneOperationInExactTime(Table *table)
{
    /*如果是以table作为参数传入两个个时间的话，要这么写; */
    // TableRow* titleRow = GetTableTitle(table);
    // LinkedList* titleLink = titleRow->items;
    // char* oriTimeName, * endTimeName;
    // oriTimeName = (char*)titleLink->data;//获取表格中初始时间名字
    // titleLink = titleLink->next;
    // endTimeName = (char*)titleLink->data;//获取表格中末尾时间名字

    if (table == NULL)
    {                                                                // 若没有传递信息
        TableRow *blank_row = NewTableRow();                         // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有提供信息 !"); // 指明错误信息
        return remark_table;                                         // 返回空表格
    }

    const Time *oriTime, *endTime;
    Operation operation;
    // const char* oriTimeString, * endTimeString,*operation;
    TableRow *row = GetRowByIndex(table, 1);
    const char *oriTimeString = GetRowItemByColumnName(table, row, "oriTime");     // 初始时间字符串
    const char *endTimeString = GetRowItemByColumnName(table, row, "endTime");     // 末尾时间字符串
    const char *operationString = GetRowItemByColumnName(table, row, "operation"); // 操作字符串
    oriTime = (Time *)endTimeString;                // 获取初始时间  //指针强制转换可以吗
    endTime = (Time *)endTimeString;                // 获取末尾时间  //指针强制转换可以吗
    operation = StringToOperation(operationString); // 获取操作

    if (CompareTime(oriTime, endTime) != -1)
    {                                        // 若给出的时间不符合大小要求
        TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "给出的查询时间段有问题 !"); // 指明错误信息
        return remark_table;                                                   // 返回空表格
    }
    else
    {                                                                          // 符合大小要求
        LinkedList *allJournalofOperation = GetJournalsByOperation(operation); // 获取某操作的全部日志
        if (allJournalofOperation == NULL)
        {                                                                // 若没有日志
            TableRow *blank_row = NewTableRow();                         // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "当前没有日志 !"); // 指明错误信息
            return remark_table;                                         // 返回空表格
        }
        LinkedList *oriTimeJournalLink = NULL, *endTimeJournalLink = NULL; // 初始时间日志，末尾时间日志
        LinkedList *p = allJournalofOperation;
        while (p != NULL)
        {
            JournalEntry *journal = (JournalEntry *)p->data;
            Time tempTime = GetJournalEntryTime(journal);                      // 获取该日志条目中的时间
            const char *journalTime = TimeToString(GetTimeInfo(&tempTime, 1)); // 获取当前日志的“时间”
            if (strcmp(oriTimeString, journalTime) == 0)
            {
                oriTimeJournalLink = p;
                break;
            }
            p = p->next;
        }
        /*获取末尾时间日志*/
        p = allJournalofOperation;
        while (p != NULL)
        {
            JournalEntry *journal = (JournalEntry *)p->data;
            Time tempTime = GetJournalEntryTime(journal);                      // 获取该日志条目中的时间
            const char *journalTime = TimeToString(GetTimeInfo(&tempTime, 1)); // 获取当前日志的“时间”
            if (strcmp(endTimeString, journalTime) == 0)
            {
                endTimeJournalLink = p;
                break;
            }
            p = p->next;
        }

        if (oriTimeJournalLink == NULL || endTimeJournalLink == NULL)
        {
            TableRow *blank_row = NewTableRow();                                   // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "给出的查询时间段有问题 !"); // 指明错误信息
            return remark_table;                                                   // 返回空表格
        }
        else
        {
            p = oriTimeJournalLink;
            TableRow *title = NewTableRow(); // 开创标题行
            AppendTableRow(title, "id");
            AppendTableRow(title, "time");
            AppendTableRow(title, "operation");
            Table *allJournalOfOneOperationInTimePeriod = NewTable(title, "某操作在某时间段的全部日志"); // 备注待修改

            JournalEntry *journal = (JournalEntry *)p->data; // 获取第一个日志信息
            int argumentCount = GetJournalEntryArgumentCount(journal);
            char **arguments = GetJournalEntryArguments(journal);
            for (int i = 0; i < argumentCount; i++)
            {
                char temp[50];
                AppendTableRow(title, getStringBefore(temp, arguments[i])); // 向title中加入：操作的各个标题
            }

            int staffid;
            Time time1;
            int judge;
            /*遍历从初始时间到末尾时间的日志信息，将信息放入table中*/
            while (p != endTimeJournalLink)
            {
                TableRow *item = NewTableRow();    // 创建具体信息行
                journal = (JournalEntry *)p->data; // 获取journal
                staffid = GetJournalEntryStaffId(journal);
                time1 = GetJournalEntryTime(journal);
                arguments = GetJournalEntryArguments(journal); // 获取多个操作
                /*向table信息行中放入具体信息*/
                free(AppendTableRow(item, LongLongToString(staffid)));
                free(AppendTableRow(item, TimeToString(GetTimeInfo(&time1, 1))));
                free(AppendTableRow(item, CloneString(OperationToString(operation))));
                for (int i = 0; i < argumentCount; i++)
                {
                    AppendTableRow(item, arguments[i]);
                }
                /*将信息行加入表格中*/
                judge = AppendTable(allJournalOfOneOperationInTimePeriod, item);
                if (judge != 0)
                {
                    TableRow *blank_row = NewTableRow(); // 创建一个空的表格行
                    Table *remark_table = NewTable(blank_row, "查询某操作某段时间内的全部日志时出错 !"); // 指明错误信息
                    return remark_table; // 返回空表格
                }

                p = p->next;
            }

            return allJournalOfOneOperationInTimePeriod;
        }
    }
}

/*读取某员工的所有日志*/
Table *GetAllJournalOfOneStaff(Table *staff)
{
    if (staff == NULL)
    {                                                                // 若没有传入信息
        TableRow *blank_row = NewTableRow();                         // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "没有提供信息 !"); // 指明错误信息
        return remark_table;                                         // 返回空表格
    }

    TableRow *row = GetRowByIndex(staff, 1);
    const char *idString = GetRowItemByColumnName(staff, row, "id");
    int staffid = atoi(idString); // 获取员工工号

    LinkedList *allJournalLink = GetJournalsByStaffId(staffid); // 获取某个操作的链表
    if (allJournalLink == NULL)
    {                                                                // 若该员工没有操作日志
        TableRow *blank_row = NewTableRow();                         // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "当前没有日志 !"); // 指明错误信息
        return remark_table;                                         // 返回空表格
    }
    TableRow *title = NewTableRow(); // 开创标题行
    AppendTableRow(title, "id");
    AppendTableRow(title, "time");
    AppendTableRow(title, "operation");
    Table *allJournalOfOneStaff = NewTable(title, "某员工的全部日志"); // 备注待修改

    JournalEntry *journal = (JournalEntry *)allJournalLink->data; // 获取第一个日志信息
    int argumentCount = GetJournalEntryArgumentCount(journal);
    char **arguments = GetJournalEntryArguments(journal);
    for (int i = 0; i < argumentCount; i++)
    {
        char temp[50];
        AppendTableRow(title, getStringBefore(temp, arguments[i])); // 向title中加入：操作的各个标题
    }

    Operation operation;
    Time time;
    int judge;
    /*遍历日志信息，将信息放入table中*/
    while (allJournalLink != NULL)
    {
        TableRow *item = NewTableRow();                 // 创建具体信息行
        journal = (JournalEntry *)allJournalLink->data; // 获取journal
        operation = GetJournalEntryOperation(journal);
        time = GetJournalEntryTime(journal);
        arguments = GetJournalEntryArguments(journal); // 获取多个操作
        /*向table信息行中放入具体信息*/
        free(AppendTableRow(item, LongLongToString(staffid)));
        free(AppendTableRow(item, TimeToString(GetTimeInfo(&time, 1))));
        free(AppendTableRow(item, CloneString(OperationToString(operation))));
        for (int i = 0; i < argumentCount; i++)
        {
            AppendTableRow(item, arguments[i]);
        }
        /*将信息行加入表格中*/
        judge = AppendTable(allJournalOfOneStaff, item);
        if (judge != 0)
        {
            TableRow *blank_row = NewTableRow();                                     // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "读取某员工全部日志时出错 !"); // 指明错误信息
            return remark_table;                                                     // 返回空表格
        }

        allJournalLink = allJournalLink->next;
    }
    return allJournalOfOneStaff;
}

/*读取全部日志*/
Table *GetAllJournal(__attribute__((unused)) Table *table)
{
    LinkedList *allJournalLink = GetAllJournals(); // 获取某个操作的链表
    if (allJournalLink == NULL)
    {
        TableRow *blank_row = NewTableRow();                         // 创建一个空的表格行
        Table *remark_table = NewTable(blank_row, "当前没有日志 !"); // 指明错误信息
        return remark_table;                                         // 返回空表格
    }
    TableRow *title = NewTableRow(); // 开创标题行
    AppendTableRow(title, "id");
    AppendTableRow(title, "time");
    AppendTableRow(title, "operation");
    Table *allJournals = NewTable(title, "全部日志");

    JournalEntry *journal = (JournalEntry *)allJournalLink->data; // 获取第一个日志信息
    int argumentCount = GetJournalEntryArgumentCount(journal);
    char **arguments = GetJournalEntryArguments(journal);
    for (int i = 0; i < argumentCount; i++)
    {
        char temp[50];
        AppendTableRow(title, getStringBefore(temp, arguments[i])); // 向title中加入：操作的各个标题
    }

    int staffid;
    Time time;
    int judge;
    Operation operation;
    /*遍历日志信息，将信息放入table中*/
    while (allJournalLink != NULL)
    {
        TableRow *item = NewTableRow();                 // 创建具体信息行
        journal = (JournalEntry *)allJournalLink->data; // 获取journal
        staffid = GetJournalEntryStaffId(journal);
        time = GetJournalEntryTime(journal);
        operation = GetJournalEntryOperation(journal);
        arguments = GetJournalEntryArguments(journal); // 获取多个操作
        /*向table信息行中放入具体信息*/
        free(AppendTableRow(item, LongLongToString(staffid)));
        free(AppendTableRow(item, TimeToString(GetTimeInfo(&time, 1))));
        free(AppendTableRow(item, CloneString(OperationToString(operation))));
        for (int i = 0; i < argumentCount; i++)
        {
            AppendTableRow(item, arguments[i]);
        }
        /*将信息行加入表格中*/
        judge = AppendTable(allJournals, item);
        if (judge != 0)
        {
            TableRow *blank_row = NewTableRow();                               // 创建一个空的表格行
            Table *remark_table = NewTable(blank_row, "读取全部日志时出错 !"); // 指明错误信息
            return remark_table;                                               // 返回空表格
        }

        allJournalLink = allJournalLink->next;
    }
    return allJournals;
}
