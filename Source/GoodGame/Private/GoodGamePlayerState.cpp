#include "GoodGame.h"
#include "GoodGamePlayerState.h"


void AGoodGamePlayerState::ServerToClientChatMessage_Implementation(const FChatMessage& message)
{
	if (IsNetMode(NM_DedicatedServer))
		return;

	const auto controller = GetWorld()->GetFirstPlayerController();
	auto HUD = Cast<AGoodGameHUD>(controller->GetHUD());
	if (HUD)
		HUD->AddChatMessage(message);
}

void AGoodGamePlayerState::ClientToServerChatMessage_Implementation(const FChatMessage& message)
{
	if (GetWorld()->RealTimeSeconds < NextChat)
		return;

	static const float timeout_normal = (float)CHAT_TIMEOUT / 1000.f;
	static const float timeout_throttled = (float)CHAT_TIMEOUT / 250.f;

	if (LastChatMessage.Equals(message.Message, ESearchCase::IgnoreCase))
		NextChat = GetWorld()->RealTimeSeconds + timeout_throttled;
	else
		NextChat = GetWorld()->RealTimeSeconds + timeout_normal;

	FChatMessage newMessage = message;

	//  Get rid of real \n and \r and such.
	newMessage.Message = message.Message.ReplaceCharWithEscapedChar();
	const auto len = newMessage.Message.Len();
	if (len > FChatMessage::MAX_CHAT_LENGTH || len == 0)
		return;

	newMessage.Actor = this;

	const EChatMessageType type = (EChatMessageType)message.TypeID;

	// Send this message to proper recipients...
	if (type == EChatMessageType::Private) {
		AGoodGamePlayerState* otherState = Cast<AGoodGamePlayerState>(message.Actor);
		if (otherState)
			otherState->ServerToClientChatMessage(newMessage);
		else
			ServerToClientChatMessage(FChatMessage("Player is no longer connected.", EChatMessageType::System));
	}
	else if (type == EChatMessageType::Area) {
		APlayerController* controller = nullptr;
		APawn* localPawn = nullptr;

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
			controller = (*It).Get();

			if (controller && controller->PlayerState == this) {
				localPawn = controller->GetPawn();
				break;
			}
		}

		static const float area_chat_distance_sqr = FMath::Pow((float)AREA_CHAT_DISTANCE, 2.f);

		if (localPawn) {
			FVector localLocation = localPawn->GetActorLocation();

			APawn* pawn = nullptr;
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It) {
				controller = (*It).Get();

				if (controller) {
					pawn = controller->GetPawn();
					
					if (pawn && controller->PlayerState && FVector::DistSquared(pawn->GetActorLocation(), localLocation) < area_chat_distance_sqr)
						((AGoodGamePlayerState*)controller->PlayerState)->ServerToClientChatMessage(message);
				}
			}
		}
	}
	else if (type == EChatMessageType::Global) {
		BroadcastChatMessage(newMessage);
	}
}

void AGoodGamePlayerState::SendDamageNumber(AActor* actor, const float& amount, const bool& isBackHit, const bool& isHeal)
{
	static const float max = UINT16_MAX - 32768 - 16384;

	uint16 compressedDamage = (uint16)(amount / 200.f * max);
	if (isBackHit)
		compressedDamage |= 32768;
	if (isHeal)
		compressedDamage |= 16384;

	ServerToClientSendDamageNumber(actor, compressedDamage);
}

void AGoodGamePlayerState::ServerToClientSendDamageNumber_Implementation(AActor* actor, const uint16& compressedDamage)
{
	if (actor && actor->IsValidLowLevel()) {
		uint16 com = compressedDamage;

		const bool isBackHit = (com & 32768) != 0;
		if (isBackHit)
			com -= 32768;

		const bool isHeal = (com & 16384) != 0;
		if (isHeal)
			com -= 16384;

		static const float max = UINT16_MAX - 32768 - 16384;
		const float amount = (float)com / max * 200.f + 0.1f; // fudge number up a small amount for truncation accuracy loss.

		const auto controller = GetWorld()->GetFirstPlayerController();
		if (controller) {
			auto HUD = Cast<AGoodGameHUD>(controller->GetHUD());
			if (HUD)
				HUD->AddDamageFloater(actor, amount, isBackHit, isHeal);
		}
	}
}

bool AGoodGamePlayerState::ClientToServerChatMessage_Validate(const FChatMessage& message)
{
	// Can never send these channels.
	if (message.TypeID <= (uint8)EChatMessageType::System || message.TypeID == (uint8)EChatMessageType::LENGTH)
		return false;

	// No blank or super long messages.
	const auto len = message.Message.Len();
	return len <= FChatMessage::MAX_CHAT_LENGTH && len > 0;
}

void AGoodGamePlayerState::BroadcastChatMessage_Implementation(const FChatMessage& message)
{
	if (IsNetMode(NM_DedicatedServer))
		return;

	const auto controller = GetWorld()->GetFirstPlayerController();
	auto HUD = Cast<AGoodGameHUD>(controller->GetHUD());
	if (HUD)
		HUD->AddChatMessage(message);
}
