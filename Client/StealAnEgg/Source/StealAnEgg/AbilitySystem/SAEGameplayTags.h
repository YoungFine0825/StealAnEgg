#pragma once

#include "NativeGameplayTags.h"

namespace SAEGameplayTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_Interact);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Input_DropItem);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_Activate);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_PickupEgg);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_UI_OptionsChanged);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_Interaction_UI_Hold);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Interaction_Pickup_StolenEgg);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Interaction_PickupEgg);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Item_Drop);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Interacting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Interacting_PickupEgg);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Locked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Carrying_Item);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Carrying_StolenEgg);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_World_Refreshing);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_KnockedBack);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Zone_Safe);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_Carry_Front);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(EquipmentSlot_Hand_Right);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pose_Carry_TwoHanded);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pose_Hold_OneHanded);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Pickup_StolenEgg);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Interaction_OptionsChanged);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(UI_Interaction_Hold);
	
};
