// Fill out your copyright notice in the Description page of Project Settings.


#include "SpamWidget.h"


#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "wat/Widgets/KeyIconWidget.h"

bool USpamWidget::OnInput(FKey PressedInput)
{
    TArray<FKey> Keys;
    PossibleInputs.GetKeys(Keys);

    if(!Super::OnInput(PressedInput))
    {
        if (PressedInput == Keys[0])
        {
            if (CurrentPressNumber < PressNumberNeeded)
            {
                ++CurrentPressNumber;
            }else
            {
                OnComplete();
            }
        } 
    }
    return true;
}

void USpamWidget::Start()
{
    Super::Start();
    PossibleInputs.Empty();
    for(const auto ActionName : PossibleActionsName)
    {
        PossibleInputs.Add(UWatStaticUtils::GetKeyForDevice(ActionName, ParentInteractable->PlayerController->bIsUsingController), UWatStaticUtils::GetKeyIcon(ActionName, ParentInteractable->PlayerController->bIsUsingController));
    }
    KeyIconWidget->SetBrushTexture(*PossibleInputs.Find(UWatStaticUtils::GetKeyForDevice(PossibleActionsName[0], ParentInteractable->PlayerController->bIsUsingController)));
}

void USpamWidget::OnComplete()
{
	ParentInteractable->EndInteraction(true, false);
}

void USpamWidget::OnCancel()
{
	ParentInteractable->EndInteraction(false, false);
}
