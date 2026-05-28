#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatFlagMessageAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatFlagMessageSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFlagMessageFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatFlagMessageAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFlagMessageSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFlagMessageFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Moderation")
    static UCometChatFlagMessageAction* FlagMessageAsync(UObject* WorldContextObject, int64 MessageId, const FCometChatFlagDetail& FlagDetail);

    virtual void Activate() override;

private:
    int64 MessageId = 0;
    FCometChatFlagDetail FlagDetail;
};
