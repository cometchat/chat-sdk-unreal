#include "CometChatLeaveGroupAction.h"
#include "chatsdk/cometchat.h"

UCometChatLeaveGroupAction* UCometChatLeaveGroupAction::LeaveGroupAsync(UObject* WorldContextObject, const FString& Guid)
{
	UCometChatLeaveGroupAction* Node = NewObject<UCometChatLeaveGroupAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Guid = Guid;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatLeaveGroupAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(TEXT("SDK not configured."));
		SetReadyToDestroy();
		return;
	}

	Sdk->leave_group(
		TCHAR_TO_UTF8(*Guid),
		[WeakThis = TWeakObjectPtr<UCometChatLeaveGroupAction>(this)](const std::string& err)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->OnSuccess.Broadcast();
			}
			else
			{
				Self->OnFailure.Broadcast(UTF8_TO_TCHAR(err.c_str()));
			}
			Self->SetReadyToDestroy();
		}
	);
}
