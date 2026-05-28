#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatJoinGroupAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatJoinGroupSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatJoinGroupFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatJoinGroupAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatJoinGroupSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatJoinGroupFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
	static UCometChatJoinGroupAction* JoinGroupAsync(UObject* WorldContextObject, const FString& Guid);

	virtual void Activate() override;

private:
	FString Guid;
};
