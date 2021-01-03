// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"

bool UQTEWidget::Initialize()
{
    DefineInputList();
    return Super::Initialize();
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

void UQTEWidget::OnComplete()
{
    //TODO OnComplete
    UE_LOG(LogTemp, Warning, TEXT("C FINI"));
}
