#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "../CometChatSubsystem.h"
#include <string>
#include "CometChatAsyncAction.generated.h"

namespace chatsdk { class ChatSDK; }

UCLASS(Abstract)
class COMETCHAT_API UCometChatAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<UObject> CachedWorldContextObject;

	UCometChatSubsystem* GetSubsystem() const
	{
		if (!CachedWorldContextObject) return nullptr;
		UWorld* World = CachedWorldContextObject->GetWorld();
		if (!World) return nullptr;
		UGameInstance* GI = World->GetGameInstance();
		if (!GI) return nullptr;
		return GI->GetSubsystem<UCometChatSubsystem>();
	}

	chatsdk::ChatSDK* GetSdk() const
	{
		UCometChatSubsystem* Sub = GetSubsystem();
		return Sub ? Sub->GetSdk() : nullptr;
	}

	static FCometChatError MakeError(const FString& Message, const FString& Code = TEXT("ERR_UNKNOWN"), const FString& Details = TEXT(""))
	{
		FCometChatError Err;
		Err.Code = Code;
		Err.Message = Message;
		Err.Details = Details;
		return Err;
	}

	static FCometChatError MakeError(const std::string& err)
	{
		FCometChatError Err;
		Err.Message = UTF8_TO_TCHAR(err.c_str());
		// Try to extract code from error string (format: "CODE: message" or just "message")
		FString Full = Err.Message;
		int32 ColonIdx;
		if (Full.FindChar(TEXT(':'), ColonIdx) && ColonIdx < 30)
		{
			Err.Code = Full.Left(ColonIdx).TrimStartAndEnd();
			Err.Message = Full.Mid(ColonIdx + 1).TrimStartAndEnd();
		}
		else
		{
			Err.Code = TEXT("ERR_UNKNOWN");
		}
		return Err;
	}
};
