#pragma once

/*
 * 可交互目标接口
 * 让交互扫描器只依赖“可交互”的对象
 * 交互时间放在目标配置中而不是硬编码到Ability。普通蛋可以是2秒，其他高品质蛋可以使用其他时长。
 */

#include "CoreMinimal.h"
#include "SAEInteractionOption.h"
#include "UObject/Interface.h"
#include "SAEInteractableTarget.generated.h"

class ASAEPlayerCharacter;
class ISAEInteractableTarget;
struct FGameplayEventData;


struct FSAEInteractionOptionBuilder
{
	FSAEInteractionOptionBuilder(
		TScriptInterface<ISAEInteractableTarget> InScope,
		TArray<FSAEInteractionOption>& InOptions) : Scope(InScope), Options(InOptions)
	{
		
	}

	void AddOption(const FSAEInteractionOption& Option)
	{
		FSAEInteractionOption& Added = Options.Add_GetRef(Option);
		Added.Target = Scope;
	}
private:
	TScriptInterface<ISAEInteractableTarget> Scope;

	TArray<FSAEInteractionOption>& Options;
};

UINTERFACE(BlueprintType,meta=(CannotImplementInterfaceInBlueprint))
class STEALANEGG_API USAEInteractableTarget : public UInterface
{
	GENERATED_BODY()
};

class STEALANEGG_API ISAEInteractableTarget
{
	GENERATED_BODY()

public:

	virtual void GatherInteractionOptions(const ASAEPlayerCharacter* Character,FSAEInteractionOptionBuilder& Builder) const = 0;
	
	virtual bool CanInteract(const ASAEPlayerCharacter* Character,FText& OutFailureReason) const = 0;//纯虚函数

	virtual float GetInteractionDuration() const
	{
		return 2.0f;
	}

	virtual void CustomizeInteractionEventData(const FGameplayTag& EventTag,FGameplayEventData& InOutEventData) const
	{
		
	}
};