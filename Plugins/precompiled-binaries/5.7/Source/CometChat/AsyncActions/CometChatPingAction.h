#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatPingAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatPingSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatPingFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatPingAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatPingSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatPingFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Connection")
    static UCometChatPingAction* PingAsync(UObject* WorldContextObject);

    virtual void Activate() override;
};
