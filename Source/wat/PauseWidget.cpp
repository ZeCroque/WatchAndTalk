// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseWidget.h"
#include "wat/Core/WatPlayerController.h"

void UPauseWidget::OnInput(FKey PressedInput)
{
    GEngine->AddOnScreenDebugMessage(1,1,FColor::Green, FString::Printf(TEXT("%s"), *PressedInput.ToString()));
    if (PressedInput.ToString() == "Escape" || PressedInput.ToString()=="X")
    {
	    AWatPlayerController* PC = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
        PC->PauseWidget->RemoveFromParent();
        PC->PauseWidget = nullptr;
        PC->bShowMouseCursor = false;
    }
}
