#include "CometChatCreateGroupAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatCreateGroupAction* UCometChatCreateGroupAction::CreateGroupAsync(UObject* WorldContextObject, const FString& Name, const TArray<FString>& MemberIds)
{
	UCometChatCreateGroupAction* Node = NewObject<UCometChatCreateGroupAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->GroupName = Name;
	Node->MemberIds = MemberIds;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatCreateGroupAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(MakeError(TEXT("SDK not configured."), TEXT("ERR_SDK")));
		SetReadyToDestroy();
		return;
	}

	std::vector<std::string> ids;
	ids.reserve(MemberIds.Num());
	for (const FString& Id : MemberIds)
	{
		ids.push_back(TCHAR_TO_UTF8(*Id));
	}

	Sdk->create_group(
		TCHAR_TO_UTF8(*GroupName),
		ids,
		[WeakThis = TWeakObjectPtr<UCometChatCreateGroupAction>(this)](const std::string& err, const chatsdk::Group& group)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				Self->OnSuccess.Broadcast(CometChatConversions::ToUnreal(group));
			}
			else
			{
				Self->OnFailure.Broadcast(UCometChatAsyncAction::MakeError(err));
			}
			Self->SetReadyToDestroy();
		}
	);
}
