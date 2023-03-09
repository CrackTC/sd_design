#include "journal.h"

struct JournalEntry
{
    const int staffId;
    const Time *time;
    const Operation operation;
    const char **arguments;
};

JournalEntry *NewJournalEntry(int staffId, Time *time, Operation operation, const char **arguments)
{

}

void FreeJournalEntry(JournalEntry *entry)
{

}

const LinkedList *GetAllJournals()
{

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
