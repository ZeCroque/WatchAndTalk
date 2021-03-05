// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BunkerDoor.generated.h"

UCLASS()
class WAT_API ABunkerDoor : public AActor
{
	GENERATED_BODY()

private:
    float InitialZ;
	float MeshHeight;
	int OverlappingPlayerCount;
	
public:	
	// Sets default values for this actor's properties
	ABunkerDoor();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* DoorTrigger;

	UPROPERTY(EditAnywhere, Category= "WAT Door")
	float DoorMovingSpeed;

	UPROPERTY(EditAnywhere, Category="WAT Door")
	float ZOffset = 1.f;

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
