#pragma once

#include "GoodGame.h"
#include "SchoolType.h"
#include "SchoolData.generated.h"

USTRUCT()
struct FSchoolData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = School)
	ESchoolType SchoolTypeID;

	UPROPERTY(EditAnywhere, Category = School)
	FString Name = "Name";

	UPROPERTY(EditAnywhere, Category = School)
	FString Description = "Description";

	UPROPERTY(EditAnywhere, Category = School)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = School)
	FColor Color =  FColor(255, 255, 255, 255);
};
