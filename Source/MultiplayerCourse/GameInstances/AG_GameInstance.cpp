// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstances/AG_GameInstance.h"

#include "AbilitySys/Components/AG_AbilitySystemComponentBase.h"
#include "AbilitySystemGlobals.h"

void UAG_GameInstance::Init()
{
	Super::Init();

	UAbilitySystemGlobals::Get().InitGlobalData();
}
