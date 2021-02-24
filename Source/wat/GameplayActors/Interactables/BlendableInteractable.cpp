// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/BlendableInteractable.h"

#include "Camera/CameraComponent.h"
#include "wat/Core/WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"

ABlendableInteractable::ABlendableInteractable()
{
	BlendTime = 1.0f;
	
	InteractableCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractableCamera"));
	InteractableCamera->SetupAttachment(RootComponent);
}

void ABlendableInteractable::RegisterBlendCompleteCallback_Implementation(AWatPlayerController* TargetController)
{
	if(TargetController == GetWorld()->GetFirstPlayerController())
	{
		TargetController->PlayerCameraManager->OnBlendComplete().AddUObject(this, &ABlendableInteractable::OnBlendComplete);
	}
}

void ABlendableInteractable::OnInteractionBegin()
{
	Super::OnInteractionBegin();
	
	bIsFinalBlend = false;
	PlayerCharacter->DisableInput(PlayerController);
	RegisterBlendCompleteCallback(PlayerController);
}	

void ABlendableInteractable::OnInteraction()
{
	PlayerController->SetViewTargetWithBlend(this, BlendTime, VTBlend_Linear, 0, true);
}

void ABlendableInteractable::OnNotFinalBlend()
{
	PlayerController->PossessRPC(this);
}

void ABlendableInteractable::OnFinalBlend()
{
	bIsFinalBlend = false;
	bInUse = false;
	PlayerController->PossessRPC(PlayerCharacter, true);
    PlayerController->PlayerCameraManager->OnBlendComplete().RemoveAll(this);
}

void ABlendableInteractable::BlendToPlayer()
{
	bIsFinalBlend = true;
	PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime, VTBlend_Linear, 0, true);
	PlayerController->BroadcastInteractionEnd(PlayerCharacter, this);
}

void ABlendableInteractable::OnBlendComplete()
{

        if(!bIsFinalBlend)
        {
	        OnNotFinalBlend();		
        }
        else
        {
    	        OnFinalBlend();
        }	
    }
