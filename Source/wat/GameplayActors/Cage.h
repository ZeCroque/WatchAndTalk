// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"

#include "Cage.generated.h"

UCLASS()
class WAT_API ACage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACage();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CageMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LeftDoorMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RightDoorMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BoxCollision; 

	UPROPERTY()
	class AWatGameState* GameState;
	
	UPROPERTY()
	TArray<AActor*> Gas;
	
	UPROPERTY(EditAnywhere, Category="WAT EndGame")
	float DoorMovingSpeed = 175.f;

	UPROPERTY(EditAnywhere, Category="WAT EndGame")
	float MonsterDeathCooldown = 2.f;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentAB;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentAC;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentB;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentD;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorDownFromTop;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorDownFromNotTop;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorUpFromBottom;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorEnd;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CloseDoors();

	void ChangeGasVisibility(bool bVisible);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	bool bShouldMove;

	UPROPERTY(Replicated)
	bool bIsDown;

	UPROPERTY(Replicated)
	bool bIsOverlappingWithMonster;

	float AccumulatedDeltaTime;
	
	float InitialZ;

	void StopSound() const;

};
