// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Interactable.h"
#include "BlendableInteractable.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ABlendableInteractable : public AInteractable
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* InteractableCamera;

	UPROPERTY(EditAnywhere)
	float BlendTime;

	ABlendableInteractable();

	virtual void OnInteractionBegin() override;
	
};
