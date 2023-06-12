#pragma once

class FClickstreamProtocol
{
public:
	bool Init(const class UClickstreamSettings* ClickstreamSettings);

	// Temp functions
	FString GetRequestUrl();
	FString EncodeLogMessage(const FString& LogMessage);

	ESTHERCLICKSTREAM_API static FClickstreamProtocol& Get();

private:
	static FClickstreamProtocol* Singleton;

	FString InvokeUrl;
	FString AppId;
	FString AppVersion;
	FString Compression;

	FString Platform;
	FString SdkVersion;
	
	uint32 HashCode = 0;
};
