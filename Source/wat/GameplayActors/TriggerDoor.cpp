// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerDoor.h"
#include "Characters/MonsterCharacter.h"
#include "Core/MainGameState.h"
#include "Characters/PlayerCharacter.h"

// Sets default values
ATriggerDoor::ATriggerDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	BoxComponent->SetupAttachment(RootComponent);
	BoxComponent->SetHiddenInGame(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ATriggerDoor::OnOverlapBegin);
	BoxComponent->SetRelativeScale3D(FVector(3.0f, 0.5f, 5.0f));
}

// Called when the game starts or when spawned
void ATriggerDoor::BeginPlay()
{
	Super::BeginPlay();
	GameState = Cast<AMainGameState>(GetWorld()->GetGameState());
}

// Called every frame
void ATriggerDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATriggerDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult)
{
	if (OtherActor == GameState->MonsterCharacter->PlayerToChase)
	{
		GameState->MonsterCharacter->StopGoalReaction();
	}
}
