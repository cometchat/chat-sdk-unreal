#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatCreateGroupAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatCreateGroupSuccess, const FCometChatGroup&, Group);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatCreateGroupFailure, const FString&, Error);

UCLASS()
class COMETCHAT_API UCometChatCreateGroupAction : public UCometChatAsyncAction
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCometChatCreateGroupSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FOnCometChatCreateGroupFailure OnFailure;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
	static UCometChatCreateGroupAction* CreateGroupAsync(UObject* WorldContextObject, const FString& Name, const TArray<FString>& MemberIds);

	virtual void Activate() override;

private:
	FString GroupName;
	TArray<FString> MemberIds;
};
