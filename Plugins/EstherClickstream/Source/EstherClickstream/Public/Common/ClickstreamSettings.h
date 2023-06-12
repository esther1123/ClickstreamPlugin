// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ClickstreamSettings.generated.h"

/**
 * NOTE: For performance reasons, you NEED to restart the UE Editor after
 *       modifying the following properties
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Esther's Clickstream"))
class ESTHERCLICKSTREAM_API UClickstreamSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UClickstreamSettings()
	{
		CategoryName = "Plugins";
	}
	
	// Clickstream service's HTTP(s) URL(including resource path)
	UPROPERTY(config, EditAnywhere, Category = "Service Config")
	FString EndpointUrl;

	// Clickstream service's App ID
	UPROPERTY(config, EditAnywhere, Category = "Service Config")
	FString AppId;

	// Clickstream service's App version
	UPROPERTY(config, EditAnywhere, Category = "Service Config")
	FString AppVersion;

	// Log message's compression mode. 'gzip' or empty for not compress
	UPROPERTY(config, EditAnywhere, Category = "Service Config")
	FString Compression;

	// Interval(in seconds) of log message persistence to local database
	UPROPERTY(config, EditAnywhere, Category = "Plugin Config")
	uint32 LogPersistenceInterval;

	// Interval(in seconds) of log message sync with Clickstream service
	UPROPERTY(config, EditAnywhere, Category = "Plugin Config")
	uint32 LogSyncInterval;

	// Max log message count in a sync cycle
	UPROPERTY(config, EditAnywhere, Category = "Plugin Config")
	uint32 LogSyncBatchCount;

	// Enable/Disable log persistence
	UPROPERTY(config, EditAnywhere, Category = "Plugin Config")
	bool EnableLogPersistence;

	// Enable/Disable log sync with Clickstream service
	UPROPERTY(config, EditAnywhere, Category = "Plugin Config")
	bool EnableLogSync;
};
