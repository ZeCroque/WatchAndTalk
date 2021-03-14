// Fill out your copyright notice in the Description page of Project Settings.


#include "TutoMessageWidget.h"
#include "KeyIconWidget.h"
#include "wat/Core/WatPlayerCharacter.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"

void UTutoMessageWidget::Init()
{
	TutoKeyIcon->SetBrushTexture(UWatStaticUtils::GetKeyIcon(ActionName, Controller->bIsUsingController));
	Character->bCrouchEnabled = true;
}

void UTutoMessageWidget::OnInput(FKey PressedInput)
{
	if(PressedInput == UWatStaticUtils::GetKeyForDevice(ActionName, Controller->bIsUsingController))
    {
		RemoveFromParent();
		Controller->TutoMessageWidget = nullptr;		
    }
}
