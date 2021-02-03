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


	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* BlendTargetCamera;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MonitorMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ScreenMesh;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0", ClampMax = "4"))
	int ZoneIndex;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0", ClampMax = "1"))
	int FloorIndex;
	
	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
    UMaterialInterface* BaseRenderMaterial;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PaddingPercentage;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "2", ClampMax = "12"))
	int GraphicQuality;
	
    UPROPERTY()
    UMaterialInstanceDynamic* ScreenMaterial;

    UPROPERTY()
    UTextureRenderTarget2D* ScreenRenderTarget;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> CameraRenderMaterials;

	UPROPERTY()
	TArray<UTextureRenderTarget2D*> CameraRenderTargets;

	bool bShowSnow;
private:

	UPROPERTY()
	class AMainGameState* GameState;

	float RenderTargetWidth;

	float RenderTargetHeight;
	
public:
	AMonitor();

	void SwitchFloor();

	void SetRenderingState(bool bShouldRender);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
