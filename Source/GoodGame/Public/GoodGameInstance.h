#pragma once

#include "Engine/GameInstance.h"
#include "GoodGameInstance.generated.h"


UCLASS()
class GOODGAME_API UGoodGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
};
