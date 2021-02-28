// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/GameplayActors/Interactables/Interactable.h"
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

	UPROPERTY(EditAnywhere, Category = "WAT BlendableInteractable", meta = (ClampMin = "0.0"))
	float BlendTime;

	ABlendableInteractable();

	UFUNCTION(NetMulticast, Reliable)
	void RegisterBlendCompleteCallback(AWatPlayerController* TargetController);
	
	virtual void OnInteraction() override;

	void OnBlendComplete();
	
	virtual void OnNotFinalBlend();

	virtual void OnFinalBlend();

	void BlendToPlayer();

	void CheckInteractionState();
	
	void SetUseState(bool bInUse);

	virtual bool CanInteract(AWatPlayerCharacter* Player) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:

	UPROPERTY(Replicated)
	bool bIsInUse;
	
	bool bIsFinalBlend;
};
