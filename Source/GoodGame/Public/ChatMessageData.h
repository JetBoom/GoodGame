#pragma once

#include "ChatMessageData.generated.h"

UENUM(BlueprintType)
enum class EChatMessageType : uint8
{
	System,
	Global,
	Area,
	Private,

	LENGTH
};


USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_USTRUCT_BODY()

	static const int MAX_CHAT_LENGTH = 255;

	FChatMessage()
	{
		TypeID = (uint8)EChatMessageType::System;
		Message = FString("");
		Actor = nullptr;
	}

	FChatMessage(const FString& msg)
	{
		TypeID = (uint8)EChatMessageType::System;
		Message = msg;
		Actor = nullptr;
	}

	FChatMessage(const FString& msg, const EChatMessageType type)
	{
		TypeID = (uint8)type;
		Message = msg;
		Actor = nullptr;
	}

	FChatMessage(const FString& msg, const EChatMessageType type, AActor* act)
	{
		TypeID = (uint8)type;
		Message = msg;
		Actor = act;
	}

	UPROPERTY()
	uint8 TypeID;

	UPROPERTY()
	FString Message;

	UPROPERTY()
	AActor* Actor;

};
