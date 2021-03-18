// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UMiniGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
    class AObjective* ParentInteractable;

	UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* ReturnIcon;

	UPROPERTY(EditAnywhere, Category= "WAT Minigame")
    class USoundCue* SuccessSound;

	UPROPERTY(EditAnywhere, Category= "WAT Minigame")
    class USoundCue* FailSound;

	virtual void Start();
	virtual void Stop();
	virtual bool OnInput(FKey PressedInput, EInputEvent EventType);

	virtual void OnComplete();
	virtual void OnCancel();
	virtual void OnFail();
};
