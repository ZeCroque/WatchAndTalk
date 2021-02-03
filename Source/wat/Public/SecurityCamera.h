// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "SecurityCamera.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API ASecurityCamera : public ASceneCapture2D
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CameraMesh;
	UPROPERTY(EditAnywhere, Category = "WAT CameraSettings")
		int32 CameraNumber;
	UPROPERTY(EditAnywhere, Category = "WAT CameraSettings")
		int32 ZoneNumber;
	UPROPERTY(EditAnywhere, Category = "WAT CameraSettings")
		int32 FloorNumber;
	ASecurityCamera();
};
