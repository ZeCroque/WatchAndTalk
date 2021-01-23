// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"
#include "Core/MainPlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"

class AMainPlayerController;

bool UQTEWidget::Initialize()
{
    DefineInputList();
    TimeLeft = TimeLimit;
    bIsStarted = false;
    return Super::Initialize();
}

void UQTEWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    if(bIsStarted)
    {
	if (TimeLeft > 0)
	{
	    TimeLeft -= InDeltaTime;
	}
	else
	{
	    OnFail();
	}
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
        }
        else
        {
            OnComplete();
        }
    }
    else
    {
    	TArray<FInputActionKeyMapping> ActionsMappings;

    	UInputSettings::GetInputSettings()->GetActionMappingByName("Return", ActionsMappings);

    	for(auto ActionMapping : ActionsMappings)
    	{
    		if(PressedInput == ActionMapping.Key)
    		{
    			OnCancel();
    			break;
    		}
    	}
    	
    }
}

float UQTEWidget::GetProgressBarCompletion() const
{
    return static_cast<float>(CurrentIndex + 1) / static_cast<float>(InputNumber);
}

void UQTEWidget::Start()
{
	bIsStarted = true;
}

void UQTEWidget::Stop()
{
	bIsStarted = false;
	TimeLeft = TimeLimit;
    CurrentIndex = 0;
	ParentInteractable->PlayerController->Widget = nullptr;
}

void UQTEWidget::OnComplete()
{
    ParentInteractable->EndInteraction(true, true);
}

void UQTEWidget::OnCancel()
{
    ParentInteractable->EndInteraction(false, false);
}

void UQTEWidget::OnFail()
{

    ParentInteractable->EndInteraction(false, true);
}
