#pragma once

#include "ClickstreamThreadBase.h"
#include "Common/ClickstreamLogMessage.h"

class ClickstreamLogSyncWorker : public FClickstreamThreadBase
{
public:
	typedef FClickstreamThreadBase Super;
	ClickstreamLogSyncWorker(const FTimespan& InThreadTickRate, uint32 BatchCount, const FString& ThreadDescription);
	
	void SendLog(const FClickstreamLogMessage& LogMessage);

protected:
	virtual void ThreadTick() override;

private:
	TSharedPtr<FClickstreamLogQueue> SendQueue;
	TSharedPtr<class ClickstreamNormalHTTPClient> LogServiceClient;
	TSharedPtr<class FClickstreamLogSQLiteDatabase> SQLiteDatabase;
	
	uint32 LogSyncBatchCount = 1;

	void SendLogMessageResponseCallback(int ResponseCode, int LogMessageId);
};
