#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatLogoutAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatLogoutSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatLogoutFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatLogoutAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatLogoutSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatLogoutFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Auth")
	static UCometChatLogoutAction* LogoutAsync(UObject* WorldContextObject);

	virtual void Activate() override;
};
