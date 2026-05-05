#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatLeaveGroupAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatLeaveGroupSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLeaveGroupFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatLeaveGroupAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatLeaveGroupSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatLeaveGroupFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
	static UCometChatLeaveGroupAction* LeaveGroupAsync(UObject* WorldContextObject, const FString& Guid);

	virtual void Activate() override;

private:
	FString Guid;
};
