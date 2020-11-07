// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class AInteractable;
UCLASS()
class WAT_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* FirstPersonCameraComponent = nullptr;
	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* Mesh1P = nullptr;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, Category = Camera)
		float BaseLookUpRate;

	bool AddInteractable(AInteractable* Interactable);
	void RemoveInteractable(AInteractable* Interactable);
	AInteractable* GetClosestInteractable();
	bool IsClosestInteractable(AInteractable* Interactable);
		

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void Interact();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	TSet<AInteractable*> InteractablesSet;
};