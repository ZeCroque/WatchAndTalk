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

	virtual void Start();
	virtual void Stop();
	virtual bool OnInput(FKey PressedInput);

	virtual void OnComplete();
	virtual void OnCancel();
	virtual void OnFail();
};
