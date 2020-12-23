// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Desk/Monitor.h"
#include "MonitorDegeu.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API AMonitorDegeu : public AMonitor
{
	GENERATED_BODY()

public:
        AMonitorDegeu();

        UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* ScreenMesh1;

        UPROPERTY()
        UMaterialInstanceDynamic* ScreenMaterial1;

        UPROPERTY()
        class USceneCaptureComponent2D* RenderSourceCamera1;

        UPROPERTY()
        UTextureRenderTarget2D* RenderTarget1;

        UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* ScreenMesh2;

        UPROPERTY()
        UMaterialInstanceDynamic* ScreenMaterial2;

        UPROPERTY()
        class USceneCaptureComponent2D* RenderSourceCamera2;

        UPROPERTY()
        UTextureRenderTarget2D* RenderTarget2;

         UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* ScreenMesh3;

        UPROPERTY()
        UMaterialInstanceDynamic* ScreenMaterial3;

        UPROPERTY()
        class USceneCaptureComponent2D* RenderSourceCamera3;

        UPROPERTY()
        UTextureRenderTarget2D* RenderTarget3;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
