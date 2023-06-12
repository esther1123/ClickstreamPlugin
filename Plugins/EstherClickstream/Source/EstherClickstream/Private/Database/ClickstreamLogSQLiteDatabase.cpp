#include "ClickstreamLogSQLiteDatabase.h"
#include "SQLitePreparedStatement.h"
#include "Common/ClickstreamLogging.h"
#include "Common/ClickstreamLogMessage.h"

FClickstreamLogSQLiteDatabase::FClickstreamLogSQLiteDatabase(TWeakPtr<FSQLiteDatabase> DBInstance)
	: SQLiteDBInstance(DBInstance)
{
	const TCHAR* InsertLogQuery = TEXT("insert into LogStream (Id, Message) values ($Id, $Message)");
	InsertLogStatement = new FSQLitePreparedStatement;
	InsertLogStatement->Create(*SQLiteDBInstance.Pin(), InsertLogQuery, ESQLitePreparedStatementFlags::Persistent);

	const TCHAR* DeleteLogQuery = TEXT("delete from LogStream where Id = $Id");
	DeleteLogStatement = new FSQLitePreparedStatement;
	DeleteLogStatement->Create(*SQLiteDBInstance.Pin(), DeleteLogQuery, ESQLitePreparedStatementFlags::Persistent);

	const TCHAR* SelectLogQuery = TEXT("select * from LogStream limit $RowCount");
	QueryLogStatement = new FSQLitePreparedStatement;
	QueryLogStatement->Create(*SQLiteDBInstance.Pin(), SelectLogQuery, ESQLitePreparedStatementFlags::Persistent);
}

FClickstreamLogSQLiteDatabase::~FClickstreamLogSQLiteDatabase()
{
	InsertLogStatement->Destroy();
	DeleteLogStatement->Destroy();
	QueryLogStatement->Destroy();
}

bool FClickstreamLogSQLiteDatabase::InsertLogMessage(const FClickstreamLogMessage& LogMessage)
{
	if (!SQLiteDBInstance.IsValid() || !InsertLogStatement->IsValid())
	{
		return false;
	}
	
	InsertLogStatement->Reset();
	bool bBindingSuccess = true;
	bBindingSuccess = bBindingSuccess && InsertLogStatement->SetBindingValueByName(TEXT("$Id"), LogMessage.Id);
	bBindingSuccess = bBindingSuccess && InsertLogStatement->SetBindingValueByName(TEXT("$Message"), LogMessage.Message);
	if (!bBindingSuccess || !InsertLogStatement->Execute())
	{
		return false;
	}

	return true;
}

bool FClickstreamLogSQLiteDatabase::DeleteLogMessage(uint32 LogMessageId)
{
	if (!SQLiteDBInstance.IsValid() || !DeleteLogStatement->IsValid())
	{
		return false;
	}
	
	DeleteLogStatement->Reset();
	bool bBindingSuccess = DeleteLogStatement->SetBindingValueByName(TEXT("$Id"), LogMessageId);
	if (!bBindingSuccess || !DeleteLogStatement->Execute())
	{
		return false;
	}

	return true;
}

bool FClickstreamLogSQLiteDatabase::QueryLogMessages(uint32 RowCount, TWeakPtr<FClickstreamLogQueue> LogQueue)
{
	if (!SQLiteDBInstance.IsValid() || !QueryLogStatement->IsValid() || !LogQueue.IsValid())
	{
		return false;
	}
	
	QueryLogStatement->Reset();
	bool bBindingSuccess = QueryLogStatement->SetBindingValueByName(TEXT("$RowCount"), RowCount);
	if (!bBindingSuccess || !QueryLogStatement->Execute())
	{
		return false;
	}

	// Loop while there are result rows to process...
	while (QueryLogStatement->Step() == ESQLitePreparedStatementStepResult::Row)
	{
		// Example of getting the data for a column
		FClickstreamLogMessage LogMessage;
		QueryLogStatement->GetColumnValueByIndex(0, LogMessage.Id);
		QueryLogStatement->GetColumnValueByIndex(1, LogMessage.Message);
		UE_LOG(LogEstherClickstream, Warning, TEXT("Query log message: [Id: %d, Message: %s] from SQLite database"), LogMessage.Id, *LogMessage.Message);

		LogQueue.Pin()->Enqueue(LogMessage);
	}

	return true;
}
