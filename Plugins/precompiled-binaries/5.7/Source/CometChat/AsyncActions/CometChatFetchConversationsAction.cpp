#include "CometChatFetchConversationsAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatFetchConversationsAction* UCometChatFetchConversationsAction::FetchConversationsAsync(UObject* WorldContextObject, const FCometChatConversationsRequest& Request)
{
    UCometChatFetchConversationsAction* Node = NewObject<UCometChatFetchConversationsAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->Request = Request;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFetchConversationsAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::ConversationsRequestBuilder builder = CometChatConversions::FromUnreal(Request);

    Sdk->fetch_conversations(builder,
        [WeakThis = TWeakObjectPtr<UCometChatFetchConversationsAction>(this)](const std::string& err, const std::vector<chatsdk::Conversation>& conversations)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatConversation> Results;
                for (const auto& c : conversations)
                {
                    Results.Add(CometChatConversions::ToUnreal(c));
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
