// Fill out your copyright notice in the Description page of Project Settings.


#include "Cage.h"


#include "wat/AI/MonsterCharacter.h"
#include "wat/Core/WatGameState.h"
#include "wat/Core/WatPlayerController.h"

// Sets default values
ACage::ACage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");

	CageMesh = CreateDefaultSubobject<UStaticMeshComponent>("Cage");
	CageMesh->SetupAttachment(RootComponent);
	
	LeftDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("LeftDoor");
	LeftDoorMesh->SetupAttachment(RootComponent);
	
	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("RightDoor");
	RightDoorMesh->SetupAttachment(RootComponent);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("BoxCollision");
	BoxCollision->SetupAttachment(RootComponent);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACage::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACage::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACage::BeginPlay()
{
	Super::BeginPlay();

	GameState = Cast<AWatGameState>(GetWorld()->GetGameState());
	
	InitialZ = RightDoorMesh->GetRelativeLocation().Z;
	bShouldMove = false;
	bIsDown = false;
	AccumulatedDeltaTime = 0.f;
}

void ACage::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->IsA(AMonsterCharacter::StaticClass()))
	{
		bIsOverlappingWithMonster = true;
	}
}

void ACage::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(OtherActor->IsA(AMonsterCharacter::StaticClass()))
	{
		bIsOverlappingWithMonster = false;
	}
}

// Called every frame
void ACage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//DOWN
	if(bShouldMove)
	{
		if(LeftDoorMesh->GetRelativeLocation().Z > 0.f)
		{
			LeftDoorMesh->AddRelativeLocation(GetActorUpVector() * -1.f * DoorMovingSpeed * DeltaTime);
		}
		if(RightDoorMesh->GetRelativeLocation().Z > 0.f)
		{
			RightDoorMesh->AddRelativeLocation(GetActorUpVector() * -1.f * DoorMovingSpeed * DeltaTime);
		}

		if(LeftDoorMesh->GetRelativeLocation().Z <= 0.f)
		{
			auto RelativeLocation = LeftDoorMesh->GetRelativeLocation();
			RelativeLocation.Z = 0;
			LeftDoorMesh->SetRelativeLocation(RelativeLocation);
			bShouldMove = false;
			bIsDown = true;
			GameState->Gas->GetRootComponent()->SetVisibility(true);
		}
		if(RightDoorMesh->GetRelativeLocation().Z <= 0.f)
		{
			auto RelativeLocation = RightDoorMesh->GetRelativeLocation();
			RelativeLocation.Z = 0;
			RightDoorMesh->SetRelativeLocation(RelativeLocation);
			bShouldMove = false;
			bIsDown = true;
			GameState->Gas->GetRootComponent()->SetVisibility(true);
		}
	}
	else if(bIsDown && !bIsOverlappingWithMonster && !GameState->bGameEnded)
	{
		//UP
		if(LeftDoorMesh->GetRelativeLocation().Z < InitialZ)
		{
			LeftDoorMesh->AddRelativeLocation(GetActorUpVector() * DoorMovingSpeed * DeltaTime);
		}
		if(RightDoorMesh->GetRelativeLocation().Z < InitialZ)
		{
			RightDoorMesh->AddRelativeLocation(GetActorUpVector() * DoorMovingSpeed * DeltaTime);
		}

		if(LeftDoorMesh->GetRelativeLocation().Z >= InitialZ)
		{
			auto RelativeLocation = LeftDoorMesh->GetRelativeLocation();
			RelativeLocation.Z = InitialZ;
			LeftDoorMesh->SetRelativeLocation(RelativeLocation);
			bIsDown = false;
		}
		if(RightDoorMesh->GetRelativeLocation().Z >= InitialZ)
		{
			auto RelativeLocation = RightDoorMesh->GetRelativeLocation();
			RelativeLocation.Z = InitialZ;
			RightDoorMesh->SetRelativeLocation(RelativeLocation);
			bIsDown = false;
		}
	}
	else if(bIsDown && bIsOverlappingWithMonster)
	{
		AccumulatedDeltaTime += DeltaTime;
		if(AccumulatedDeltaTime >= MonsterDeathCooldown)
		{
			GEngine->AddOnScreenDebugMessage(-1,0.f,FColor::Emerald, "THE MOTHERF*CKER IS DEAD");
			auto MaxIndex = 0;
			AObjective* MaxObjective = nullptr;
			for(auto* Objective : GameState->Objectives)
			{
				if(Objective && Objective->ObjectiveIndex >= MaxIndex)
				{
					MaxIndex = Objective->ObjectiveIndex;
					MaxObjective = Objective;
				}
			}
			MaxObjective->bCompleted =true;
			Cast<AWatPlayerController>(GetWorld()->GetFirstPlayerController())->NotifyObjectiveCompleted(MaxObjective);
		}
	}
}

void ACage::CloseDoors()
{
	bShouldMove = true;
}

void ACage::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(ACage, bShouldMove);
}

