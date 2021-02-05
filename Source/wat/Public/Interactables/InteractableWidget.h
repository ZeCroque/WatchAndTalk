// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Brushes/SlateImageBrush.h"

#include "InteractableWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UInteractableWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	UTexture2D* IconTexture;
	
	FSlateBrush IconBrush;
	
public:
	UFUNCTION(BlueprintCallable)
	FSlateBrush GetInputTexture() const;

	void SetBrushTexture(UTexture2D* InIconTexture);
};
