// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/BlendableInteractable.h"

#include "Camera/CameraComponent.h"
#include "Core/MainPlayerController.h"

ABlendableInteractable::ABlendableInteractable()
{
	BlendTime = 1.0f;
	
	InteractableCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractableCamera"));
	InteractableCamera->SetupAttachment(RootComponent);
}

void ABlendableInteractable::OnInteractionBegin()
{
	PlayerController->SetViewTargetWithBlend(this, BlendTime);
}
