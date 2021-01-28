// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/BlendableInteractable.h"

#include "Camera/CameraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Core/MainPlayerController.h"

ABlendableInteractable::ABlendableInteractable()
{
	BlendTime = 1.0f;
	
	InteractableCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractableCamera"));
	InteractableCamera->SetupAttachment(RootComponent);
}

void ABlendableInteractable::OnInteractionBegin()
{
	bIsFinalBlend = false;
	PlayerCharacter->DisableInput(PlayerController);
	PlayerController->PlayerCameraManager->OnBlendComplete().AddUObject(this, &ABlendableInteractable::OnBlendComplete);
}

void ABlendableInteractable::OnInteraction()
{
	PlayerController->SetViewTargetWithBlend(this, BlendTime, VTBlend_Linear, 0, true);
}

void ABlendableInteractable::OnBlendComplete()
{
	UE_LOG(LogTemp, Warning, TEXT("BlendEnded"));
	if(!bIsFinalBlend)
	{
		OnNotFinalBlend();		
	}
    else
    {
    	OnFinalBlend();
    }	
}

void ABlendableInteractable::OnNotFinalBlend()
{
	PlayerController->Possess(this);
}

void ABlendableInteractable::OnFinalBlend()
{
	PlayerController->Possess(PlayerCharacter);
	PlayerCharacter->EnableInput(PlayerController);
    PlayerController->PlayerCameraManager->OnBlendComplete().RemoveAll(this);
}

void ABlendableInteractable::BlendToPlayer()
{
	bIsFinalBlend = true;
	PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime, VTBlend_Linear, 0, true);
}
