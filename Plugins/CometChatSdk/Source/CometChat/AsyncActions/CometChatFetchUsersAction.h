#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatFetchUsersAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchUsersSuccess, const TArray<FCometChatUser>&, Users);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchUsersFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatFetchUsersAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchUsersSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchUsersFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Users")
    static UCometChatFetchUsersAction* FetchUsersAsync(UObject* WorldContextObject, const FCometChatUsersRequest& Request);

    virtual void Activate() override;

private:
    FCometChatUsersRequest Request;
};
