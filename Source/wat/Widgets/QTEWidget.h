// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MiniGames/MiniGameWidget.h"
#include "wat/GameplayActors/Interactables/Objectives/QTEInteractable.h"

#include "QTEWidget.generated.h"

/**
 * 
 */
UCLASS()
class WAT_API UQTEWidget : public UMiniGameWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
    // Array de définition des inputs pouvant être utilisés dans le QTE
    UPROPERTY(EditAnywhere, Category= "WAT QTE")
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "WAT QTE")
    float TimeLimit;
	
    UPROPERTY(BlueprintReadWrite)
    float TimeLeft;

    UPROPERTY(meta=(BindWidget))
    class UKeyIconWidget* KeyIconWidget;

    virtual void OnInput(FKey PressedInput) override;

    virtual void Start() override;

    virtual void Stop() override;
    
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
