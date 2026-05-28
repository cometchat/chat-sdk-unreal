#include "CometChatFlagMessageAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatFlagMessageAction* UCometChatFlagMessageAction::FlagMessageAsync(UObject* WorldContextObject, int64 MessageId, const FCometChatFlagDetail& FlagDetail)
{
    UCometChatFlagMessageAction* Node = NewObject<UCometChatFlagMessageAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->MessageId = MessageId;
    Node->FlagDetail = FlagDetail;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFlagMessageAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::FlagDetail detail = CometChatConversions::FromUnreal(FlagDetail);

    Sdk->flag_message(
        MessageId,
        detail,
        [WeakThis = TWeakObjectPtr<UCometChatFlagMessageAction>(this)](const std::string& err)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                Self->OnSuccess.Broadcast();
            }
            else
            {
                Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
            }
            Self->SetReadyToDestroy();
        }
    );
}
