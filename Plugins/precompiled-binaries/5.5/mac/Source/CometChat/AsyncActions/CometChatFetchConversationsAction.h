#pragma once

#include "CometChatAsyncAction.h"
#include "CometChatFetchConversationsAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchConversationsSuccess, const TArray<FCometChatConversation>&, Conversations);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCometChatFetchConversationsFailure, const FCometChatError&, Error);

UCLASS()
class COMETCHAT_API UCometChatFetchConversationsAction : public UCometChatAsyncAction
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchConversationsSuccess OnSuccess;

    UPROPERTY(BlueprintAssignable)
    FOnCometChatFetchConversationsFailure OnFailure;

    UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"), Category="CometChat|Conversations")
    static UCometChatFetchConversationsAction* FetchConversationsAsync(UObject* WorldContextObject, const FCometChatConversationsRequest& Request);

    virtual void Activate() override;

private:
    FCometChatConversationsRequest Request;
};
