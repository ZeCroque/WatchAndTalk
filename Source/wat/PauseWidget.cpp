// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseWidget.h"

#include "Core/WatStaticUtils.h"
#include "wat/Core/WatPlayerController.h"

void UPauseWidget::DestroyWidget()
{
    AWatPlayerController* PC = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
    PC->PauseWidget->RemoveFromParent();
    PC->PauseWidget = nullptr;
    PC->bShowMouseCursor = false;
    const FInputModeGameOnly GameOnly;
    PC->SetInputMode(GameOnly);
}

FReply UPauseWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    AWatPlayerController* PC = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
    if (InKeyEvent.GetKey() == UWatStaticUtils::GetKeyForDevice("Pause" ,InKeyEvent.GetKey().IsGamepadKey()))
    {
        DestroyWidget();
        return FReply::Handled();
    }
    return FReply::Unhandled();
}
