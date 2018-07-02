#pragma once

#include "GameFramework/GameStateBase.h"
#include "GoodGameStateBase.generated.h"


UCLASS()
class GOODGAME_API AGoodGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable, Category = Chat)
	void BroadcastChatMessage(const FChatMessage& message);

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

};
