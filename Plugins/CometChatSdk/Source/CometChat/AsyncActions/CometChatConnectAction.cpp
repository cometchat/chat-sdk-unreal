#include "CometChatConnectAction.h"
#include "chatsdk/cometchat.h"

UCometChatConnectAction* UCometChatConnectAction::ConnectAsync(UObject* WorldContextObject)
{
    UCometChatConnectAction* Node = NewObject<UCometChatConnectAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatConnectAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    Sdk->connect(
        [WeakThis = TWeakObjectPtr<UCometChatConnectAction>(this)](const std::string& err)
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
