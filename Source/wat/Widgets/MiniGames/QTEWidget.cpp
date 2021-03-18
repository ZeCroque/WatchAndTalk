// Fill out your copyright notice in the Description page of Project Settings.


#include "QTEWidget.h"
#include "Blueprint/WidgetTree.h"
#include "wat/Core/WatPlayerController.h"
#include "GameFramework/InputSettings.h"
#include "GenericPlatform/GenericPlatformMisc.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Widgets/KeyIconWidget.h"
#include "wat/Core/WatStaticUtils.h"

bool UQTEWidget::OnInput(FKey PressedInput, EInputEvent EventType)
{
	if(!Super::OnInput(PressedInput,EventType) && EventType == IE_Pressed)
	{
	    if (PressedInput == InputList[CurrentIndex])
	    {
	        if (CurrentIndex < InputList.Num()-1)
	        {  	
	            CurrentIndex++;
        		KeyIconWidget->SetBrushTexture(*PossibleInputs.Find(InputList[CurrentIndex]));
	        	UGameplayStatics::SpawnSoundAtLocation(ParentInteractable, KeySound, ParentInteractable->GetActorLocation());
	        }
	        else
	        {
	            OnComplete();
	        }
	    }
	    else
	    {
    		OnFail();
	    }
	}
	return true;
}

void UQTEWidget::Start()
{
    Super::Start();

	InputList.Empty();
	PossibleInputs.Empty();
	for(const auto ActionName : PossibleActionsName)
	{
		PossibleInputs.Add(UWatStaticUtils::GetKeyForDevice(ActionName, ParentInteractable->PlayerController->bIsUsingController), UWatStaticUtils::GetKeyIcon(ActionName, ParentInteractable->PlayerController->bIsUsingController));
	}
	DefineInputList();
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

float UQTEWidget::GetProgressBarCompletion() const
{
    return static_cast<float>(CurrentIndex + 1) / static_cast<float>(InputNumber);
}

