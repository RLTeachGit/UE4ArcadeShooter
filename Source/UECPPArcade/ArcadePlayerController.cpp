// Fill out your copyright notice in the Description page of Project Settings.

#include "ArcadePlayerController.h"

#include "Blueprint/UserWidget.h"

void AArcadePlayerController::BeginPlay()
{
	Super::BeginPlay();
    
    if (IsValid(wPlayerHUD)) //Make sure HUD is valid
    {
        PlayerHUDRef = CreateWidget<UUserWidget>(this, wPlayerHUD); //Create a Widget
        if (IsValid(PlayerHUDRef))
        {
            PlayerHUDRef->AddToViewport();  //Add to VP
        }
    }
}

