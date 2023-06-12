// Copyright Epic Games, Inc. All Rights Reserved.

#include "..\Public\EstherClickstreamModule.h"
#include "Common/ClickstreamLogging.h"
#include "Common/ClickstreamSettings.h"
#include "SQLiteDatabase.h"
#include "MultiThread/ClickstreamLogPersistenceWorker.h"
#include "MultiThread/ClickstreamLogSyncWorker.h"

#define LOCTEXT_NAMESPACE "FEstherClickstreamModule"

FEstherClickstreamModule* FEstherClickstreamModule::Singleton = nullptr;

void FEstherClickstreamModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	Singleton = this;

	// Get settings in 'Project Settings->Esther's Clickstream'
	const UClickstreamSettings* ClickstreamSettings = GetDefault<UClickstreamSettings>();
	
	FString DatabaseRootPath = FPaths::ProjectPluginsDir() + "EstherClickstream/Resources/";
	FString DatabaseFileName = "LogDatabase.sqlite";
	FString DatabaseFilePath = DatabaseRootPath + DatabaseFileName;
	UE_LOG(LogEstherClickstream, Warning, TEXT("DatabaseFilePath: %s"), *DatabaseFilePath);

	// Open local database
	SQLiteDBInstance = MakeShareable(new FSQLiteDatabase);
	if (!SQLiteDBInstance->Open(*DatabaseFilePath, ESQLiteDatabaseOpenMode::ReadWrite) ||
		!SQLiteDBInstance->IsValid())
	{
		UE_LOG(LogEstherClickstream, Warning, TEXT("Failed to open SQLite database: %s"),
			*SQLiteDBInstance->GetLastError());
		return;
	}

	// Enable LogPersistenceWorker or not
	if (ClickstreamSettings->EnableLogPersistence)
	{
		// Clamp persistence interval, min is 1 seconds
		LogPersistenceInterval = FMath::Clamp(ClickstreamSettings->LogPersistenceInterval, 1, ClickstreamSettings->LogPersistenceInterval);
		FTimespan ThreadWaitTime = FTimespan::FromSeconds(LogPersistenceInterval);
		FString UniqueThreadName = "LogPersistenceWorker";
		LogPersistenceWorker = new FClickstreamLogPersistenceWorker(ThreadWaitTime, UniqueThreadName);
	}
	// Enable LogSyncWorker or not
	if (ClickstreamSettings->EnableLogSync)
	{
		// Clamp sync to Clickstream service interval, min is 1 seconds
		LogSyncInterval = FMath::Clamp(ClickstreamSettings->LogSyncInterval, 1, ClickstreamSettings->LogSyncInterval);
		uint32 LogSyncBatchCount = FMath::Clamp(ClickstreamSettings->LogSyncBatchCount, 1, ClickstreamSettings->LogSyncBatchCount);
		FTimespan ThreadWaitTime = FTimespan::FromSeconds(LogSyncInterval);
		FString UniqueThreadName = "LogSyncWorker";
		LogSyncWorker = new ClickstreamLogSyncWorker(ThreadWaitTime, LogSyncBatchCount, UniqueThreadName);
	}
}

void FEstherClickstreamModule::ShutdownModule()
{
	// Stop log persistence thread
	if (LogPersistenceWorker)
	{
		LogPersistenceWorker->Stop();
		while(!LogPersistenceWorker->HasStopped())
		{
			FPlatformProcess::Sleep(LogPersistenceInterval / 10.0);
		}
		delete LogPersistenceWorker;
		LogPersistenceWorker = nullptr;
	}

	// Stop log sync thread
	if (LogSyncWorker)
	{
		LogSyncWorker->Stop();
		while(!LogSyncWorker->HasStopped())
		{
			FPlatformProcess::Sleep(LogSyncInterval / 10.0);
		}
		delete LogSyncWorker;
		LogSyncWorker = nullptr;
	}

	// Close local log database
	if (!SQLiteDBInstance->Close())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to close database: %s"), *SQLiteDBInstance->GetLastError());
	}
	
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	Singleton = nullptr;
}

void FEstherClickstreamModule::LogRawData(const FString& Message)
{
	if (Message.Len() == 0)
	{
		UE_LOG(LogEstherClickstream, Warning, TEXT("Log message is empty, ignore"));
		return;
	}
	
	if (LogPersistenceWorker)
	{
		LogPersistenceWorker->SaveLogMessage(FClickstreamLogMessage(FMath::Rand(), Message));
	}
	else
	{
		UE_LOG(LogEstherClickstream, Warning,
			TEXT("Failed to save log message: %s. please check 'EnableLogPersistence' in 'Project Settings->Esther's Clickstream' first"), *Message);
	}
}

FEstherClickstreamModule& FEstherClickstreamModule::Get()
{
	if (Singleton == nullptr)
	{
		check(IsInGameThread());
		FModuleManager::LoadModuleChecked<FEstherClickstreamModule>("EstherClickstream");
	}
	check(Singleton != nullptr);
	return *Singleton;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FEstherClickstreamModule, EstherClickstream)
