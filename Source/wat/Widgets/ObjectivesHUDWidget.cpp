// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectivesHUDWidget.h"

#include "ImageUtils.h"
#include "ObjectiveHUDRowWidget.h"
#include "Brushes/SlateImageBrush.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/DataTable.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatObjectivesTableRow.h"
#include "wat/Core/WatPlayerController.h"
#include "wat/Core/WatPlayerCharacter.h"

void UObjectivesHUDWidget::UpdateObjectiveList()
{
	const auto Objectives = ObjectivesList->GetAllChildren();
	auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());

	const FString ContextString;
	TArray<FWatObjectivesTableRow*> Rows;
	GameState->ObjectiveDataTable->GetAllRows<FWatObjectivesTableRow>(ContextString, Rows);

	if(GameState->bScientistTurn == Parent->IsMechanist())
	{
		if(Objectives.Num())
		{
				const FVector2D ImageSize(1,1);
				auto SlateBrush = FSlateImageBrush("InteractionIcon", ImageSize);
				TArray<FColor> Colors;
				Colors.Add(FColor::White);
				SlateBrush.SetResourceObject(FImageUtils::CreateTexture2D(1,1, Colors, this, FString("StrikeTexture"), EObjectFlags::RF_NoFlags, FCreateTexture2DParameters()));
				Cast<UObjectiveHUDRowWidget>(ObjectivesList->GetChildAt(Objectives.Num() - 1))->ObjectiveDescription->SetStrikeBrush(SlateBrush);
		}
		
		auto* ObjectiveRow = CreateWidget<UObjectiveHUDRowWidget>(Controller, RowClass);
		ObjectiveRow->ObjectiveDescription->SetText(FText::FromString(TEXT("En attente de l'autre joueur")));
		ObjectivesList->AddChildToVerticalBox(ObjectiveRow);
	}
	else if(Objectives.Num() - 1 != GameState->ObjectiveIndex + GameState->bTutorialActivated)
	{
		if((Parent->IsMechanist() && Objectives.Num() > 1) || (!Parent->IsMechanist() && Objectives.Num() > 0))
		{
			ObjectivesList->RemoveChildAt(Objectives.Num() - 1);
		}
		
		for(auto& Row : Rows)
		{
			if(Row->Index == GameState->ObjectiveIndex && ((Parent->IsMechanist() && Row->PlayerClass == EPlayerClass::Mechanist) || (!Parent->IsMechanist() && Row->PlayerClass == EPlayerClass::Scientist) || Row->PlayerClass == EPlayerClass::Both))
			{
				auto* ObjectiveRow = CreateWidget<UObjectiveHUDRowWidget>(Controller, RowClass);
				ObjectiveRow->ObjectiveDescription->SetText(FText::FromString(Row->ObjectiveDesc));
				ObjectivesList->AddChildToVerticalBox(ObjectiveRow);
				break;
			}
		}
	}
}
