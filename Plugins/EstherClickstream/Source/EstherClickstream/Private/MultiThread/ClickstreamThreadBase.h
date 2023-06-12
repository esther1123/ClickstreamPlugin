#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"
#include "Misc/SingleThreadRunnable.h"

// Inspired by Rama EverNewJoy's "Rama Code: Multi-Threading in UE5 C++"
// Reference:
// https://dev.epicgames.com/community/learning/tutorials/7Rz/unreal-engine-rama-code-multi-threading-in-ue5-c
class FClickstreamThreadBase : public FRunnable, FSingleThreadRunnable
{
public:
	FClickstreamThreadBase(const FTimespan& TickRate, const FString& Description);
	virtual ~FClickstreamThreadBase();

	// FSingleThreadRunnable interface
	virtual void Tick() override;
	
	// FRunnable interface
	// Returns a pointer to the single threaded interface when mulithreading is disabled.
	virtual FSingleThreadRunnable* GetSingleThreadInterface() override
	{
		return this;
	}
	
	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	
	void SetPaused(bool InPaused);
	bool IsPaused();
	bool IsVerifiedSuspended();
	bool HasStopped();
	
protected:
	// FClickstreamThreadBase interface
	virtual void ThreadTick() {}
	void Wait(float Seconds);
	
	FThreadSafeBool Paused;
	FThreadSafeBool VerifiedSuspended;
	FThreadSafeBool Stopped;
	
	bool Stopping;
	bool bIsRealThread;

	// Holds the thread object
	FRunnableThread* Thread = nullptr;
	// Holds the amount of time to wait
	FTimespan ThreadTickRate;
};
