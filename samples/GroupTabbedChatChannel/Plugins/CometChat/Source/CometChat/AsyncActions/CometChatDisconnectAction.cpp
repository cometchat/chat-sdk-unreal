#include "CometChatDisconnectAction.h"
#include "chatsdk/cometchat.h"

UCometChatDisconnectAction* UCometChatDisconnectAction::DisconnectAsync(UObject* WorldContextObject)
{
    UCometChatDisconnectAction* Node = NewObject<UCometChatDisconnectAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatDisconnectAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    Sdk->disconnect(
        [WeakThis = TWeakObjectPtr<UCometChatDisconnectAction>(this)](const std::string& err)
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
