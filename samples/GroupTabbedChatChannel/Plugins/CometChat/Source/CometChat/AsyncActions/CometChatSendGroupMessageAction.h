#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatSendGroupMessageAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatSendGroupMessageSuccess, const FCometChatMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatSendGroupMessageFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatSendGroupMessageAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatSendGroupMessageSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatSendGroupMessageFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
	static UCometChatSendGroupMessageAction* SendGroupMessageAsync(UObject* WorldContextObject, const FString& Guid, const FString& Text);

	virtual void Activate() override;

private:
	FString Guid;
	FString Text;
};
