#pragma once

#include "GameFramework/PlayerState.h"
#include "ChatMessageData.h"
#include "GoodGameHUD.h"
#include "GoodGamePlayerState.generated.h"


UCLASS()
class GOODGAME_API AGoodGamePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	static const int CHAT_TIMEOUT = 1000;
	static const int AREA_CHAT_DISTANCE = 10000;

	// Called by server to send a message to this client.
	UFUNCTION(Client, Reliable, Category = Chat)
	void ServerToClientChatMessage(const FChatMessage& message);

	// Called by client to send a message to the server.
	UFUNCTION(Server, Reliable, WithValidation, Category = Chat)
	void ClientToServerChatMessage(const FChatMessage& message);
	
	// Called from server to relay a message to all clients.
	UFUNCTION(NetMulticast, Reliable, Category = Chat)
	void BroadcastChatMessage(const FChatMessage& message);

	UFUNCTION(Category = DamageNumbers)
	void SendDamageNumber(AActor* actor, const float& amount, const bool& isBackHit = false, const bool& isHeal = false);

	UFUNCTION(Client, Reliable, Category = DamageNumbers)
	void ServerToClientSendDamageNumber(AActor* actor, const uint16& compressedDamage);

	UPROPERTY(BlueprintReadWrite, Category = Chat)
	float NextChat;

	UPROPERTY(BlueprintReadWrite, Category = Chat)
	FString LastChatMessage;

};
