#include "ClickstreamHTTPClient.h"

#include "HttpModule.h"
#include "Common/ClickstreamLogMessage.h"
#include "ClickstreamProtocol.h"
#include "Interfaces/IHttpResponse.h"

FOnSendLogMessageResponse& ClickstreamNormalHTTPClient::SendLogMessage(const FClickstreamLogMessage& LogMessage)
{
	// NOTE: Must call GetRequestBody before GetRequestUrl
	const FString Content = FClickstreamProtocol::Get().EncodeLogMessage(LogMessage.Message);
	const FString InvokeUrl = FClickstreamProtocol::Get().GetRequestUrl();
	const FString InvokeMethod("POST");
	
	TSharedRef<IHttpRequest> SendLogMessageRequest = FHttpModule::Get().CreateRequest();
	SendLogMessageRequest->SetURL(InvokeUrl);
	SendLogMessageRequest->SetVerb(InvokeMethod);
	SendLogMessageRequest->SetHeader("Content-Type", "application/json");
	SendLogMessageRequest->SetHeader(LOG_MESSAGE_ID_HEADER, FString::FromInt(LogMessage.Id));
	SendLogMessageRequest->SetContentAsString(Content);
	SendLogMessageRequest->OnProcessRequestComplete().BindRaw(this, &ClickstreamNormalHTTPClient::OnSendLogMessageResponseReceived);
	SendLogMessageRequest->ProcessRequest();
	
	return OnSendLogMessageResponse;
}

void ClickstreamNormalHTTPClient::OnSendLogMessageResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	bool bConnectedSuccessfully)
{
	FString LogMessageId = Request->GetHeader(LOG_MESSAGE_ID_HEADER);
	OnSendLogMessageResponse.ExecuteIfBound(Response->GetResponseCode(), FCString::Atoi(*LogMessageId));
}
