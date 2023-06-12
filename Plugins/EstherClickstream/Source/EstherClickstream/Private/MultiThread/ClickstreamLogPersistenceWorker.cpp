#include "ClickstreamLogPersistenceWorker.h"
#include "EstherClickstreamModule.h"
#include "Common/ClickstreamLogging.h"
#include "Database/ClickstreamLogSQLiteDatabase.h"

FClickstreamLogPersistenceWorker::FClickstreamLogPersistenceWorker(const FTimespan& InThreadTickRate, const FString& ThreadDescription)
	: Super(InThreadTickRate, ThreadDescription)
{
	InsertQueue = MakeShareable(new FClickstreamLogQueue);
	SQLiteDatabase = MakeShareable(new FClickstreamLogSQLiteDatabase(FEstherClickstreamModule::Get().SQLiteDBInstance));
}

void FClickstreamLogPersistenceWorker::SaveLogMessage(const FClickstreamLogMessage& LogMessage)
{
	InsertQueue->Enqueue(LogMessage);
}

void FClickstreamLogPersistenceWorker::ThreadTick()
{
	//Throttle Thread to avoid consuming un-needed resources <3 Rama
	// Set during thread startup, can be modified any time!
	if(ThreadTickRate.GetTotalSeconds() > 0 && bIsRealThread)
	{
		Wait(ThreadTickRate.GetTotalSeconds());
	}
	
	while (!InsertQueue->IsEmpty())
	{
		FClickstreamLogMessage LogMessage;
		InsertQueue->Dequeue(LogMessage);
		UE_LOG(LogEstherClickstream, Warning, TEXT("Insert log message: [%d, %s] into SQLite database"), LogMessage.Id, *LogMessage.Message);
		
		SQLiteDatabase->InsertLogMessage(LogMessage);

		if (!bIsRealThread)
		{
			break;
		}
	}
}
