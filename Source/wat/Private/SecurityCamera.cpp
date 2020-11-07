// Fill out your copyright notice in the Description page of Project Settings.


#include "SecurityCamera.h"

ASecurityCamera::ASecurityCamera()
{
	CameraNumber = 0;
	
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	CameraMesh->SetupAttachment(RootComponent);
}
