// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySys/Abilities/AG_GameplayAbilityBase.h"
#include "AG_GameplayAbilityThrowBullet.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCOURSE_API UAG_GameplayAbilityThrowBullet : public UAG_GameplayAbilityBase
{
	GENERATED_BODY()

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
