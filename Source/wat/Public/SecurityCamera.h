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
	UPROPERTY(EditAnywhere, Category = "Gameplay")
		int32 CameraNumber;
	UPROPERTY(EditAnywhere, Category = "Debug")
		int32 ZoneNumber;
	ASecurityCamera();
};
