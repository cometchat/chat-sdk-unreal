#include "CometChatGetUserAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatGetUserAction* UCometChatGetUserAction::GetUserAsync(UObject* WorldContextObject, const FString& Uid)
{
	UCometChatGetUserAction* Node = NewObject<UCometChatGetUserAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Uid = Uid;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatGetUserAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	Sdk->get_user(
		TCHAR_TO_UTF8(*Uid),
		[WeakThis = TWeakObjectPtr<UCometChatGetUserAction>(this)](const std::string& err, const chatsdk::User& user)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->OnSuccess.Broadcast(CometChatConversions::ToUnreal(user));
			}
			else
			{
				Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
			}
			Self->SetReadyToDestroy();
		}
	);
}
