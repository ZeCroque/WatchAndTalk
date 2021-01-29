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

	UFUNCTION(BlueprintCallable, Category="Grid")
	float GetValueAt(int X, int Y);

	void SetValueAt(int X, int Y, float Value);

	void DecrementValueAt(int X, int Y, float Value);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void SetValueAround(const FVector& Center, const int Ray);

	UFUNCTION(BlueprintCallable, Category="Grid")
	void CleanPolygon(const TArray<FVector>& WorldPolygon);

	UFUNCTION(BlueprintCallable, Category="Grid")
	bool RandomHotWorldCoordinate(FVector& Vector);

	FVector2D GetGridCoordinate(const FVector& WorldCoordinate) const;
	FVector GetWorldCoordinate(const FVector2D& GridCoordinate) const;

	static bool IsInsidePolygon(const TArray<FVector2D> &Polygon, const  FVector2D &Point);

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int CellPerSide = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	float CoolingStrength = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int FloorHeight = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup", meta = (AllowPrivateAccess = "true"))
	int GroundHeight = 0;

	FVector2D MinWorld;
	FVector2D MaxWorld;

	float CellWidth; // X
	float CellHeight; // Y

	float SumOfWeight;

	TArray<float> Grid;
};
