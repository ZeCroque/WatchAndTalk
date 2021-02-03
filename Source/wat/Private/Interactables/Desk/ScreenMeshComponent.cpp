// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/Desk/ScreenMeshComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Core/MainGameState.h"
#include "SecurityCamera.h"

UScreenMeshComponent::UScreenMeshComponent()
{
	DebugZoneIndex = 0;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("ScreenCamera"));
	CameraComponent->FieldOfView = 75.0f;
	if (!IsTemplate())
	{
		CameraComponent->SetupAttachment(this);	auto* Owner = GetOwner();
		if (Owner)
		{
			CameraComponent->SetWorldLocationAndRotation(FVector(-50.0f, -50.0f, 0.0f), FRotator());
		}
	}

}

void UScreenMeshComponent::BeginPlay()
{
	Super::BeginPlay();

	ScreenMaterial = CreateAndSetMaterialInstanceDynamic(0);
	ScreenMaterial->ShadingModels = FMaterialShadingModelField(EMaterialShadingModel::MSM_Unlit);

	RenderTarget = NewObject<UTextureRenderTarget2D>(this);
	RenderTarget->InitAutoFormat(512, 512);

	auto* GameState = Cast<AMainGameState>(GetWorld()->GetGameState());

	//Camera = GameState->Cameras[DebugZoneIndex].ZoneCameras[0]->GetCaptureComponent2D();
	Camera->TextureTarget = RenderTarget;
	ScreenMaterial->SetTextureParameterValue(TEXT("RenderTexture"), Cast<UTexture>(RenderTarget));
}
