#include "ClickstreamProtocol.h"

#include "Common/ClickstreamSettings.h"
#include "Common/ClickstreamLogging.h"
#include "Interfaces/IPluginManager.h"
#include "Kismet/GameplayStatics.h"

FClickstreamProtocol* FClickstreamProtocol::Singleton = nullptr;

bool FClickstreamProtocol::Init(const UClickstreamSettings* ClickstreamSettings)
{
	if (ClickstreamSettings == nullptr)
	{
		UE_LOG(LogEstherClickstream, Warning, TEXT("Failed to initialize ClickstreamProtocol"));
		return false;
	}
	
	InvokeUrl = ClickstreamSettings->EndpointUrl;
	AppId = ClickstreamSettings->AppId;
	AppVersion = ClickstreamSettings->AppVersion;
	Compression = ClickstreamSettings->Compression;
	
	Platform = UGameplayStatics::GetPlatformName();

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("EstherClickstream"));
	if (Plugin.IsValid())
	{
		SdkVersion = Plugin->GetDescriptor().VersionName;
	}
	
	return true;
}

FClickstreamProtocol& FClickstreamProtocol::Get()
{
	if (Singleton == nullptr)
	{
		Singleton = new FClickstreamProtocol;
		const UClickstreamSettings* ClickstreamSettings = GetDefault<UClickstreamSettings>();
		Singleton->Init(ClickstreamSettings);
	}
	check(Singleton != nullptr);
	return *Singleton;
}

FString FClickstreamProtocol::GetRequestUrl()
{
	return FString::Printf(TEXT("%s?platform=%s&sdkVersion=%s&appId=%s&appVersion=%s&compression=%s&hashCode=%lu"),
		*InvokeUrl, *Platform, *SdkVersion, *AppId, *AppVersion, *Compression, HashCode);
}

FString FClickstreamProtocol::EncodeLogMessage(const FString& LogMessage)
{
	// create a JSON array
	TArray<TSharedPtr<FJsonValue>> ArrayValues;
	// create the first JSON object
	TSharedPtr<FJsonObject> LogMessageJsonObject = MakeShareable(new FJsonObject);
	LogMessageJsonObject->SetNumberField("sequence_id", 1);
	LogMessageJsonObject->SetStringField("hashCode", "80452b0");
	LogMessageJsonObject->SetStringField("unique_id", "c84ad28d-16a8-4af4-a331-f34cdc7a7a18");
	LogMessageJsonObject->SetStringField("event_type", "PasswordReset");
	LogMessageJsonObject->SetStringField("event_id", "460daa08-0717-4385-8f2e-acb5bd019ee7");
	LogMessageJsonObject->SetNumberField("timestamp", 1667877566697);
	LogMessageJsonObject->SetStringField("device_id", "f24bec657ea8eff7");
	LogMessageJsonObject->SetStringField("platform", "ANDROID");
	LogMessageJsonObject->SetStringField("os_version", "10");
	LogMessageJsonObject->SetStringField("make", "HUAWEI");
	LogMessageJsonObject->SetStringField("brand", "HUAWEI");
	LogMessageJsonObject->SetStringField("model", "TAS-AN00");
	LogMessageJsonObject->SetStringField("locale", "zh_CN_#Hans");
	LogMessageJsonObject->SetStringField("carrier", "CDMA");
	LogMessageJsonObject->SetStringField("network_type", "Mobile");
	LogMessageJsonObject->SetNumberField("screen_height", 2259);
	LogMessageJsonObject->SetNumberField("screen_width", 1080);
	LogMessageJsonObject->SetNumberField("zone_offset", 28800000);
	LogMessageJsonObject->SetStringField("system_language", "zh");
	LogMessageJsonObject->SetStringField("country_code", "CN");
	LogMessageJsonObject->SetStringField("sdk_version", "0.2.0");
	LogMessageJsonObject->SetStringField("sdk_name", "aws-solution-clickstream-sdk");
	LogMessageJsonObject->SetStringField("app_version", "1.0");
	LogMessageJsonObject->SetStringField("app_package_name", "com.xiaowei.demoapplication");
	LogMessageJsonObject->SetStringField("app_title", "DemoApplication");
	LogMessageJsonObject->SetStringField("app_id", "uba-app");
	LogMessageJsonObject->SetStringField("log_message", LogMessage);
	
	// create JSON user object
	TSharedPtr<FJsonObject> User = MakeShareable(new FJsonObject);
	User->SetStringField("_user_id", "312121");
	User->SetBoolField("_user_name", true);
	User->SetNumberField("_user_age", 20);
	LogMessageJsonObject->SetObjectField("user", User);

	// create JSON Attributes object
	TSharedPtr<FJsonObject> Attributes = MakeShareable(new FJsonObject);
	Attributes->SetStringField("Channel", "SMS");
	Attributes->SetBoolField("Successful", true);
	Attributes->SetNumberField("Price", 120.1);
	Attributes->SetNumberField("ProcessDuration", 791);
	Attributes->SetStringField("_error_name_invalid", "invalid name:1_add_cart");
	Attributes->SetStringField("_error_name_exceed", "exceed limit,attribute name:add_cart999");
	Attributes->SetStringField("_session_id", "dc7a7a18-20221108-031926703");
	Attributes->SetNumberField("_session_start_timestamp", 1667877566703);
	Attributes->SetNumberField("_session_duration", 391809);
	Attributes->SetBoolField("_is_first_time", true);
	Attributes->SetBoolField("_is_first_day", true);
	LogMessageJsonObject->SetObjectField("attributes", Attributes);
	
	ArrayValues.Add(MakeShareable(new FJsonValueObject(LogMessageJsonObject)));
	
	// serialize JSON object to string
	FString JsonString;
	TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(ArrayValues, JsonWriter);

	HashCode = TextKeyUtil::HashString(JsonString);

	return JsonString;
}
