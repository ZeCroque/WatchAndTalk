// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackableWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UBackableWidget : public UUserWidget
{
	GENERATED_BODY()

public:	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintImplementableEvent)
	void CloseWidget();
};
