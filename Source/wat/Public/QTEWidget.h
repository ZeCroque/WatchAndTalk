// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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
    UPROPERTY(EditAnywhere)
    TArray<FKey> PossibleInputs;
    UPROPERTY(BlueprintReadOnly)
    TArray<FKey> InputList;
    TArray<FString> InputLabelList;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int InputNumber;
    UPROPERTY(BlueprintReadOnly)
    int CurrentIndex = 0;
    
    
private:
    UFUNCTION(BlueprintCallable)
    void DefineInputList();

    UFUNCTION(BlueprintCallable)
    FString GetCurrentInputLabel();

    UFUNCTION(BlueprintCallable)
    void OnInput(FKey PressedInput);

    void OnComplete();
};
