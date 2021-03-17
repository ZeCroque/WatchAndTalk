// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"

#include "TutoBunkerDoor.generated.h"

UCLASS()
class WAT_API ATutoBunkerDoor : public AActor
{
	GENERATED_BODY()
	
private:
	int OverlappingPlayerCount;
	
public:	
	// Sets default values for this actor's properties
	ATutoBunkerDoor();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DoorTrigger;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentD;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentAC;

	UPROPERTY(VisibleAnywhere)
	UAudioComponent* AudioComponentC;
	
	UPROPERTY(EditAnywhere, Category= "WAT Door")
	float DoorMovingSpeed;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* ButtonOn;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* ButtonOff;
	
	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorEnd;

	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorUpFromBottom;
	
	UPROPERTY(EditAnywhere, Category="WAT - Sound")
	USoundCue* DoorUpFromNotBottom;

	bool bHasMove;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
