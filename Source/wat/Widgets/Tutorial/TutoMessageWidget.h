// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutoMessageWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UTutoMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY()
	class AWatPlayerCharacter* Character;
	
	UPROPERTY()
	class AWatPlayerController* Controller;

	UPROPERTY(meta=(BindWidget))
	class UKeyIconWidget* TutoKeyIcon;

	UPROPERTY(EditAnywhere)
	FName ActionName;

	virtual void Init();
	
	virtual void OnInput(FKey PressedInput, EInputEvent EventType);
};
