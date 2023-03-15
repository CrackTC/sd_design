#include "journal.h"
#include "../utils.h"
#include "linkedList.h"
#include "serialization.h"
#include "table.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *path = "data/journal.txt";
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

    entry->arguments = malloc(argumentCount * sizeof(char *));
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

LinkedList *GetAllJournals()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
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

        int staffId;
        Time time;
        Operation operation;
        char **arguments;
        int argumentCount;

        char *joinedArgument;

        sscanf(GetRowItemByColumnName(table, row, staffIdRow), "%d", &staffId);
        sscanf(GetRowItemByColumnName(table, row, timeRow), "%ld", &time.value);
        sscanf(GetRowItemByColumnName(table, row, operationRow), "%d", &operation);
        joinedArgument = CloneString(GetRowItemByColumnName(table, row, argumentsRow));
        sscanf(GetRowItemByColumnName(table, row, argumentCountRow), "%d", &argumentCount);

        arguments = malloc(argumentCount * sizeof(char *));
        int startIndex = 0;
        int endIndex = 0;
        int argumentIndex = 0;
        while (argumentIndex < argumentCount)
        {
            while (joinedArgument[endIndex] != '\n' && joinedArgument[endIndex] != '\0')
                endIndex++;
            int length = endIndex - startIndex + 1;
            arguments[argumentIndex] = malloc(length * sizeof(char));
            for (int i = 0; i < length; i++)
            {
                arguments[argumentIndex][i] = joinedArgument[startIndex + i];
            }
            endIndex++;
            startIndex = endIndex;
            argumentIndex++;
        }

        free(joinedArgument);

        JournalEntry *entry = malloc(sizeof(JournalEntry));
        entry->staffId = staffId;
        entry->time = time;
        entry->operation = operation;
        entry->arguments = arguments;
        entry->argumentCount = argumentCount;

        LinkedList *node = malloc(sizeof(LinkedList));
        node->data = entry;
        node->next = NULL;

        list = AppendNode(list, node);
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
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = entry;
            node->next = NULL;
            list = AppendNode(list, node);
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
            LinkedList *node = malloc(sizeof(LinkedList));
            node->data = entry;
            node->next = NULL;
            list = AppendNode(list, node);
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
    if (systemList == NULL) {
        GetAllJournals();
    }
    if (entry == NULL) {
        return 1;
    }
    if (ExistsNode(systemList, entry)) {
        return 1;
    }

    LinkedList *node = malloc(sizeof(LinkedList));
    node->data = entry;
    node->next = NULL;
    systemList = AppendNode(systemList, node);

    return 0;
}

char *JoinArguments(char **arguments, int argumentCount)
{
    int length = 0;
    for (int i = 0; i < argumentCount; i++)
    {
        length += strlen(arguments[i]) + 1;
    }
    char *result = malloc(length * sizeof(char));
    int resultIndex = 0;
    for (int i = 0; i < argumentCount; i++) {
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
    AppendTableRow(row, staffIdRow);
    AppendTableRow(row, timeRow);
    AppendTableRow(row, operationRow);
    AppendTableRow(row, argumentsRow);
    AppendTableRow(row, argumentCountRow);

    Table *table = NewTable(row, NULL);

    LinkedList *now = systemList;
    while (now != NULL) {
        JournalEntry *entry = malloc(sizeof(JournalEntry));
        row = NewTableRow();

        char *staffIdString = LongLongToString(entry->staffId);
        char *timeString = LongLongToString(entry->time.value);
        char *operationString = LongLongToString(entry->operation);
        char *argumentsString = JoinArguments(entry->arguments, entry->argumentCount);
        char *argumentCountString = LongLongToString(entry->argumentCount);

        AppendTableRow(row, staffIdString);
        AppendTableRow(row, timeString);
        AppendTableRow(row, operationString);
        AppendTableRow(row, argumentsString);
        AppendTableRow(row, argumentCountString);

        free(staffIdString);
        free(timeString);
        free(operationString);
        free(argumentsString);
        free(argumentCountString);

        AppendTable(table, row);
        now = now->next;
    }

    Serialize(table, path);
    FreeTable(table);
}
