#pragma once

#include "HasTargetInfo.generated.h"


UINTERFACE(MinimalAPI)
class UHasTargetInfo : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};


class GOODGAME_API IHasTargetInfo
{
	GENERATED_IINTERFACE_BODY()

public:
	UFUNCTION()
	virtual FString GetTargetName() const;

	UFUNCTION()
	virtual FString GetTargetSubName() const;

};
