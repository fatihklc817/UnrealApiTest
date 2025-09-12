// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "APIGameInstanceSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FRequestedImage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString DisplayOrder;

	UPROPERTY(BlueprintReadOnly)
	FString ImagePath;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImagesReceived, const TArray<FRequestedImage>&, Images);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnImageDownloaded, FString, DisplayOrder, UTexture2D*, Texture);

/**
 * 
 */
UCLASS()
class APITEST_API UAPIGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable, Category="Request Images")
	FOnImagesReceived OnImagesReceived;

	UPROPERTY(BlueprintAssignable)
	FOnImageDownloaded OnImageDownloadedEvent;

	UFUNCTION(BlueprintCallable, Category="API|HTTP|LCW")
	void GetRequestImages();

	
private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	void OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString DisplayOrder);

	void DownloadImage(const FString& ImageUrl, const FString& DisplayOrder);
	
};
