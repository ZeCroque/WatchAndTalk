// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GridClearComponent.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "AI/GridManager.h"
#include "Containers/Array.h"
#include "Core/MainGameState.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UGridClearComponent::UGridClearComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGridClearComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
	Timer = Cooldown;
}


// Called every frame
void UGridClearComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GridManager)
	{
		GridManager = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()))->GridManager;
	}
	
	Timer += DeltaTime;
	if (Timer >= Cooldown && GridManager)
	{
		Timer = 0;

		auto const Start = Owner->GetActorLocation() + (FVector::UpVector * ViewUpOffset) + (Owner->GetActorForwardVector() *
			ViewForwardOffset);

		TArray<FVector> Polygon;
		Polygon.Add(Start);
		for (int i = 0; i < RayNumber; ++i)
		{
			const float Angle = (FieldOfView / 2) + (i * (FieldOfView / RayNumber)) - FieldOfView;

			FRotator Rotator = FRotator(GetComponentRotation().Pitch * -1,
			                            GetComponentRotation().Yaw + Angle,
			                            0);
			FVector Direction = Rotator.Vector() * RaySize * 2;
			auto const End = Start + Direction;

			if (bDebug)
			{
				DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, Cooldown, 0, 1);
			}

			FHitResult Result;
			GetWorld()->LineTraceSingleByChannel(Result, Start, End, ECC_Visibility);

			if (Result.bBlockingHit)
			{
				Polygon.Add(Result.Location);
				if (bDebug)
				{
					DrawDebugSphere(GetWorld(), Result.Location, 20, 16, FColor::Purple, false, Cooldown, 0, 1);
				}
			}
		}

		GridManager->CleanPolygon(Polygon);
	}
}
