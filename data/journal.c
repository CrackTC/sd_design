#include "design/journal.h"
#include "../config.h"
#include "design/utils.h"
#include "design/linkedList.h"
#include "design/serialization.h"
#include "design/table.h"
#include "design/time.h"
#include "design/operation.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

static const char *fileName = "data" PATH_SEPARATOR_STRING "journal.txt";
static const char *staffIdRow = "staffId";
static const char *timeRow = "time";
static const char *operationRow = "operation";
static const char *argumentsRow = "arguments";
static const char *argumentCountRow = "argumentCount";
static LinkedList *systemList = NULL;

struct JournalEntry
{
    int staffId;
    Time time;
    Operation operation;
    char **arguments;
    int argumentCount;
};

JournalEntry *NewJournalEntry(int staffId, Time *time, Operation operation, const char **arguments, int argumentCount)
{
    if (staffId < 0 || argumentCount < 0)
        return NULL;

    JournalEntry *entry = malloc(sizeof(JournalEntry));
    entry->staffId = staffId;
    entry->time = *time;
    entry->operation = operation;

    entry->arguments = argumentCount == 0 ? NULL : malloc(argumentCount * sizeof(char *));
    for (int i = 0; i < argumentCount; i++)
        entry->arguments[i] = CloneString(arguments[i]);

    entry->argumentCount = argumentCount;

    return entry;
}

void FreeJournalEntry(JournalEntry *entry)
{
    for (int i = 0; i < entry->argumentCount; i++)
        free(entry->arguments[i]);

    free(entry->arguments);
    free(entry);
}

char **ExplodeArguments(const char *joinedArgument, int argumentCount)
{
    char **arguments = malloc(argumentCount * sizeof(char *));
    int startIndex = 0;
    int endIndex = 0;
    int argumentIndex = 0;
    while (argumentIndex < argumentCount)
    {
        while (joinedArgument[endIndex] != '\n' && joinedArgument[endIndex] != '\0')
            endIndex++;
        int length = endIndex - startIndex;
        arguments[argumentIndex] = malloc((length + 1) * sizeof(char));
        for (int i = 0; i < length; i++)
        {
            arguments[argumentIndex][i] = joinedArgument[startIndex + i];
        }
        arguments[argumentIndex][length] = '\0';
        endIndex++;
        startIndex = endIndex;
        argumentIndex++;
    }

    return arguments;
}

LinkedList *GetAllJournals()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, fileName);
    if (result == 1)
    {
        JournalSave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL)
    {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        JournalEntry *entry = malloc(sizeof(JournalEntry));

        sscanf(GetRowItemByColumnName(table, row, staffIdRow), "%d", &entry->staffId);
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%lld", &entry->time.value);
        sscanf(GetRowItemByColumnName(table, row, operationRow), "%d", (int *)&entry->operation);
        char *joinedArgument = CloneString(GetRowItemByColumnName(table, row, argumentsRow));
        sscanf(GetRowItemByColumnName(table, row, argumentCountRow), "%d", &entry->argumentCount);

        entry->arguments = ExplodeArguments(joinedArgument, entry->argumentCount);
        free(joinedArgument);

        list = AppendData(list, entry);
    }

    FreeTable(table);

    systemList = list;
    return list;
}

LinkedList *GetJournalsByStaffId(int staffId)
{
    if (systemList == NULL)
        GetAllJournals();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        JournalEntry *entry = now->data;
        if (entry->staffId == staffId)
        {
            list = AppendData(list, entry);
        }
        now = now->next;
    }

    return list;
}

LinkedList *GetJournalsByOperation(Operation operation)
{
    if (systemList == NULL)
        GetAllJournals();

    LinkedList *list = NULL;
    LinkedList *now = systemList;
    while (now != NULL)
    {
        JournalEntry *entry = now->data;
        if (entry->operation == operation)
        {
            list = AppendData(list, entry);
        }
        now = now->next;
    }

    return list;
}

int GetJournalEntryStaffId(const JournalEntry *entry)
{
    return entry->staffId;
}

Time GetJournalEntryTime(const JournalEntry *entry)
{
    return entry->time;
}

Operation GetJournalEntryOperation(const JournalEntry *entry)
{
    return entry->operation;
}

char **GetJournalEntryArguments(const JournalEntry *entry)
{
    return entry->arguments;
}

int GetJournalEntryArgumentCount(const JournalEntry *entry)
{
    return entry->argumentCount;
}

int AppendJournalEntry(JournalEntry *entry)
{
    if (systemList == NULL)
    {
        GetAllJournals();
    }
    if (entry == NULL)
    {
        return 1;
    }
    if (ExistsNode(systemList, entry))
    {
        return 1;
    }

    systemList = AppendData(systemList, entry);
    return 0;
}

char *JoinArguments(char **arguments, int argumentCount)
{
    if (argumentCount == 0)
    {
        return CloneString("");
    }
    int length = 0;
    for (int i = 0; i < argumentCount; i++)
    {
        length += strlen(arguments[i]) + 1;
    }
    char *result = malloc(length * sizeof(char));
    int resultIndex = 0;
    for (int i = 0; i < argumentCount; i++)
    {
        for (int j = 0; arguments[i][j] != '\0'; j++)
        {
            result[resultIndex++] = arguments[i][j];
        }
        result[resultIndex++] = i == argumentCount - 1 ? '\0' : '\n';
    }

    return result;
}

void JournalSave()
{
    TableRow *row = NewTableRow();
    free(AppendTableRow(row, CloneString(staffIdRow)));
    free(AppendTableRow(row, CloneString(timeRow)));
    free(AppendTableRow(row, CloneString(operationRow)));
    free(AppendTableRow(row, CloneString(argumentsRow)));
    free(AppendTableRow(row, CloneString(argumentCountRow)));

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL)
    {
        JournalEntry *entry = now->data;
        row = NewTableRow();

        free(AppendTableRow(row, LongLongToString(entry->staffId)));
        free(AppendTableRow(row, LongLongToString(entry->time.value)));
        free(AppendTableRow(row, LongLongToString(entry->operation)));
        free(AppendTableRow(row, JoinArguments(entry->arguments, entry->argumentCount)));
        free(AppendTableRow(row, LongLongToString(entry->argumentCount)));

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, fileName);
    FreeTable(table);
}
