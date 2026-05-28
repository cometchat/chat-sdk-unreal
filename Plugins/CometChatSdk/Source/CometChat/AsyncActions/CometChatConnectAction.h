#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatConnectAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatConnectSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatConnectFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatConnectAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatConnectSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatConnectFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Connection")
    static UCometChatConnectAction* ConnectAsync(UObject* WorldContextObject);

    virtual void Activate() override;
};
