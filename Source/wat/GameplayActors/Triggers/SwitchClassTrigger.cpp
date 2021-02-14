// Fill out your copyright notice in the Description page of Project Settings.


#include "SwitchClassTrigger.h"

#include "Components/BoxComponent.h"
#include "wat/Core/WatPlayerCharacter.h"

// Sets default values
ASwitchClassTrigger::ASwitchClassTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASwitchClassTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ASwitchClassTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASwitchClassTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto* PlayerCharacter = Cast<AWatPlayerCharacter>(OtherActor);
	if(PlayerCharacter)
	{
		PlayerCharacter->SwitchPlayersClasses();
	}
}

// Called every frame
void ASwitchClassTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

