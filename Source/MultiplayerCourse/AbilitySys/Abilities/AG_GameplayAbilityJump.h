// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySys/Abilities/AG_GameplayAbilityBase.h"
#include "AG_GameplayAbilityJump.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERCOURSE_API UAG_GameplayAbilityJump : public UAG_GameplayAbilityBase
{
	GENERATED_BODY()
	
	UAG_GameplayAbilityJump();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> JumpEffect;
};
