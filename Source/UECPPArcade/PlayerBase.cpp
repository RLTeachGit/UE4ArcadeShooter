// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerBase.h" //Need to be in this order
#include "LaserBase.h"
#include "Camera/CameraComponent.h"  //Needed for UCameraComponent
#include "Components/SphereComponent.h" //Needed for USphereComponent
#include "Components/InputComponent.h" //Needed for UInputComponent

#include "Engine/World.h" //For Debug

#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>

#include "LaserBase.h"

#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/ActorComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerBase::APlayerBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ColliderRoot = CreateDefaultSubobject<USphereComponent>(TEXT("Collider")); //Player will be a collider with a camera
	RootComponent = ColliderRoot; //Make the collider the root so collisions work
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCam")); //Add a Camera
	Camera->SetupAttachment(ColliderRoot); // Attach Camera to Collider
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

    FirePoint1 = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint1"));
    FirePoint1->SetupAttachment(ColliderRoot);
    FirePoint1->SetRelativeLocation(FVector(1.0f, -100, 0));
    FirePoint1->SetRelativeRotation(FRotator(0.0f, 0, 10.0f));

    FirePoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint2"));
    FirePoint2->SetupAttachment(ColliderRoot);
    FirePoint2->SetRelativeLocation(FVector(100.0f, 100, 0));
    FirePoint2->SetRelativeRotation(FRotator(0.0f, 0, -10.0f));


    // Set handling parameters
    Acceleration = 700.f;
    TurnSpeed = 70.f;
    MaxSpeed = 4000.f;
    MinSpeed = 500.f;
    CurrentForwardSpeed = 0;
}


void APlayerBase::NotifyHit(class UPrimitiveComponent* MyComp,
    class AActor* Other,
    class UPrimitiveComponent* OtherComp,
    bool bSelfMoved, FVector HitLocation,
    FVector HitNormal,
    FVector NormalImpulse, const FHitResult& Hit)
{
    Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

    // Deflect along the surface when we collide.
    FRotator CurrentRotation = GetActorRotation();
    SetActorRotation(FQuat::Slerp(CurrentRotation.Quaternion(), HitNormal.ToOrientationQuat(), 0.025f));
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
    const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaTime, 0.f, 0.f);

    // Move plan forwards (with sweep so we stop when we collide with things)
    AddActorLocalOffset(LocalMove, true);

    // Calculate change in rotation this frame
    FRotator DeltaRotation(0, 0, 0);
    DeltaRotation.Pitch = CurrentPitchSpeed * DeltaTime;
    DeltaRotation.Yaw = CurrentYawSpeed * DeltaTime;
    DeltaRotation.Roll = CurrentRollSpeed * DeltaTime;

    // Rotate plane
    AddActorLocalRotation(DeltaRotation);

    // Call any parent class Tick implementation
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	// Bind our control axis' to callback functions
	PlayerInputComponent->BindAxis("Thrust", this, &APlayerBase::ThrustInput); //Bind each input
	PlayerInputComponent->BindAxis("MoveUp", this, &APlayerBase::MoveUpInput);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerBase::MoveRightInput);

    PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerBase::Fire);
}


void APlayerBase::Fire()
{
    UWorld* const World = GetWorld();
    if (LaserBP!= NULL && World != NULL)
    {
        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        ALaserBase* tLaser1 = World->SpawnActor<ALaserBase>(LaserBP, FirePoint1->GetComponentLocation(), FirePoint1->GetComponentRotation(), ActorSpawnParams);
        ALaserBase* tLaser2 = World->SpawnActor<ALaserBase>(LaserBP, FirePoint2->GetComponentLocation(), FirePoint2->GetComponentRotation(), ActorSpawnParams);
        if (tLaser1 != NULL && tLaser2!=NULL)
        {
            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, TEXT("Laser Spawning"));

            if (LaserSound != NULL)
            {
                UGameplayStatics::PlaySoundAtLocation(this, LaserSound, GetActorLocation());
            }
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Laser failed to Spawn"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("Laser invalid"));
    }
}

void APlayerBase::MoveUpInput(float Val)
{
    // Target pitch speed is based in input
    float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

    // When steering, we decrease pitch slightly
    TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

    // Smoothly interpolate to target pitch speed
    CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void APlayerBase::MoveRightInput(float Val)
{
    // Target yaw speed is based on input
    float TargetYawSpeed = (Val * TurnSpeed);

    // Smoothly interpolate to target yaw speed
    CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

    // Is there any left/right input?
    const bool bIsTurning = FMath::Abs(Val) > 0.2f;

    // If turning, yaw value is used to influence roll
    // If not turning, roll to reverse current roll value.
    float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

    // Smoothly interpolate roll speed
    CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void APlayerBase::ThrustInput(float Val)
{
    // Is there any input?
    bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
    // If input is not held down, reduce speed
    float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
    // Calculate new speed
    float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
    // Clamp between MinSpeed and MaxSpeed
    CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}
