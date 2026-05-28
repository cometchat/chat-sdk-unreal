#include "CometChatJoinGroupAction.h"
#include "chatsdk/cometchat.h"

UCometChatJoinGroupAction* UCometChatJoinGroupAction::JoinGroupAsync(UObject* WorldContextObject, const FString& Guid)
{
	UCometChatJoinGroupAction* Node = NewObject<UCometChatJoinGroupAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Guid = Guid;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatJoinGroupAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	Sdk->join_group(
		TCHAR_TO_UTF8(*Guid),
		[WeakThis = TWeakObjectPtr<UCometChatJoinGroupAction>(this)](const std::string& err)
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
