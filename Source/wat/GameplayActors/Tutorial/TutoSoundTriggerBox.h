// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "TutoSoundTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ATutoSoundTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void OnTriggerStart(AActor* OverlappingActor);
};
