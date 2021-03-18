// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniGameWidget.h"



#include "Kismet/GameplayStatics.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "wat/Widgets/KeyIconWidget.h"

void UMiniGameWidget::Start()
{
	ReturnIcon->SetBrushTexture(UWatStaticUtils::GetKeyIcon("Return", ParentInteractable->PlayerController->bIsUsingController));
}

void UMiniGameWidget::Stop()
{
	RemoveFromParent();
	ParentInteractable->PlayerController->MiniGameWidget = nullptr;
}

bool UMiniGameWidget::OnInput(FKey PressedInput, EInputEvent EventType)
{
	if(EventType == EInputEvent::IE_Pressed && PressedInput == UWatStaticUtils::GetKeyForDevice("Return", ParentInteractable->PlayerController->bIsUsingController))
    {
    	OnCancel();
		return true;
    }
	return false;
}

void UMiniGameWidget::OnComplete()
{
    ParentInteractable->EndInteraction(true, true);
	UGameplayStatics::SpawnSoundAtLocation(ParentInteractable, SuccessSound, ParentInteractable->GetActorLocation());
}

void UMiniGameWidget::OnCancel()
{
    ParentInteractable->EndInteraction(false, true);
}

void UMiniGameWidget::OnFail()
{
    ParentInteractable->EndInteraction(false, true);
	UGameplayStatics::SpawnSoundAtLocation(ParentInteractable, FailSound, ParentInteractable->GetActorLocation());
}
