// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "wat/AI/GridManager.h"
#include "GameFramework/Actor.h"
#include "Components/CapsuleComponent.h"

#include "GridTrigger.generated.h"

UCLASS()
class WAT_API AGridTrigger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridTrigger();

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* HitBox;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float DistanceFromBox = 500;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int GridDrawRay = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float CoolDown = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Debug", meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

	FVector Origin;

	float Timer;

	UPROPERTY()
	AGridManager* GridManager;
};
