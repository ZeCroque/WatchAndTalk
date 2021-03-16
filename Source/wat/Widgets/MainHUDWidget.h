// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UKeyIconWidget* ClassIcon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Crosshair;

	UPROPERTY(EditAnywhere, Category="WAT HUD")
	UTexture2D* MechanistIcon;

	UPROPERTY(EditAnywhere, Category="WAT HUD")
	UTexture2D* ScientistIcon;

	UPROPERTY()
	class AWatPlayerCharacter* ParentCharacter;
	
protected:
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
