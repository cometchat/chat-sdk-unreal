#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatFetchMessagesAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchMessagesSuccess, const TArray<FCometChatMessage>&, Messages);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchMessagesFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatFetchPreviousMessagesAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchMessagesSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchMessagesFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Messaging")
    static UCometChatFetchPreviousMessagesAction* FetchPreviousMessagesAsync(UObject* WorldContextObject, const FCometChatMessagesRequest& Request);

    virtual void Activate() override;

private:
    FCometChatMessagesRequest Request;
};

UCLASS()
class COMETCHAT_API UCometChatFetchNextMessagesAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchMessagesSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchMessagesFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Messaging")
    static UCometChatFetchNextMessagesAction* FetchNextMessagesAsync(UObject* WorldContextObject, const FCometChatMessagesRequest& Request);

    virtual void Activate() override;

private:
    FCometChatMessagesRequest Request;
};
