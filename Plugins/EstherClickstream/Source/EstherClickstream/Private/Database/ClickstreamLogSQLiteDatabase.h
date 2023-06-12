#pragma once

#include "Common/ClickstreamLogMessage.h"

struct FClickstreamLogMessage;

// Inspired by Karasa's "Unreal Engine C++ Tutorial | How To Save Data With SQLite Database"
// and Ysgrathe's "SQLiteGameDb"
// References:
// https://www.youtube.com/watch?v=1vjK5aVtPUA
// https://github.com/Ysgrathe/SQLiteGameDb
class FClickstreamLogSQLiteDatabase
{
public:
	FClickstreamLogSQLiteDatabase(TWeakPtr<class FSQLiteDatabase> DBInstance);
	~FClickstreamLogSQLiteDatabase();

	bool InsertLogMessage(const FClickstreamLogMessage& LogMessage);
	bool DeleteLogMessage(uint32 LogMessageId);
	bool QueryLogMessages(uint32 RowCount, TWeakPtr<FClickstreamLogQueue> LogQueue);

private:
	// Pointer to the underlying SQLite Db object
	TWeakPtr<class FSQLiteDatabase> SQLiteDBInstance;

	class FSQLitePreparedStatement* InsertLogStatement = nullptr;
	class FSQLitePreparedStatement* DeleteLogStatement = nullptr;
	class FSQLitePreparedStatement* QueryLogStatement = nullptr;
};
