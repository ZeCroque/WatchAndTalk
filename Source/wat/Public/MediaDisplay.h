// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "MediaDisplay.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AMediaDisplay : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UMaterialInstanceDynamic* ScreenMaterial;

	UPROPERTY()
	USceneCaptureComponent2D* Camera;
	
	UPROPERTY(VisibleAnywhere)
	UTextureRenderTarget2D* RenderTarget;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Screen;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	AMediaDisplay();
	void PostInitializeComponents() override;
	void BeginPlay() override;
	
};
