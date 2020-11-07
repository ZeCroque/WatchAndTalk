// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "PlayerCharacter.generated.h"

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

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
    UPROPERTY()
    class UAIPerceptionStimuliSourceComponent* Stimulus;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound", meta=(AllowPrivateAccess = "true"))
    USoundBase* DistractionSound;
    
    void SetupStimulus();

    void OnDistract();
};
