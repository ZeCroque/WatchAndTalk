// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserInteractable.h"

#include "Characters/PlayerCharacter.h"

void ALaserInteractable::OnInteraction()
{
    PlayerCharacter->bIsUsingLaser = true;
}

void ALaserInteractable::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (PlayerCharacter)
    {
        PlayerCharacter->bIsUsingLaser = false;
    }
}
