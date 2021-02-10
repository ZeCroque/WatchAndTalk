// Fill out your copyright notice in the Description page of Project Settings.


#include "wat/GameplayActors/Triggers//GridTrigger.h"

#include "DrawDebugHelpers.h"
#include "wat/Core//WatPlayerCharacter.h"
#include "wat/Core/WatGameState.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGridTrigger::AGridTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));

	HitBox = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HitBox"));
	HitBox->SetupAttachment(RootComponent);
	HitBox->SetHiddenInGame(true);
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &AGridTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AGridTrigger::BeginPlay()
{
	Super::BeginPlay();

	FHitResult HitResult;
	auto const ActorLocation = GetActorLocation();
	auto const bHaveHit = GetWorld()->LineTraceSingleByChannel(HitResult, GetActorLocation(),
	                                                          FVector(ActorLocation.X, ActorLocation.Y,
	                                                                  ActorLocation.Z - 1000), ECC_Visibility);

	if (bHaveHit)
	{
		Origin = FVector(ActorLocation.X, ActorLocation.Y, HitResult.ImpactPoint.Z);
	}
	else
	{
		Origin = GetActorLocation();
	}

	if (bDebug)
	{
		DrawDebugCircle(GetWorld(), Origin, DistanceFromBox, 64, FColor::Yellow,
		                true, -1, 0, 3, FVector(1.f, 0.f, 0.f), FVector(0.f, 1.f, 0.f));
	}

	Timer = CoolDown;
}

// Called every frame
void AGridTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Timer < CoolDown)
	{
		Timer += DeltaTime;
	}
}

void AGridTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                     const FHitResult& SweepResult)
{
	if (Timer >= CoolDown)
	{
		Timer = 0;
		if (!GridManager)
		{
			GridManager = Cast<AWatGameState>(UGameplayStatics::GetGameState(GetWorld()))->GridManager;
		}

		if (OtherActor->IsA(AWatPlayerCharacter::StaticClass()) && GridManager)
		{
			auto const Center = GetActorLocation() + (SweepResult.Normal * DistanceFromBox);
			auto const FinalCenter = FVector(Center.X, Center.Y, Origin.Z);
			GridManager->SetValueAround(FinalCenter, GridDrawRay);

			if (bDebug)
			{
				DrawDebugSphere(GetWorld(), FinalCenter, 20, 16, FColor::Green, false, 5, 0, 1);
			}
		}
	}
}
