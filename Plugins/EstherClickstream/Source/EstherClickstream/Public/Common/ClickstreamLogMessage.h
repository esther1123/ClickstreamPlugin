#pragma once

struct FClickstreamLogMessage
{
	FClickstreamLogMessage(uint32 LogId = 0, const FString& LogMsg = TEXT(""))
	: Id(LogId), Message(LogMsg)
	{
	}
	uint32 Id;
	FString Message;
};
typedef TQueue<FClickstreamLogMessage, EQueueMode::Spsc> FClickstreamLogQueue;
