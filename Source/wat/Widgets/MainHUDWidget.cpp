// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHUDWidget.h"


#include "KeyIconWidget.h"
#include "wat/Core/WatPlayerCharacter.h"

void UMainHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(ParentCharacter)
	{
		ClassIcon->SetBrushTexture(ParentCharacter->IsMechanist() ? MechanistIcon : ScientistIcon);
	}
}
