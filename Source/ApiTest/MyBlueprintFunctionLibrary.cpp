// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Http.h"


void UMyBlueprintFunctionLibrary::FetchDataFromUrl(const FString& URL, const FOnApiResponse& CallBack)
{
	FHttpModule* HttpModule = &FHttpModule::Get();
	if (!HttpModule) { return; }

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = HttpModule->CreateRequest();
	Request->SetVerb(TEXT("GET"));
	Request->SetURL(URL);
	
	
	Request->OnProcessRequestComplete().BindLambda([CallBack](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		int32 StatusCode = -1;
		FString ResponseString;

		if (bWasSuccessful && Response.IsValid())
		{
			StatusCode = Response->GetResponseCode();
			ResponseString = Response->GetContentAsString();
		}
		else
		{
			ResponseString = TEXT("http request failed or no response");
		}

		CallBack.ExecuteIfBound(StatusCode, ResponseString);
	});

	Request->ProcessRequest();
}




