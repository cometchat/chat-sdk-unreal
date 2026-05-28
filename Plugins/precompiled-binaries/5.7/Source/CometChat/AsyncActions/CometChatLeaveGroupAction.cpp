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
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
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
				Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
			}
			Self->SetReadyToDestroy();
		}
	);
}
