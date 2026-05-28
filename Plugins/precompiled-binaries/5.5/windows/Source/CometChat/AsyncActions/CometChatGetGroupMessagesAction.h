#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatGetGroupMessagesAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCometChatGetGroupMessagesSuccess, const TArray<FCometChatMessage>&, Messages, const FCometChatPagination&, Pagination);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetGroupMessagesFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatGetGroupMessagesAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetGroupMessagesSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetGroupMessagesFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
	static UCometChatGetGroupMessagesAction* GetGroupMessagesAsync(UObject* WorldContextObject, const FString& Guid, int32 Limit, int32 BeforeMessageId = 0);

	virtual void Activate() override;

private:
	FString Guid;
	int32 Limit = 50;
	int32 BeforeMessageId = 0;
};
