// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"



DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnApiResponse, int32, StatusCode, const FString&, Response);



/**
 * 
 */
UCLASS()
class APITEST_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="API|HTTP")
	static void FetchDataFromUrl(const FString& URL, const FOnApiResponse& CallBack);
	

};
