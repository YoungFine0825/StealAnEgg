#include "SAEAbilitySystemComponent.h"

#include "Abilities/GameplayAbility.h"


void USAEAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if(Spec.IsActive())
	{
		for(const UGameplayAbility* Instance : Spec.GetAbilityInstances())
		{
			if(!IsValid(Instance) || !Instance->IsActive())
			{
				continue;
			}
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputPressed,
				Spec.Handle,
				Instance->GetCurrentActivationInfo().GetActivationPredictionKey()
				);
		}
	}
}

void USAEAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if(Spec.IsActive())
	{
		for(const UGameplayAbility* Instance : Spec.GetAbilityInstances())
		{
			if(!IsValid(Instance) || !Instance->IsActive())
			{
				continue;
			}
			InvokeReplicatedEvent(
				EAbilityGenericReplicatedEvent::InputReleased,
				Spec.Handle,
				Instance->GetCurrentActivationInfo().GetActivationPredictionKey()
				);
		}
	}
}

void USAEAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> Handles;

	for(const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if(Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTag(InputTag))
		{
			Handles.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
		if(!Spec)
		{
			continue;
		}
		if(!Spec->IsActive())
		{
			TryActivateAbility(Handle);
			Spec = FindAbilitySpecFromHandle(Handle);
		}
		if(Spec && Spec->IsActive())
		{
			AbilitySpecInputPressed(*Spec);
		}
	}
}

void USAEAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid())
	{
		return;
	}

	TArray<FGameplayAbilitySpecHandle> Handles;

	for(const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if(Spec.Ability && Spec.GetDynamicSpecSourceTags().HasTag(InputTag))
		{
			Handles.Add(Spec.Handle);
		}
	}

	for (const FGameplayAbilitySpecHandle& Handle : Handles)
	{
		if(FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle))
		{
			AbilitySpecInputReleased(*Spec);
		}
	}
}