// Fill out your copyright notice in the Description page of Project Settings.


#include "StartChooserInteractable.h"
#include "Components/TextRenderComponent.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerCharacter.h"

void AStartChooserInteractable::OnInteraction()
{
	PlayerCharacter->SwitchStartMode();
}


void AStartChooserInteractable::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
}

void AStartChooserInteractable::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if(!GameState->bTutorialActivated)
	{
		TextRender->SetText(FText::FromString(QuickStartString));
	}
	else
	{
		TextRender->SetText(FText::FromString(TutorialString));
	}
}
