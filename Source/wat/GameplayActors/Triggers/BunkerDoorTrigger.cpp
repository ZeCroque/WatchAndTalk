// Fill out your copyright notice in the Description page of Project Settings.


#include "BunkerDoorTrigger.h"
#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerCharacter.h"

// Sets default values
ABunkerDoorTrigger::ABunkerDoorTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetHiddenInGame(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABunkerDoorTrigger::OnOverlapBegin);
	BoxComponent->SetRelativeScale3D(FVector(3.0f, 0.5f, 5.0f));
}

// Called when the game starts or when spawned
void ABunkerDoorTrigger::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
}

// Called every frame
void ABunkerDoorTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABunkerDoorTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (OtherActor == GameState->MonsterCharacter->PlayerToChase)
	{
		GameState->MonsterCharacter->StopGoalReaction();
	}
}
