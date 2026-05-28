#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatDisconnectAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCometChatDisconnectSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatDisconnectFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatDisconnectAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatDisconnectSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatDisconnectFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Connection")
    static UCometChatDisconnectAction* DisconnectAsync(UObject* WorldContextObject);

    virtual void Activate() override;
};
