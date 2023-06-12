#pragma once

#include "ClickstreamThreadBase.h"
#include "Common/ClickstreamLogMessage.h"

class FClickstreamLogPersistenceWorker : public FClickstreamThreadBase
{
public:
	typedef FClickstreamThreadBase Super;
	FClickstreamLogPersistenceWorker(const FTimespan& InThreadTickRate, const FString& ThreadDescription);

	void SaveLogMessage(const FClickstreamLogMessage& LogMessage);

protected:
	virtual void ThreadTick() override;

private:
	TSharedPtr<FClickstreamLogQueue> InsertQueue;
	TSharedPtr<class FClickstreamLogSQLiteDatabase> SQLiteDatabase;
};
