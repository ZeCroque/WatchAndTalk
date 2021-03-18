// Fill out your copyright notice in the Description page of Project Settings.


#include "SimonMiniGameWidget.h"



#include "Kismet/GameplayStatics.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"
#include "wat/GameplayActors/Interactables/Objectives/Objective.h"
#include "wat/Widgets/KeyIconWidget.h"

bool USimonMiniGameWidget::OnInput(FKey PressedInput, EInputEvent EventType)
{
    TArray<FKey> Keys;
    PossibleInputs.GetKeys(Keys);

    if (!Super::OnInput(PressedInput,EventType) && EventType == IE_Pressed)
    {
        if (PressedInput == SimonStepKeyArray[SimonStepIndex])
        {
        	UGameplayStatics::SpawnSoundAtLocation(ParentInteractable, KeySound, ParentInteractable->GetActorLocation());
            ++SimonStepIndex;
            if (SimonStepIndex > SimonStepKeyArray.Num()-1)
            {
                ++CurrentIndex;
                DefineSimonStepKeyArray();
            }
            if (InputList.Num()-1 >= CurrentIndex)
            {
                SimonInputKeyIconWidgets[CurrentIndex]->SetBrushTexture(*PossibleInputs.Find(InputList[CurrentIndex]));
                for(int i = 0; i<SimonInputKeyIconWidgets.Num()-1; ++i)
                {
                    if (i != CurrentIndex)
                    {
                        if (!ParentInteractable->PlayerController->bIsUsingController)
                        {
                            SimonInputKeyIconWidgets[i]->SetBrushTexture(EmptyKeyTexture);
                        } else
                        {
                            SimonInputKeyIconWidgets[i]->SetBrushTexture(EmptyButtonTexture);

                        }
                    }
                    if (i != SimonStepIndex)
                    {
                        SimonInputKeyIconWidgets[i]->SetColorAndOpacity(HighlightColor);
                    } else
                    {
                        SimonInputKeyIconWidgets[i]->SetColorAndOpacity(FLinearColor(1,1,1,1));
                    }
                }
            }
        } else
        {
            OnFail();
        }
    }
    if (CurrentIndex > InputList.Num()-1)
    {
        OnComplete();
    }
    return true;
}

void USimonMiniGameWidget::Start()
{
    Super::Start();
    PossibleInputs.Empty();
    for (const auto ActionName : PossibleActionsName)
    {
        PossibleInputs.Add(UWatStaticUtils::GetKeyForDevice(ActionName, ParentInteractable->PlayerController->bIsUsingController), UWatStaticUtils::GetKeyIcon(ActionName, ParentInteractable->PlayerController->bIsUsingController));       
    }
    DefineInputList();
}

void USimonMiniGameWidget::DefineInputList()
{
    TArray<FKey> Keys;
    PossibleInputs.GetKeys(Keys);
	
    for (int i = 0; i < InputNumber; ++i)
    {
        InputList.Push(Keys[rand() % Keys.Num()]);
    }
    DefineSimonStepKeyArray();
}

void USimonMiniGameWidget::DefineSimonStepKeyArray()
{
    SimonStepKeyArray.Empty();
    for (int i = 0; i<=CurrentIndex; ++i)
    {
        if (InputList.Num()-1 >= i)
        {
            SimonStepKeyArray.Add(InputList[i]);
            SimonStepIndex = 0;
        }
    }
    for(int i = 0; i<SimonInputKeyIconWidgets.Num(); ++i)
    {
        if (i > CurrentIndex)
        {
             SimonInputKeyIconWidgets[i]->SetVisibility(ESlateVisibility::Hidden);
        } else
        {
            SimonInputKeyIconWidgets[i]->SetVisibility(ESlateVisibility::Visible);
        }
    }
    SimonInputKeyIconWidgets[0]->SetBrushTexture(*PossibleInputs.Find(InputList[0]));
}
