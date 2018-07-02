#include "GoodGame.h"
#include "GoodGameHUD.h"
#include "GoodGameStateBase.h"


void AGoodGameStateBase::BroadcastChatMessage_Implementation(const FChatMessage& message)
{
	if (IsNetMode(NM_DedicatedServer))
		return;

	const auto controller = GetWorld()->GetFirstPlayerController();
	auto HUD = Cast<AGoodGameHUD>(controller->GetHUD());
	if (HUD)
		HUD->AddChatMessage(message);
}

void AGoodGameStateBase::AddPlayerState(APlayerState* PlayerState)
{
	// Determine whether it should go in the active or inactive list
	if (!PlayerState->bIsInactive)
	{
		// make sure no duplicates
		if (!PlayerArray.Contains(PlayerState)) {
			PlayerArray.Add(PlayerState);

			if (HasAuthority())
				BroadcastChatMessage(FChatMessage(FString::Printf(TEXT("%s has joined the game."), *PlayerState->PlayerName), EChatMessageType::System));
		}
	}
}

void AGoodGameStateBase::RemovePlayerState(APlayerState* PlayerState)
{
	for (int32 i = 0; i<PlayerArray.Num(); i++)
	{
		if (PlayerArray[i] == PlayerState)
		{
			if (HasAuthority())
				BroadcastChatMessage(FChatMessage(FString::Printf(TEXT("%s has left the game."), *PlayerState->PlayerName), EChatMessageType::System));

			PlayerArray.RemoveAt(i, 1);
			return;
		}
	}
}
