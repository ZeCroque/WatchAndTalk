// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControlsMap.generated.h"

UCLASS()
class WAT_API AControlsMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AControlsMap();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MapMesh;

	UPROPERTY(EditAnywhere, Category="WAT Inputs")
	UMaterialInstance* KeyboardControlsMaterial;

	UPROPERTY(EditAnywhere, Category="WAT Inputs")
	UMaterialInstance* GamepadControlsMaterial;

	UPROPERTY()
	class AWatPlayerController* PlayerController;

	bool bIsShowingGamepad;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
