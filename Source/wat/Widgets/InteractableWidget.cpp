// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Widgets/InteractableWidget.h"

#include "Brushes/SlateImageBrush.h"

FSlateBrush UInteractableWidget::GetInputTexture() const
{
	return IconBrush;
}

void UInteractableWidget::SetBrushTexture(UTexture2D* InIconTexture)
{
	IconTexture = InIconTexture;
	const FVector2D ImageSize(IconTexture->GetSizeX(),IconTexture->GetSizeY());
	IconBrush = FSlateImageBrush("InteractionIcon", ImageSize);
	IconBrush.SetResourceObject(IconTexture);
}

