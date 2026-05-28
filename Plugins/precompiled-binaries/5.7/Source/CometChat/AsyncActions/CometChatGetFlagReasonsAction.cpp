#include "CometChatGetFlagReasonsAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatGetFlagReasonsAction* UCometChatGetFlagReasonsAction::GetFlagReasonsAsync(UObject* WorldContextObject)
{
    UCometChatGetFlagReasonsAction* Node = NewObject<UCometChatGetFlagReasonsAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatGetFlagReasonsAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    Sdk->get_flag_reasons(
        [WeakThis = TWeakObjectPtr<UCometChatGetFlagReasonsAction>(this)](const std::string& err, const std::vector<chatsdk::FlagReason>& reasons)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatFlagReason> Results;
                for (const auto& r : reasons)
                {
                    Results.Add(CometChatConversions::ToUnreal(r));
                }
                Self->OnSuccess.Broadcast(Results);
            }
            else
            {
                Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
            }
            Self->SetReadyToDestroy();
        }
    );
}
