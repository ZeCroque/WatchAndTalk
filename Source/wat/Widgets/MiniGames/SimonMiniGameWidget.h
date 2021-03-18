// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "wat/Widgets/MiniGames/KeyBasedMiniGameWidget.h"
#include "SimonMiniGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API USimonMiniGameWidget : public UKeyBasedMiniGameWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	TArray<class UKeyIconWidget*> SimonInputKeyIconWidgets;

	UPROPERTY()
	FLinearColor HighlightColor = FLinearColor(222, 192, 111, 0.5);

	UPROPERTY(EditAnywhere)
	UTexture2D* EmptyKeyTexture;
	UPROPERTY(EditAnywhere)
	UTexture2D* EmptyButtonTexture;
	
	virtual bool OnInput(FKey PressedInput, EInputEvent EventType) override;

	virtual void Start() override;

	UPROPERTY(BlueprintReadOnly)
	int CurrentIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "WAT Simon")
	int InputNumber;

	UPROPERTY()
	TArray<FKey> SimonStepKeyArray;

	UPROPERTY()
	int SimonStepIndex = 0;
protected:
	UPROPERTY()
	TMap<FKey, UTexture2D*> PossibleInputs;

private:
	TArray<FKey> InputList;

	void DefineInputList();

	void DefineSimonStepKeyArray();

};
