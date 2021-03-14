// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "wat/Widgets/TutoMessageWidget.h"

#include "TutoMessageTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ATutoMessageTriggerBox : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UTutoMessageWidget> TutoMessageClass;

	ATutoMessageTriggerBox();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	bool bShown;

};
