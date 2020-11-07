// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Interactable.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(RootComponent);
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	auto* Mesh3P = this->GetMesh();
	Mesh3P->AddRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	Mesh3P->AddRelativeLocation(FVector(-30.0f, 10.0f, -90.0f));
	Mesh3P->SetVisibleFlag(false);

}

bool APlayerCharacter::AddInteractable(AInteractable* Interactable)
{
	InteractablesSet.Add(Interactable);
	return IsClosestInteractable(Interactable);
}

void APlayerCharacter::RemoveInteractable(AInteractable* Interactable)
{
	InteractablesSet.Remove(Interactable);
}

AInteractable* APlayerCharacter::GetClosestInteractable()
{
	const FVector PlayerLocation = GetActorLocation();
	TPair<float, AInteractable*> ClosestInteractable(-1.0f, nullptr);

	for (auto InteractableSetItem : InteractablesSet)
	{
		if (InteractableSetItem)
		{
			const float Distance = UKismetMathLibrary::Vector_Distance2D(PlayerLocation, InteractableSetItem->GetActorLocation());
			if (ClosestInteractable.Key == -1.0f || ClosestInteractable.Key > Distance)
			{
				ClosestInteractable.Key = Distance;
				ClosestInteractable.Value = InteractableSetItem;
			}
		}
	}
	return ClosestInteractable.Value;
}

bool APlayerCharacter::IsClosestInteractable(AInteractable* Interactable)
{
	
	if (GetClosestInteractable() == Interactable)
	{
		return true;
	}
	
	return false;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::Interact);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APlayerCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APlayerCharacter::LookUpAtRate);

}

void APlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APlayerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::Interact()
{
	auto* ClosestInteractable = GetClosestInteractable();
	if(ClosestInteractable)
	{
		ClosestInteractable->Interact();
	}
}

