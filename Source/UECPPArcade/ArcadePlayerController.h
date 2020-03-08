// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ArcadePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UECPPARCADE_API AArcadePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	//Widget
	// Reference UMG Asset in the Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wPlayerHUD;

	// So we can acccess it
	UUserWidget* PlayerHUDRef;

	// Override BeginPlay()
	virtual void BeginPlay() override;




};
