// Fill out your copyright notice in the Description page of Project Settings.


#include "TutoMessageWidget.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/Widgets/KeyIconWidget.h"

void UTutoMessageWidget::Init()
{
	TutoKeyIcon->SetBrushTexture(UWatStaticUtils::GetKeyIcon(ActionName, Controller->bIsUsingController));
}

void UTutoMessageWidget::OnInput(FKey PressedInput)
{
	if(PressedInput == UWatStaticUtils::GetKeyForDevice(ActionName, Controller->bIsUsingController))
    {
		RemoveFromParent();
		Controller->TutoMessageWidget = nullptr;		
    }
}
