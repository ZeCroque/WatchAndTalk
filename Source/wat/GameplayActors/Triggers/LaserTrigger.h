// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "LaserTrigger.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ALaserTrigger : public AActor
{
	GENERATED_BODY()
	ALaserTrigger();
	
	UPROPERTY(VisibleAnywhere)
	UBoxComponent* CollisionBoxComponent; 

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


