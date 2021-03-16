// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/Widgets/MiniGames/KeyBasedMiniGameWidget.h"
#include "SpamWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API USpamWidget : public UKeyBasedMiniGameWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta=(BindWidget))
	class UKeyIconWidget* KeyIconWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "WAT SPAM MINI GAME")
	int PressNumberNeeded;

	UPROPERTY(BlueprintReadOnly)
	int CurrentPressNumber = 0;

	virtual bool OnInput(FKey PressedInput) override;

	virtual void Start() override;

	virtual void OnComplete() override;

	virtual void OnCancel() override;

protected:
	UPROPERTY()
	TMap<FKey, UTexture2D*> PossibleInputs;

};
