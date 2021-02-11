// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "LaserInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ALaserInteractable : public AInteractable
{
	GENERATED_BODY()

	void virtual OnInteraction() override;
	void virtual OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
};
