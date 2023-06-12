#pragma once

#include "Interfaces/IHttpRequest.h"

#define LOG_MESSAGE_ID_HEADER "X-Log-Message-Id"
DECLARE_DELEGATE_TwoParams(FOnSendLogMessageResponse, int /*ResponseCode*/, int /*LogMessageId*/);

class ClickstreamNormalHTTPClient
{
public:
	// Send Clickstream log message to service endpoint
	FOnSendLogMessageResponse& SendLogMessage(const struct FClickstreamLogMessage& LogMessage);
	void OnSendLogMessageResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	FOnSendLogMessageResponse OnSendLogMessageResponse;
};
