#include "SAEGameplayTags.h"

namespace SAEGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact,"Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_DropItem,"Input.DropItem");

	UE_DEFINE_GAMEPLAY_TAG(Event_Interaction_Activate,"Event.Interaction.Activate");//GA_Interact 用来激活目标 Ability 的事件
	UE_DEFINE_GAMEPLAY_TAG(Event_Interaction_PickupEgg,"Event.Interaction.PickupEgg");
	UE_DEFINE_GAMEPLAY_TAG(Event_Interaction_UI_OptionsChanged,"Event.Interaction.UI.OptionsChanged");
	UE_DEFINE_GAMEPLAY_TAG(Event_Interaction_UI_Hold,"Event.Interaction.UI.Hold");
	
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Pickup_StolenEgg,"Interaction.Pickup.StolenEgg");

	UE_DEFINE_GAMEPLAY_TAG(Ability_Interaction_PickupEgg,"Ability.Interaction.PickupEgg");
	UE_DEFINE_GAMEPLAY_TAG(Ability_Item_Drop,"Ability.Item.Drop");

	UE_DEFINE_GAMEPLAY_TAG(State_Interacting,"State.Interacting");
	UE_DEFINE_GAMEPLAY_TAG(State_Interacting_PickupEgg,"State.Interacting.PickupEgg");
	UE_DEFINE_GAMEPLAY_TAG(State_Movement_Locked,"State.Movement.Locked");
	UE_DEFINE_GAMEPLAY_TAG(State_Carrying_Item,"State.Carrying.Item");
	UE_DEFINE_GAMEPLAY_TAG(State_Carrying_StolenEgg,"State.Carrying.StolenEgg");
	UE_DEFINE_GAMEPLAY_TAG(State_World_Refreshing,"State.World.Refreshing");
	UE_DEFINE_GAMEPLAY_TAG(State_Dead,"State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(State_KnockedBack,"State.KnockedBack");
	UE_DEFINE_GAMEPLAY_TAG(State_Zone_Safe,"State.Zone.Safe");

	UE_DEFINE_GAMEPLAY_TAG(EquipmentSlot_Carry_Front,"EquipmentSlot.Carry.Front");
	UE_DEFINE_GAMEPLAY_TAG(EquipmentSlot_Hand_Right,"EquipmentSlot.Hand.Right");
	UE_DEFINE_GAMEPLAY_TAG(Pose_Carry_TwoHanded,"Pose.Carry.TwoHanded");
	UE_DEFINE_GAMEPLAY_TAG(Pose_Hold_OneHanded,"Pose.Hold.OneHanded");

	UE_DEFINE_GAMEPLAY_TAG(Pickup_StolenEgg,"Pickup.StolenEgg");

	UE_DEFINE_GAMEPLAY_TAG(UI_Interaction_OptionsChanged,"UI.Interaction.OptionsChanged");
	UE_DEFINE_GAMEPLAY_TAG(UI_Interaction_Hold,"UI.Interaction.Hold");
}
