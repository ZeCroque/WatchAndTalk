// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraDeskHUDWidget.h"

#include "Components/HorizontalBox.h"
#include "wat/GameplayActors/Interactables/Desk/CameraDesk.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Widgets/KeyIconWidget.h"
#include "wat/Core/WatStaticUtils.h"
#include "Components/Image.h"

void UCameraDeskHUDWidget::Init() const
{	
	SwitchFloorsKeyIcon->SetBrushTexture(UWatStaticUtils::GetKeyIcon("Interact", Parent->PlayerController->bIsUsingController));
	ReturnKeyIcon->SetBrushTexture(UWatStaticUtils::GetKeyIcon("Return", Parent->PlayerController->bIsUsingController));

	EndGameHBox->SetVisibility(Parent->bShouldShowEndGameInteraction ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	
	Update(true);
}

void UCameraDeskHUDWidget::Update(bool bIsOnMap, int MonitorIndex, bool bIsFirstFloor) const
{
	if(bIsOnMap)
	{
		TopArrow->SetVisibility(ESlateVisibility::Hidden);
		RightArrow->SetVisibility(ESlateVisibility::Hidden);
		BottomArrow->SetVisibility(ESlateVisibility::Visible);
		LeftArrow->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		TopArrow->SetVisibility(ESlateVisibility::Visible);
		RightArrow->SetVisibility(MonitorIndex == 3 ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
		BottomArrow->SetVisibility(MonitorIndex == 0 || MonitorIndex == 3 ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		LeftArrow->SetVisibility(MonitorIndex ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	SwitchFloorsIcon->SetBrushTexture(bIsFirstFloor ? FirstFloorIcon : SecondFloorIcon);
}

void UCameraDeskHUDWidget::HideAll() const
{
	TopArrow->SetVisibility(ESlateVisibility::Hidden);
	RightArrow->SetVisibility(ESlateVisibility::Hidden);
	BottomArrow->SetVisibility(ESlateVisibility::Hidden);
	LeftArrow->SetVisibility(ESlateVisibility::Hidden);
}
