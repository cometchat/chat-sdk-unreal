#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatSendMessageAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatSendMessageSuccess, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatSendMessageFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatSendMessageAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatSendMessageSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatSendMessageFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Messaging")
	static UCometChatSendMessageAction* SendMessageAsync(UObject* WorldContextObject, const FString& ReceiverUid, const FString& Text);

	virtual void Activate() override;

private:
	FString ReceiverUid;
	FString Text;
};
