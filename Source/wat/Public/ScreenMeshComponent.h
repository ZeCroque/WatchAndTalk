// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneCaptureComponent2D.h"
#include "ScreenMeshComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class WAT_API UScreenMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	UMaterialInstanceDynamic* ScreenMaterial;

	UPROPERTY()
	class USceneCaptureComponent2D* Camera;
	
	UPROPERTY(VisibleAnywhere)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, Category = "Debug")
	int DebugZoneIndex;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* CameraComponent;

	UScreenMeshComponent();
protected:
	virtual void BeginPlay() override;
};
