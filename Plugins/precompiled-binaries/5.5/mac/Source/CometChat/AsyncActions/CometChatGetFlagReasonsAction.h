#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatGetFlagReasonsAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetFlagReasonsSuccess, const TArray<FCometChatFlagReason>&, FlagReasons);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatGetFlagReasonsFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatGetFlagReasonsAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatGetFlagReasonsSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatGetFlagReasonsFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Moderation")
    static UCometChatGetFlagReasonsAction* GetFlagReasonsAsync(UObject* WorldContextObject);

    virtual void Activate() override;
};
