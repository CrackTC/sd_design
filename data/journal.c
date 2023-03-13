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

const LinkedList *GetAllJournals()
{
    if (systemList != NULL)
        return systemList;

    Table *table;
    int result = Unserialize(&table, path);
    if (result == 1) {
        JournalSave();
        return NULL;
    }
    else if (result != 0)
        return NULL;

    LinkedList *list = NULL;
    LinkedList *rowNode = table->rows;
    while (rowNode->next != NULL) {
        rowNode = rowNode->next;
        const TableRow *row = rowNode->data;

        int staffId;
        Time time;
        Operation operation;
        char **arguments;
        int argumentCount;

        char *joinedArgument;

        sscanf("%d", GetRowItemByColumnName(table, row, staffIdRow), &staffId);
        sscanf("%d", GetRowItemByColumnName(table, row, timeRow), &time.value);
        sscanf("%d", GetRowItemByColumnName(table, row, operationRow), &operation);
        joinedArgument = CloneString(GetRowItemByColumnName(table, row, argumentsRow));
        sscanf("%d", GetRowItemByColumnName(table, row, argumentCountRow), &argumentCount);

        arguments = malloc(argumentCount * sizeof(char *));
        int startIndex = 0;
        int endIndex = 0;
        int argumentIndex = 0;
        while (argumentIndex < argumentCount) {
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
    }
}

const LinkedList *GetJournalsByStaffId(int staffId)
{

}

const LinkedList *GetJournalsByOperation(Operation operation)
{

}

int GetJournalEntryStaffId(const JournalEntry *entry)
{

}

Time *GetJournalEntryTime(const JournalEntry *entry)
{

}

Operation GetJournalEntryOperation(const JournalEntry *entry)
{

}

const char **GetJournalEntryArguments(const JournalEntry *entry)
{

}

int AppendJournalEntry(JournalEntry *entry)
{

}

void JournalSave()
{

}
