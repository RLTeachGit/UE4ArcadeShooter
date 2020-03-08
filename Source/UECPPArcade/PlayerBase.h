// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerBase.generated.h"

UCLASS()
class UECPPARCADE_API APlayerBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


//Non Boilerplate code below
public:
    virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* ColliderRoot;

	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

//Callbacks for movement
protected:
	void MoveUpInput(float Val);

	void MoveRightInput(float Val);

	void ThrustInput(float Val);

private:

    /** How quickly forward speed changes */
    UPROPERTY(Category = Plane, EditAnywhere)
        float Acceleration;

    /** How quickly pawn can steer */
    UPROPERTY(Category = Plane, EditAnywhere)
        float TurnSpeed;

    /** Max forward speed */
    UPROPERTY(Category = Pitch, EditAnywhere)
        float MaxSpeed;

    /** Min forward speed */
    UPROPERTY(Category = Yaw, EditAnywhere)
        float MinSpeed;

    /** Current forward speed */
    float CurrentForwardSpeed;

    /** Current yaw speed */
    float CurrentYawSpeed;

    /** Current pitch speed */
    float CurrentPitchSpeed;

    /** Current roll speed */
    float CurrentRollSpeed;
};
