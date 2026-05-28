#include "CometChatFetchUsersAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatFetchUsersAction* UCometChatFetchUsersAction::FetchUsersAsync(UObject* WorldContextObject, const FCometChatUsersRequest& Request)
{
    UCometChatFetchUsersAction* Node = NewObject<UCometChatFetchUsersAction>();
    Node->CachedWorldContextObject = WorldContextObject;
    Node->Request = Request;
    Node->RegisterWithGameInstance(WorldContextObject);
    return Node;
}

void UCometChatFetchUsersAction::Activate()
{
    chatsdk::ChatSDK* Sdk = GetSdk();
    if (!Sdk)
    {
        OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
        SetReadyToDestroy();
        return;
    }

    chatsdk::UsersRequestBuilder builder = CometChatConversions::FromUnreal(Request);

    Sdk->fetch_users(builder,
        [WeakThis = TWeakObjectPtr<UCometChatFetchUsersAction>(this)](const std::string& err, const std::vector<chatsdk::User>& users)
        {
            if (!WeakThis.IsValid()) return;
            auto* Self = WeakThis.Get();
            if (err.empty())
            {
                TArray<FCometChatUser> Results;
                for (const auto& u : users)
                {
                    Results.Add(CometChatConversions::ToUnreal(u));
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
