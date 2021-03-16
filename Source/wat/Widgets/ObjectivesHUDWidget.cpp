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
				const auto ImageSize =FVector2D(StrikeTexture->GetSizeX(), StrikeTexture->GetSizeY());
				auto SlateBrush = FSlateImageBrush("InteractionIcon", ImageSize);

				SlateBrush.SetResourceObject(StrikeTexture);
				Cast<UObjectiveHUDRowWidget>(ObjectivesList->GetChildAt(Objectives.Num() - 1))->ObjectiveDescription->SetStrikeBrush(SlateBrush);

				if(IsInViewport())
				{
					RemoveFromParent();
					AddToViewport();
				}
		}
		
		auto* ObjectiveRow = CreateWidget<UObjectiveHUDRowWidget>(Controller, RowClass);
		ObjectiveRow->ObjectiveDescription->SetText(FText::FromString(TEXT("En attente de l'autre joueur")));
		ObjectivesList->AddChildToVerticalBox(ObjectiveRow);
	}
	else if(Objectives.Num() - 1 != GameState->ObjectiveIndex + GameState->bIsInTutorialMode)
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
