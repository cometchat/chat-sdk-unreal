#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatGetUserAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetUserSuccess, const FCometChatUser&, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetUserFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatGetUserAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetUserSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatGetUserFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Users")
	static UCometChatGetUserAction* GetUserAsync(UObject* WorldContextObject, const FString& Uid);

	virtual void Activate() override;

private:
	FString Uid;
};
