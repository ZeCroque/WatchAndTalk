// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"
#include "GenericPlatform/GenericPlatformMisc.h"

class AMainPlayerController;

bool UQTEWidget::Initialize()
{
    DefineInputList();
    //Cast<AMainPlayerController>(ParentInteractable->PlayerController)->bIsInMiniGame = true;
    TimeLeft = TimeLimit;
    return Super::Initialize();
}

void UQTEWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if (TimeLeft > 0)
    {
        TimeLeft -= InDeltaTime;
    } else
    {
        OnFail();
    }
    Super::NativeTick(MyGeometry, InDeltaTime);
}

void UQTEWidget::DefineInputList()
{
    for (int i = 0; i < InputNumber; ++i)
    {
        InputList.Push(PossibleInputs[rand() % PossibleInputs.Num()]);
     }
        
}

FString UQTEWidget::GetCurrentInputLabel()
{
    return InputList[CurrentIndex].GetFName().ToString();
}

void UQTEWidget::OnInput(FKey PressedInput)
{
    if (PressedInput == InputList[CurrentIndex])
    {
        if (CurrentIndex < InputList.Num()-1)
        {
            CurrentIndex++;
        } else
        {
            OnComplete();
        }
    }
}

float UQTEWidget::GetProgressBarCompletion()
{
    return float(CurrentIndex+1) / float(InputNumber);
}

void UQTEWidget::OnComplete()
{
    ParentInteractable->InteractionEnd();
}

void UQTEWidget::OnFail()
{
    //TODO OnFail
    ParentInteractable->bCanInteract = true;
    UE_LOG(LogTemp, Warning, TEXT("FAIL FAIL FAIL"));
}
