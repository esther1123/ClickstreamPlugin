#include "ClickstreamThreadBase.h"

FClickstreamThreadBase::FClickstreamThreadBase(const FTimespan& TickRate, const FString& Description)
	: Stopping(false), ThreadTickRate(TickRate)
{
	Paused.AtomicSet(false);
	bIsRealThread = FPlatformProcess::SupportsMultithreading();
	Thread = FRunnableThread::Create(this, *Description, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
}

FClickstreamThreadBase::~FClickstreamThreadBase()
{
	if (Thread != nullptr)
	{
		Thread->Kill(true);
		delete Thread;
	}
}

void FClickstreamThreadBase::Tick()
{
	ThreadTick();
}

bool FClickstreamThreadBase::Init()
{
	return true;
}

uint32 FClickstreamThreadBase::Run()
{
	Stopped.AtomicSet(false);
	
	while (!Stopping)
	{
		if (Paused)
		{
			if (!VerifiedSuspended)
			{
				VerifiedSuspended.AtomicSet(true);
			}

			Wait(ThreadTickRate.GetTotalSeconds());
				
			continue;
		}

		ThreadTick();
	}

	Stopped.AtomicSet(true);
	
	return 0;
}

void FClickstreamThreadBase::Stop()
{
	SetPaused(true);
	Stopping = true;
}

void FClickstreamThreadBase::Wait(float Seconds)
{
	FPlatformProcess::Sleep(Seconds);
}

void FClickstreamThreadBase::SetPaused(bool InPaused)
{
	Paused.AtomicSet(InPaused);
	if(!InPaused)
	{
		VerifiedSuspended.AtomicSet(false);
	}
}

bool FClickstreamThreadBase::IsPaused()
{
	return Paused;
}

bool FClickstreamThreadBase::IsVerifiedSuspended()
{
	return VerifiedSuspended;
}

bool FClickstreamThreadBase::HasStopped()
{
	return Stopped;
}
