// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AG_GameplayAbilityBase.generated.h"

class AMultiplayerCourseCharacter;
/**
 * 
 */
UCLASS()
class MULTIPLAYERCOURSE_API UAG_GameplayAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToApply;

	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TArray<TSubclassOf<UGameplayEffect>> OnGoingEffectsToRemove;

	TArray<FActiveGameplayEffectHandle> RemoveOnEndEffectHandles;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	AMultiplayerCourseCharacter* GetCharacterFromActorInfo() const;
};
