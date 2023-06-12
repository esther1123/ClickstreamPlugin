// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClickstreamBPFunctionLib.generated.h"

/**
 * 
 */
UCLASS()
class ESTHERCLICKSTREAM_API UClickstreamBPFunctionLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	// Log raw data to local SQLite database
	UFUNCTION(BlueprintCallable, Category = "AWS|Esther|Clickstream",
		meta = (WorldContext = "WorldContextObject", DisplayName = "Log Raw Data", Keywords = "esther 1123 clickstream log raw data"))
	static void LogRawData(const UObject* WorldContextObject, const FString& Message);
};
