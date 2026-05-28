#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatLoginWithAuthTokenAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatLoginWithTokenSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLoginWithTokenFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatLoginWithAuthTokenAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatLoginWithTokenSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatLoginWithTokenFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Auth")
	static UCometChatLoginWithAuthTokenAction* LoginWithAuthTokenAsync(UObject* WorldContextObject, const FString& AuthToken);

	virtual void Activate() override;

private:
	FString AuthToken;
};
