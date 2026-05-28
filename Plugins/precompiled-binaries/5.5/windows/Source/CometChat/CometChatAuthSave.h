#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CometChatAuthSave.generated.h"

/**
 * Persists the CometChat auth token between game sessions.
 * Saved via UGameplayStatics::SaveGameToSlot — works on all platforms.
 */
UCLASS()
class COMETCHAT_API UCometChatAuthSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString AuthToken;

	UPROPERTY()
	FString UserId;
};
