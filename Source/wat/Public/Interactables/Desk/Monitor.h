// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monitor.generated.h"

UCLASS()
class WAT_API AMonitor : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Debug")
	int DebugZoneIndex;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* BlendTargetCamera;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MonitorMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ScreenMesh;

	UPROPERTY(EditAnywhere)
        UMaterialInterface* BaseRenderMaterial;

        UPROPERTY()
        UMaterialInstanceDynamic* ScreenMaterial;

        UPROPERTY()
        UTextureRenderTarget2D* ScreenRenderTarget;


	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> CameraRenderMaterials;

	UPROPERTY()
	TArray<class USceneCaptureComponent2D*> RenderSourceCameras;

	UPROPERTY()
	TArray<UTextureRenderTarget2D*> CameraRenderTargets;

	AMonitor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
