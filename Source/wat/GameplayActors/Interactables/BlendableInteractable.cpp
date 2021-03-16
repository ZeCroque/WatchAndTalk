// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Interactables/BlendableInteractable.h"


#include "GeneratedCodeHelpers.h"
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

void ABlendableInteractable::OnInteraction()
{
	Super::OnInteraction();
	
	SetUseState(true);
	bIsFinalBlend = false;
	PlayerCharacter->DisableInput(PlayerController);
	RegisterBlendCompleteCallback(PlayerController);
	KeyIconWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	PlayerController->SetViewTargetWithBlend(this, BlendTime, VTBlend_Linear, 0, true);
}

void ABlendableInteractable::OnNotFinalBlend()
{
	PlayerController->MainHUDWidget->SetVisibility(ESlateVisibility::Hidden);
	PlayerController->PossessRPC(this);
}

void ABlendableInteractable::OnFinalBlend()
{
	bIsFinalBlend = false;
	PlayerController->MainHUDWidget->SetVisibility(ESlateVisibility::Visible);
	PlayerController->NotifyPossessionEnd(this);
	PlayerController->PossessRPC(PlayerCharacter, true);
    PlayerController->PlayerCameraManager->OnBlendComplete().RemoveAll(this);
}

void ABlendableInteractable::BlendToPlayer()
{
	bIsFinalBlend = true;
	PlayerController->SetViewTargetWithBlend(PlayerCharacter, BlendTime, VTBlend_Linear, 0, true);
	PlayerController->BroadcastInteractionEnd(PlayerCharacter, this);
}

void ABlendableInteractable::CheckInteractionState()
{
	for(auto* OverlappingPlayer : OverlappingPlayers)
	{
		if(CanInteract(OverlappingPlayer))
		{
			OverlappingPlayer->AddInteractable(this);
		}
		else
		{
			OverlappingPlayer->RemoveInteractable(this);
		}
	}
}

bool ABlendableInteractable::CanInteract(AWatPlayerCharacter* Player)
{
	return Super::CanInteract(Player) && !bIsInUse;
}

void ABlendableInteractable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlendableInteractable, bIsInUse);
}

void ABlendableInteractable::SetUseState(bool bInUse)
{
	bIsInUse = bInUse;

	CheckInteractionState();
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
