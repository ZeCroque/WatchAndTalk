// Fill out your copyright notice in the Description page of Project Settings.


#include "BunkerTriggerBox.h"


#include "wat/AI/GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerController.h"

// Sets default values
ABunkerTriggerBox::ABunkerTriggerBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	HitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetHiddenInGame(true);
	HitBox->SetBoundsScale(1000);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ABunkerTriggerBox::OnOverlapBegin);
	HitBox->OnComponentEndOverlap.AddDynamic(this, &ABunkerTriggerBox::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ABunkerTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	Timer = CoolDown;
}

// Called every frame
void ABunkerTriggerBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABunkerTriggerBox::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	const auto Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsLocallyControlled())
	{
		Cast<AWatPlayerController>(Pawn->GetController())->UpdateBunkerPlayerCount(1);
	}
}

void ABunkerTriggerBox::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	const auto Pawn = Cast<APawn>(OtherActor);
	if (Pawn && Pawn->IsLocallyControlled())
	{
		Cast<AWatPlayerController>(Pawn->GetController())->UpdateBunkerPlayerCount(-1);
	}
}
