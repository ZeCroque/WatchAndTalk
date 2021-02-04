// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Camera/CameraComponent.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Monitor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSnowTransitionEndDelegate, bool, bWasOn);

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
    UFont* Font;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
	FString CameraNumberPrefix;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings",  meta = (ClampMin = "0.0") )
	float TextMargin;
	
	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
    UMaterialInterface* BaseRenderMaterial;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings")
    UMaterialInterface* BaseSnowMaterial;

	UPROPERTY(EditAnywhere,  Category = "WAT MonitorSettings")
	UMaterialInstance* PostProcessMaterialBase;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PaddingPercentage;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "2", ClampMax = "12"))
	int GraphicQuality;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0"))
	float BlendingModeSnowTransitionTime;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0"))
	float FloorSwitchModeSnowTransitionTime;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxSnowOpacity;

	UPROPERTY(EditAnywhere, Category = "WAT MonitorSettings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinSnowOpacity;
	
    UPROPERTY()
    UMaterialInstanceDynamic* ScreenMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* SnowMaterial;

	UPROPERTY()
	UMaterialInstanceDynamic* PostProcessMaterial;

    UPROPERTY()
    UTextureRenderTarget2D* ScreenRenderTarget;

	UPROPERTY()
	TArray<UMaterialInstanceDynamic*> CameraRenderMaterials;

	UPROPERTY()
	TArray<UTextureRenderTarget2D*> CameraRenderTargets;

	bool bStartSnowGrowth;

	bool bStartSnowFading;

	bool bIsInSnowFadingMode;

	bool bIsInSnowGrowthMode;
	
	FSnowTransitionEndDelegate OnTransitionEnd;
	
private:

	UPROPERTY()
	class ACameraDesk* ParentDesk;
	
	UPROPERTY()
	class AMainGameState* GameState;

	float ScreenWidth;
	
	float AspectRatio;
	
	float RenderTargetWidth;

	float RenderTargetHeight;

	float ElapsedTimeSinceSnowTransitionStart;

	float ElapsedTime;

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
