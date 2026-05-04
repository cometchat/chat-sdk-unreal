#include "CometChatGetGroupMessagesAction.h"
#include "../CometChatConversions.h"
#include "chatsdk/cometchat.h"

UCometChatGetGroupMessagesAction* UCometChatGetGroupMessagesAction::GetGroupMessagesAsync(UObject* WorldContextObject, const FString& Guid, int32 Limit, int32 BeforeMessageId)
{
	UCometChatGetGroupMessagesAction* Node = NewObject<UCometChatGetGroupMessagesAction>();
	Node->CachedWorldContextObject = WorldContextObject;
	Node->Guid = Guid;
	Node->Limit = Limit;
	Node->BeforeMessageId = BeforeMessageId;
	Node->RegisterWithGameInstance(WorldContextObject);
	return Node;
}

void UCometChatGetGroupMessagesAction::Activate()
{
	chatsdk::ChatSDK* Sdk = GetSdk();
	if (!Sdk)
	{
		OnFailure.Broadcast(TEXT("SDK not configured."));
		SetReadyToDestroy();
		return;
	}

	Sdk->get_group_messages(
		TCHAR_TO_UTF8(*Guid),
		Limit,
		static_cast<int64_t>(BeforeMessageId),
		[WeakThis = TWeakObjectPtr<UCometChatGetGroupMessagesAction>(this)](const std::string& err, const chatsdk::PaginatedMessages& result)
		{
			if (!WeakThis.IsValid()) return;
			auto* Self = WeakThis.Get();
			if (err.empty())
			{
				TArray<FCometChatMessage> Out;
				Out.Reserve(result.messages.size());
				for (const auto& m : result.messages)
				{
					Out.Add(CometChatConversions::ToUnreal(m));
				}

				FCometChatPagination PaginationOut;
				PaginationOut.Total = result.pagination.total;
				PaginationOut.Count = result.pagination.count;
				PaginationOut.PerPage = result.pagination.per_page;
				PaginationOut.CurrentPage = result.pagination.current_page;
				PaginationOut.TotalPages = result.pagination.total_pages;
				PaginationOut.HasMore = result.pagination.has_more;
				PaginationOut.NextCursor = static_cast<int32>(result.pagination.next_cursor);

				Self->OnSuccess.Broadcast(Out, PaginationOut);
			}
			else
			{
				Self->OnFailure.Broadcast(UTF8_TO_TCHAR(err.c_str()));
			}
			Self->SetReadyToDestroy();
		}
	);
}
