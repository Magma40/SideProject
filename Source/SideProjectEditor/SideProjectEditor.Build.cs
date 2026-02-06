using UnrealBuildTool;

public class SideProjectEditor : ModuleRules
{
	public SideProjectEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		// All dependencies in a single AddRange call
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"SideProject",   // your runtime module
			"UnrealEd",
			"EditorStyle",
			"Slate",
			"SlateCore"
		});
	}
}