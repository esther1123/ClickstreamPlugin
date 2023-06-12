#include "ClickstreamLogSyncWorker.h"
#include "EstherClickstreamModule.h"
#include "Common/ClickstreamLogging.h"
#include "Database/ClickstreamLogSQLiteDatabase.h"
#include "..\HTTP\ClickstreamHTTPClient.h"

ClickstreamLogSyncWorker::ClickstreamLogSyncWorker(const FTimespan& InThreadTickRate, uint32 BatchCount, const FString& ThreadDescription)
 : Super(InThreadTickRate, ThreadDescription),
   LogSyncBatchCount(BatchCount)
{
	SendQueue = MakeShareable(new FClickstreamLogQueue);
	SQLiteDatabase = MakeShareable(new FClickstreamLogSQLiteDatabase(FEstherClickstreamModule::Get().SQLiteDBInstance));
	LogServiceClient = MakeShareable(new ClickstreamNormalHTTPClient);
	LogServiceClient->OnSendLogMessageResponse.BindRaw(this, &ClickstreamLogSyncWorker::SendLogMessageResponseCallback);
}

void ClickstreamLogSyncWorker::SendLog(const FClickstreamLogMessage& LogMessage)
{
	SendQueue->Enqueue(LogMessage);
}

void ClickstreamLogSyncWorker::ThreadTick()
{
	if(ThreadTickRate.GetTotalSeconds() > 0 && bIsRealThread)
	{
		Wait(ThreadTickRate.GetTotalSeconds());
	}

	// Batch fetch log messages from local SQLite database
	SQLiteDatabase->QueryLogMessages(LogSyncBatchCount, SendQueue);

	// Send all log messages to Clickstream service
	while (!SendQueue->IsEmpty())
	{
		FClickstreamLogMessage LogMessage;
		SendQueue->Dequeue(LogMessage);
		LogServiceClient->SendLogMessage(LogMessage);

		if (!bIsRealThread)
		{
			break;
		}
	}
}

void ClickstreamLogSyncWorker::SendLogMessageResponseCallback(int ResponseCode, int LogMessageId)
{
	UE_LOG(LogEstherClickstream, Warning, TEXT("ClickstreamLogSyncWorker::SendLogMessageResponseCallback, ResponseCode: %d, LogMessageId: %d"), ResponseCode, LogMessageId);
	// Only delete log messages after receiving 2xx response
	if (ResponseCode >= 200 && ResponseCode < 300)
	{
		SQLiteDatabase->DeleteLogMessage(LogMessageId);
	}
}
