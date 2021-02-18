// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BunkerTriggerBox.generated.h"

class AGridManager;
UCLASS()
class WAT_API ABunkerTriggerBox : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABunkerTriggerBox();

	UPROPERTY(EditAnywhere)
	UBoxComponent* HitBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent,
	                  int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid")
	int GridDrawRay = 15;

private:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float CoolDown = 1;

	UPROPERTY()
	AGridManager* GridManager;
	
	float Timer;
	
};
