// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "WatPlayerCharacter.generated.h"

class AInteractable;
UCLASS()
class WAT_API AWatPlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Sets default values for this character's properties
    AWatPlayerCharacter();

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

	UPROPERTY(EditAnywhere)
	float SprintAdditionalSpeedValue;

	UPROPERTY(EditAnywhere)
	float SprintSpeedIncreaseRate;

	UPROPERTY(EditAnywhere)
	float SprintSpeedDecreaseRate;

	UPROPERTY()
	float OriginalMaxSpeed;


	bool AddInteractable(AInteractable* Interactable);
	void RemoveInteractable(AInteractable* Interactable);
	AInteractable* GetClosestInteractable();
	bool IsClosestInteractable(AInteractable* Interactable);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Inputs

	void CreateDebugWidget();

	//Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	//Special displacements
	void StartSprinting();
	void StopSprinting();
	void StartCrouching();
    void StopCrouching();

	//Interactables
	UFUNCTION(Server, Reliable)
	void Interact();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void SetPlayerAsReadyForRestart();
	
	UFUNCTION(Server, Reliable)
	void RestartGame(uint32 PlayerId);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool IsAlive() const;
	
	void Die();

private:

	bool bIsAlive;
	bool bIsSprinting;
	
    UPROPERTY()
    class UAIPerceptionStimuliSourceComponent* Stimulus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound", meta=(AllowPrivateAccess = "true"))
    USoundBase* DistractionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> GridDebugWidget;

    UPROPERTY()
    TSet<AInteractable*> InteractablesSet;

    void SetupStimulus();

    void OnDistract();

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> SprintShake;
};
