// Fill out your copyright notice in the Description page of Project Settings.


#include "TutoSoundTriggerBox.h"

#include "wat/Core/WatPlayerCharacter.h"

void ATutoSoundTriggerBox::OnTriggerStart(AActor* OverlappingActor)
{
    if (OverlappingActor->GetClass() == Cast<AWatPlayerCharacter>(OverlappingActor)->GetClass())
    {
        GEngine->AddOnScreenDebugMessage(1,1,FColor::Green, "lsdqfhqskdhfjqmsdfq");
    }
}
