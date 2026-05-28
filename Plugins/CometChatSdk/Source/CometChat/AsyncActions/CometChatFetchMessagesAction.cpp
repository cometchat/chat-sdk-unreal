#include "CometChatFetchMessagesAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

// ============================================================
// FetchPreviousMessages
// ============================================================

UCometChatFetchPreviousMessagesAction* UCometChatFetchPreviousMessagesAction::FetchPreviousMessagesAsync(UObject* WorldContextObject, const FCometChatMessagesRequest& Request)
{
    UCometChatFetchPreviousMessagesAction* Node = NewObject<UCometChatFetchPreviousMessagesAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->Request = Request;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFetchPreviousMessagesAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::MessagesRequestBuilder builder = CometChatConversions::FromUnreal(Request);

    Sdk->fetch_previous_messages(builder,
        [WeakThis = TWeakObjectPtr<UCometChatFetchPreviousMessagesAction>(this)](const std::string& err, const std::vector<chatsdk::Message>& messages)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatMessage> Results;
                for (const auto& m : messages)
                {
                    Results.Add(CometChatConversions::ToUnreal(m));
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

// ============================================================
// FetchNextMessages
// ============================================================

UCometChatFetchNextMessagesAction* UCometChatFetchNextMessagesAction::FetchNextMessagesAsync(UObject* WorldContextObject, const FCometChatMessagesRequest& Request)
{
    UCometChatFetchNextMessagesAction* Node = NewObject<UCometChatFetchNextMessagesAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->Request = Request;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFetchNextMessagesAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::MessagesRequestBuilder builder = CometChatConversions::FromUnreal(Request);

    Sdk->fetch_next_messages(builder,
        [WeakThis = TWeakObjectPtr<UCometChatFetchNextMessagesAction>(this)](const std::string& err, const std::vector<chatsdk::Message>& messages)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatMessage> Results;
                for (const auto& m : messages)
                {
                    Results.Add(CometChatConversions::ToUnreal(m));
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
