// Fill out your copyright notice in the Description page of Project Settings.


#include "BackableWidget.h"

#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatStaticUtils.h"

FReply UBackableWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    AWatPlayerController* PC = Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController());
    if (InKeyEvent.GetKey() == UWatStaticUtils::GetKeyForDevice("BackMenu" ,InKeyEvent.GetKey().IsGamepadKey()))
    {
        CloseWidget();
        return FReply::Handled();
    }
    return FReply::Unhandled();
}