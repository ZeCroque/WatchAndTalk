// Fill out your copyright notice in the Description page of Project Settings.


#include "WatTutoLevelScriptActor.h"


#include "WatGameInstance.h"
#include "WatGameState.h"

void AWatTutoLevelScriptActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if(HasAuthority())
	{
	    auto* GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	    if(GameState)
	    {
	    	GameState->bIsInTutorialMode = true;
	    }

		auto* GameInstance = Cast<UWatGameInstance>(GetWorld()->GetGameInstance());
		if(GameInstance)
		{
			GameInstance->bIsInTutorialMode = true;
		}
	}
}
