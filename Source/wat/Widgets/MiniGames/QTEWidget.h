// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "wat/Widgets/MiniGames/KeyBasedMiniGameWidget.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"

#include "QTEWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UQTEWidget : public UKeyBasedMiniGameWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "WAT QTE")
    int InputNumber;
	
    UPROPERTY(BlueprintReadOnly)
    int CurrentIndex = 0;

    UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* KeyIconWidget;

    virtual bool OnInput(FKey PressedInput) override;

    virtual void Start() override;

protected:
	UPROPERTY()
    TMap<FKey, UTexture2D*> PossibleInputs;
	
private:
    TArray<FKey> InputList;
	
    void DefineInputList();
	
    UFUNCTION(BlueprintCallable)
    float GetProgressBarCompletion() const;
};
