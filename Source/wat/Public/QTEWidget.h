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
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
    UPROPERTY()
    AObjective* ParentInteractable;
	
    // Array de définition des inputs pouvant être utilisés dans le QTE
    UPROPERTY(EditAnywhere)
    TArray<FName> PossibleActionsName;

	UPROPERTY()
    TMap<FKey, UTexture2D*> PossibleInputs;
	
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

	UPROPERTY(meta=(BindWidget))
	class UInteractableWidget* KeyIconWidget;

    void OnInput(FKey PressedInput);

	void Start();

	void Stop();
    
private:

    bool bIsStarted;
	
    UFUNCTION(BlueprintCallable)
    void DefineInputList();

    UFUNCTION(BlueprintCallable)
    float GetProgressBarCompletion() const;

    void OnComplete();

    void OnCancel();
	
    void OnFail();
};
