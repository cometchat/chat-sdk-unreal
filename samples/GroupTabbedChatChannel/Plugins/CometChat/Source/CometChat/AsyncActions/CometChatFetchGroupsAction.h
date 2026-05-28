#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatFetchGroupsAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchGroupsSuccess, const TArray<FCometChatGroup>&, Groups);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchGroupsFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatFetchGroupsAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchGroupsSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchGroupsFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Groups")
    static UCometChatFetchGroupsAction* FetchGroupsAsync(UObject* WorldContextObject, const FCometChatGroupsRequest& Request);

    virtual void Activate() override;

private:
    FCometChatGroupsRequest Request;
};
