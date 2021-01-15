// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interactables/Objectives/QTEInteractable.h"

#include "QTEWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    UPROPERTY()
    AQTEInteractable* ParentInteractable;
    // Array de définition des inputs pouvant être utilisés dans le QTE
    UPROPERTY(EditAnywhere)
    TArray<FKey> PossibleInputs;
    UPROPERTY(BlueprintReadOnly)
    TArray<FKey> InputList;    
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int InputNumber;
    UPROPERTY(BlueprintReadOnly)
    int CurrentIndex = 0;
    //TIMER
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float TimeLimit;
    UPROPERTY(BlueprintReadWrite)
    float TimeLeft;

    void OnInput(FKey PressedInput);
    
private:
    UFUNCTION(BlueprintCallable)
    void DefineInputList();

    UFUNCTION(BlueprintCallable)
    FString GetCurrentInputLabel();

    UFUNCTION(BlueprintCallable)
    float GetProgressBarCompletion();
    
    void OnComplete();

    void OnFail();
};
