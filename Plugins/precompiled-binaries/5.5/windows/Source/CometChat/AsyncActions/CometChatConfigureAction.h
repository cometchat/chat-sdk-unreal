#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatConfigureAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatConfigureSuccess, const FCometChatUser&, LoggedInUser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatConfigureFailure, const FCometChatError&, Error);

/**
 * Async action that configures the CometChat SDK and automatically
 * restores a previous session if one exists.
 *
 * On success:
 *   - If a saved session was restored, LoggedInUser contains the user details.
 *   - If no saved session exists, LoggedInUser will have an empty Uid
 *     (SDK is configured but user needs to login).
 *
 * On failure:
 *   - SDK configuration failed.
 */
UCLASS()
class COMETCHAT_API UCometChatConfigureAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatConfigureSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatConfigureFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Config")
	static UCometChatConfigureAction* CometChatConfigureAsync(UObject* WorldContextObject, const FString& AppId, const FString& Region);

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Config")
	static UCometChatConfigureAction* CometChatConfigureWithSettingsAsync(UObject* WorldContextObject, const FString& AppId, const FCometChatAppSettings& Settings);

	virtual void Activate() override;

private:
	FString AppId;
	FCometChatAppSettings Settings;
};
