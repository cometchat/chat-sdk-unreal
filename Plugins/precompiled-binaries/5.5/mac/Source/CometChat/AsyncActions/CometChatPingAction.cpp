#include "CometChatPingAction.h"
#include "chatsdk/cometchat.h"

UCometChatPingAction* UCometChatPingAction::PingAsync(UObject* WorldContextObject)
{
    UCometChatPingAction* Node = NewObject<UCometChatPingAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatPingAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    Sdk->ping(
        [WeakThis = TWeakObjectPtr<UCometChatPingAction>(this)](const std::string& err)
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
