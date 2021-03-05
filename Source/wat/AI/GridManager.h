// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

UCLASS()
class WAT_API AGridManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGridManager();
	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	float GetValueAt(int X, int Y);

	void SetValueAt(int X, int Y, float Value);

	void DecrementValueAt(int X, int Y, float Value);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	void SetValueAround(const FVector& Center, const int Ray);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	float GetValueAround(const FVector& Center, const int Ray);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	void SetValueOutside(const FVector& Center, const int Ray);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	void CleanPolygon(const TArray<FVector>& WorldPolygon);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	bool RandomHotWorldCoordinate(FVector& Vector);

	UFUNCTION(BlueprintCallable, Category="WAT - Function")
	bool SmartRandomHotWorldCoordinate(FVector& Vector, int Try, int Ray);

	FVector2D GetGridCoordinate(const FVector& WorldCoordinate) const;
	FVector GetWorldCoordinate(const FVector2D& GridCoordinate) const;

	static bool IsInsidePolygon(const TArray<FVector2D>& Polygon, const FVector2D& Point);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup")
	int FloorHeight = 405;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup", meta = (AllowPrivateAccess = "true"))
	int CellPerSide = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup", meta = (AllowPrivateAccess = "true"))
	float CoolingStrength = 0.09f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup", meta = (AllowPrivateAccess = "true"))
	float CoolingInterval = 1.f;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WAT - Setup", meta = (AllowPrivateAccess = "true"))
	int GroundHeight = 94;

	FVector2D MinWorld;
	FVector2D MaxWorld;

	float CellWidth; // X
	float CellHeight; // Y

	float SumOfWeight;

	float Timer;

	TArray<float> Grid;
};
