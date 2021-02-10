// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"
#include "Blueprint/WidgetTree.h"
#include "wat/Core/MainPlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "wat/Widgets/InteractableWidget.h"
#include "wat/Core/WatStaticUtils.h"

class AMainPlayerController;

void UQTEWidget::NativeConstruct()
{
	Super::NativeConstruct();

    TimeLeft = TimeLimit;
    bIsStarted = false;
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
	TArray<FKey> Keys;
    PossibleInputs.GetKeys(Keys);
	
    for (int i = 0; i < InputNumber; ++i)
    {
        InputList.Push(Keys[rand() % Keys.Num()]);
    }
	KeyIconWidget->SetBrushTexture(*PossibleInputs.Find(InputList[CurrentIndex]));   
}


void UQTEWidget::OnInput(FKey PressedInput)
{
    if (PressedInput == InputList[CurrentIndex])
    {
        if (CurrentIndex < InputList.Num()-1)
        {  	
            CurrentIndex++;
        	KeyIconWidget->SetBrushTexture(*PossibleInputs.Find(InputList[CurrentIndex])); 
        }
        else
        {
            OnComplete();
        }
    }
    else
    {
    	if(PressedInput == UWatStaticUtils::GetKeyForDevice("Return", ParentInteractable->PlayerController->bIsUsingController))
    	{
    		OnCancel();
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

	InputList.Empty();
	PossibleInputs.Empty();
	for(const auto ActionName : PossibleActionsName)
	{
		PossibleInputs.Add(UWatStaticUtils::GetKeyForDevice(ActionName, ParentInteractable->PlayerController->bIsUsingController), UWatStaticUtils::GetKeyIcon(ActionName, ParentInteractable->PlayerController->bIsUsingController));
	}
	DefineInputList();

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
