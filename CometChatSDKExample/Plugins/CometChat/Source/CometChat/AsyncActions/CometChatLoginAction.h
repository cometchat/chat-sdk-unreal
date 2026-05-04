#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatLoginAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatLoginSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLoginFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatLoginAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatLoginSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatLoginFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Auth")
	static UCometChatLoginAction* LoginAsync(UObject* WorldContextObject, const FString& Uid, const FString& AuthKey);

	virtual void Activate() override;

private:
	FString Uid;
	FString AuthKey;
};
