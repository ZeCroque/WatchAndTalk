// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GridManager.h"
#include "GridClearComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WAT_API UGridClearComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGridClearComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int RayCountByRow = 32;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int RayLength = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int FieldOfView = 210;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int ViewForwardOffset = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int ViewUpOffset = 45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float TracePeriod = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float RaysRowMinimumAngle = -10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	float RaysRowMaximumAngle = -30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Grid", meta = (AllowPrivateAccess = "true"))
	int RaysRowCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Debug", meta = (AllowPrivateAccess = "true"))
	bool bShowLineTraces = false;

	UPROPERTY()
	AGridManager* GridManager;

	UPROPERTY()
	float Timer;
};
