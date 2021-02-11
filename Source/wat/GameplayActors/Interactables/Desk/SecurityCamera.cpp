// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"

#include "Components/SceneCaptureComponent2D.h"

ASecurityCamera::ASecurityCamera()
{
	CameraNumber = 0;
	GetCaptureComponent2D()->bCaptureEveryFrame = false;
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	CameraMesh->SetupAttachment(RootComponent);
}
