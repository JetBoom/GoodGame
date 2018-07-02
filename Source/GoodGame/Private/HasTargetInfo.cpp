#include "GoodGame.h"
#include "HasTargetInfo.h"


UHasTargetInfo::UHasTargetInfo(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}


FString IHasTargetInfo::GetTargetName() const
{
	return FString();
}

FString IHasTargetInfo::GetTargetSubName() const
{
	return FString();
}
