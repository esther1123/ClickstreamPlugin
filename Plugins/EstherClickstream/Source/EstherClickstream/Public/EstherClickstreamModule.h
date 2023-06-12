// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FEstherClickstreamModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void LogRawData(const FString& Message);

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	ESTHERCLICKSTREAM_API static FEstherClickstreamModule& Get();

	TSharedPtr<class FSQLiteDatabase> SQLiteDBInstance;

private:
	/** singleton for the module while loaded and available */
	static FEstherClickstreamModule* Singleton;
	
	class FClickstreamLogPersistenceWorker* LogPersistenceWorker = nullptr;
	class ClickstreamLogSyncWorker* LogSyncWorker = nullptr;

	double LogPersistenceInterval = 1.0;
	double LogSyncInterval = 1.0;
};
