// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/ClickstreamBPFunctionLib.h"
#include "EstherClickstreamModule.h"

void UClickstreamBPFunctionLib::LogRawData(const UObject* WorldContextObject, const FString& Message)
{
	FEstherClickstreamModule::Get().LogRawData(Message);
}
