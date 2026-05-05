#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatGetMessagesAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetMessagesSuccess, const TArray<FCometChatMessage>&, Messages);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetMessagesFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatGetMessagesAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetMessagesSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetMessagesFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Messages")
	static UCometChatGetMessagesAction* GetMessagesAsync(UObject* WorldContextObject, const FString& Uid, int32 Limit);

	virtual void Activate() override;

private:
	FString Uid;
	int32 Limit = 50;
};
