// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectivesHUDWidget.h"
#include "ObjectiveHUDRowWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "wat/Core/WatPlayerController.h"

void UObjectivesHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto* ObjectiveRow = CreateWidget<UObjectiveHUDRowWidget>(Controller, RowClass);
	const auto TestString = FString("Test");
	ObjectiveRow->ObjectiveDescription->SetText(FText::FromString(TestString));
	ObjectivesList->AddChildToVerticalBox(ObjectiveRow);
}
