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
	
	if(Objectives.Num() == 0 || Objectives.Num() != GameState->ObjectiveIndex + GameState->bTutorialActivated)
	{
		for(int i = 0; i < Objectives.Num(); ++i)
		{
			auto* Row = Cast<UObjectiveHUDRowWidget>(Objectives[i]);

			if(!Row->ObjectiveDescription->StrikeBrush.GetResourceObject())
			{
				const FVector2D ImageSize(1,1);
				auto SlateBrush = FSlateImageBrush("InteractionIcon", ImageSize);
				TArray<FColor> Colors;
				Colors.Add(FColor::White);
				SlateBrush.SetResourceObject(FImageUtils::CreateTexture2D(1,1, Colors, this, FString("StrikeTexture") + FString::FromInt(i), EObjectFlags::RF_NoFlags, FCreateTexture2DParameters()));
				Row->ObjectiveDescription->SetStrikeBrush(SlateBrush);
			}
		}

		if(GameState->ObjectiveIndex < Rows.Num())
		{
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
}
