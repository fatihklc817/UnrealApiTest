// Fill out your copyright notice in the Description page of Project Settings.


#include "APIGameInstanceSubsystem.h"

#include "HttpModule.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Interfaces/IHttpResponse.h"

void UAPIGameInstanceSubsystem::GetRequestImages()
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();

	Request->SetVerb(TEXT("GET"));
	Request->SetURL(TEXT("https://lcw.berkadiguzel.com.tr/api/RequestImages"));

	Request->OnProcessRequestComplete().BindUObject(this,&UAPIGameInstanceSubsystem::OnResponseReceived);

	Request->ProcessRequest();
}

void UAPIGameInstanceSubsystem::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful && !Response.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("response failed "));
		return;
	}

	FString ResponseStr = Response->GetContentAsString();
	int32 ResponseCode = Response->GetResponseCode();

	FString URL = Request->GetURL();
	FString Time = FDateTime::Now().ToString(TEXT("%Y-%m-%d %H:%M:%S"));

	FString OutputString = FString::Printf(TEXT("[%s] URL: %s | Code:%d | Body:%s \n"),*Time, *URL, ResponseCode, *ResponseStr);

	//UE_LOG(LogTemp, Warning, TEXT("Response: %s and Response code : %d"), *ResponseStr, ResponseCode);

	FString FilePath = FPaths::ProjectDir() / TEXT("SavedJsons/Images.txt");

	//----------------------------------------------------------------------save to pc as a text file / if it already has override 
	//bool bSaved = FFileHelper::SaveStringToFile(ResponseStr, *FilePath);

	//-----------------------------------------------------------------------add the string to already created txt file 
	bool bSaved = FFileHelper::SaveStringToFile(OutputString, *FilePath, FFileHelper::EEncodingOptions::AutoDetect,&IFileManager::Get(),FILEWRITE_Append);

	
	
	
	TArray<FRequestedImage> OutImages;

	TArray<TSharedPtr<FJsonValue>> JsonArray;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

	if (FJsonSerializer::Deserialize(Reader,JsonArray))
	{
		for (const TSharedPtr<FJsonValue>& JsonValue : JsonArray)
		{
			TSharedPtr<FJsonObject> JsonObject = JsonValue->AsObject();
			if (!JsonObject)
			{
				continue;
			}

			FRequestedImage Image;
			Image.DisplayOrder = JsonObject->GetStringField(TEXT("display_order"));
			Image.ImagePath = JsonObject->GetStringField(TEXT("image_path"));

			OutImages.Add(Image);

			DownloadImage(Image.ImagePath, Image.DisplayOrder);
			
		}
		
	}

	OnImagesReceived.Broadcast(OutImages);
	
}


void UAPIGameInstanceSubsystem::DownloadImage(const FString& ImageUrl, const FString& DisplayOrder)
{
	TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
	Request->SetVerb(TEXT("GET"));
	Request->SetURL(ImageUrl);

	Request->OnProcessRequestComplete().BindUObject(this,&UAPIGameInstanceSubsystem::OnImageDownloaded, DisplayOrder);
	Request->ProcessRequest();
	
}


void UAPIGameInstanceSubsystem::OnImageDownloaded(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString DisplayOrder)
{
	if (!bWasSuccessful || !Response.IsValid() || Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Warning, TEXT("Image download failed: %s"), *DisplayOrder);
		return;
	}

	const TArray<uint8>& ImageData = Response->GetContent();

	//image wrapper ile decode
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);

	if (ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		TArray<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
		{
			UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
			if (Texture)
			{
				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData,UncompressedBGRA.GetData(),UncompressedBGRA.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();

				OnImageDownloadedEvent.Broadcast(DisplayOrder, Texture);
			}
		}
	}
	
}
