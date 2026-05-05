#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "../CometChatSubsystem.h"
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
};
