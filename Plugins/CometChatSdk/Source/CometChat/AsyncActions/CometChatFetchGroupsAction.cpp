#include "CometChatFetchGroupsAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatFetchGroupsAction* UCometChatFetchGroupsAction::FetchGroupsAsync(UObject* WorldContextObject, const FCometChatGroupsRequest& Request)
{
    UCometChatFetchGroupsAction* Node = NewObject<UCometChatFetchGroupsAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->Request = Request;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFetchGroupsAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::GroupsRequestBuilder builder = CometChatConversions::FromUnreal(Request);

    Sdk->fetch_groups(builder,
        [WeakThis = TWeakObjectPtr<UCometChatFetchGroupsAction>(this)](const std::string& err, const std::vector<chatsdk::Group>& groups)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatGroup> Results;
                for (const auto& g : groups)
                {
                    Results.Add(CometChatConversions::ToUnreal(g));
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
