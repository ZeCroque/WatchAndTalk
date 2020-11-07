// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeskMap.generated.h"

UCLASS()
class WAT_API ADeskMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeskMap();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MapStaticMesh;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* BlendTarget;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
