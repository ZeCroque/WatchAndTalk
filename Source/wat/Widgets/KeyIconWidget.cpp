// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/Widgets/KeyIconWidget.h"

#include "Brushes/SlateImageBrush.h"

FSlateBrush UKeyIconWidget::GetInputTexture() const
{
	return IconBrush;
}

void UKeyIconWidget::SetBrushTexture(UTexture2D* InIconTexture)
{
	IconTexture = InIconTexture;
	const FVector2D ImageSize(IconTexture->GetSizeX(),IconTexture->GetSizeY());
	IconBrush = FSlateImageBrush("InteractionIcon", ImageSize);
	IconBrush.SetResourceObject(IconTexture);
}

