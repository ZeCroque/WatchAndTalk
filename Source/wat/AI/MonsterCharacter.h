// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MonsterCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventReactionMode);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEventPlayerIsNowInSight);

UCLASS()
class WAT_API AMonsterCharacter final : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AMonsterCharacter();

	void MeleeAttack(class AWatPlayerCharacter* Player);

	UAnimMontage* GetMontage() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> GameOverWidgetTemplate;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere)
	class UGridClearComponent* GridClearComponent = nullptr;

	UFUNCTION(NetMulticast, Reliable)
	void KillPlayer(class AWatPlayerCharacter* Player);

	void StartGoalReaction(ACharacter* Character);
	void StopGoalReaction();

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnEventReactionMode"))
	FOnEventReactionMode OnEventReactionModeDispatcher;

	UPROPERTY(BlueprintAssignable, meta = (DisplayName = "OnEventPlayerIsNowInSight"))
	FOnEventPlayerIsNowInSight OnEventPlayerIsNowInSightDispatcher;

	UPROPERTY()
	AWatPlayerCharacter* PlayerToChase;

	UPROPERTY(BlueprintReadWrite, Category = "WAT")
	bool bNotAnimPlaying;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* Montage;
};
