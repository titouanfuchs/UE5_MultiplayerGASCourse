// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySys/Abilities/AG_GameplayAbilityJump.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

UAG_GameplayAbilityJump::UAG_GameplayAbilityJump()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UAG_GameplayAbilityJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	
	return Character->CanJump();
}

void UAG_GameplayAbilityJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		return;
	}

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	
	Character->Jump();

	if (UAbilitySystemComponent* AbilityComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		FGameplayEffectContextHandle EffectContext = AbilityComponent->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = AbilityComponent->MakeOutgoingSpec(JumpEffect, 1, EffectContext);

		if (!SpecHandle.IsValid()){return;}

		FActiveGameplayEffectHandle ActiveGEHandle = AbilityComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		if (!ActiveGEHandle.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to apply JumpEffect to %s"), *GetNameSafe(Character));
		}
	}
}
