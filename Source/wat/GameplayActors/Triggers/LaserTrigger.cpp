// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserTrigger.h"
#include "wat/Core/WatPlayerCharacter.h"

ALaserTrigger::ALaserTrigger()
{
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALaserTrigger::OnOverlapBegin);
    CollisionBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ALaserTrigger::OnOverlapEnd);
    CollisionBoxComponent->SetupAttachment(RootComponent);
}

void ALaserTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    auto* PlayerCharacter = Cast<AWatPlayerCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->SetIsUsingLaser(false);
    }
}

void ALaserTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    auto* PlayerCharacter = Cast<AWatPlayerCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->SetIsUsingLaser(true);
    }
}