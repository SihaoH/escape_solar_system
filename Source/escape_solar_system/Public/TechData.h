// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TechData.generated.h"


USTRUCT(BlueprintType)
struct FTechDemand
{
	GENERATED_USTRUCT_BODY()

	/** 升级所需的点数 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Points = 0;

	/** 升级所需的物品 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, int32> Items;
};

USTRUCT(BlueprintType)
struct FTechDemandList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FTechDemand> List = { FTechDemand(), FTechDemand(), FTechDemand(), FTechDemand(), FTechDemand() };
};

USTRUCT(BlueprintType)
struct FTechValueList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> List = { 0, 0, 0, 0, 0 };
};

UENUM(BlueprintType)
enum class ETech : uint8
{
	None,

	CharHP,
	CharMass,
	CharBackpack,
	CharShieldCold,
	CharShieldHeat,
	CharShieldPress,
	CharEnginePower,
	CharEngineMass,
	CharEngineEPR,
	CharEngineEMR,
	CharEnergy,

	ShipHP,
	ShipMass,
	ShipBackpack,
	ShipShieldCold,
	ShipShieldHeat,
	ShipShieldPress,
	ShipEngine0Power,
	ShipEngine0Mass,
	ShipEngine0EPR,
	ShipEngine0EMR,
	ShipEnergy0,
	ShipEngine1Power,
	ShipEngine1Mass,
	ShipEngine1EPR,
	ShipEngine1EMR,
	ShipEnergy1,
	ShipEngine2Power,
	ShipEngine2Mass,
	ShipEngine2EPR,
	ShipEngine2EMR,
	ShipEnergy2,
};

namespace TECH
{
const TMap<ETech, FName> ValueRow = {
	{ ETech::CharHP,          "CharHP"          },
	{ ETech::CharMass,        "CharMass"        },
	{ ETech::CharBackpack,    "CharBackpack"    },
	{ ETech::CharShieldCold,  "CharShieldCold"  },
	{ ETech::CharShieldHeat,  "CharShieldHeat"  },
	{ ETech::CharShieldPress, "CharShieldPress" },
	{ ETech::CharEnginePower, "CharEnginePower" },
	{ ETech::CharEngineMass,  "CharEngineMass"  },
	{ ETech::CharEngineEPR,   "CharEngineEPR"   },
	{ ETech::CharEngineEMR,   "CharEngineEMR"   },
	{ ETech::CharEnergy,      "CharEnergy"      },

	{ ETech::ShipHP,           "ShipHP"          },
	{ ETech::ShipMass,         "ShipMass"        },
	{ ETech::ShipBackpack,     "ShipBackpack"    },
	{ ETech::ShipShieldCold,   "ShipShieldCold"  },
	{ ETech::ShipShieldHeat,   "ShipShieldHeat"  },
	{ ETech::ShipShieldPress,  "ShipShieldPress" },
	{ ETech::ShipEngine0Power, "ShipEngine0Power"},
	{ ETech::ShipEngine0Mass,  "ShipEngine0Mass" },
	{ ETech::ShipEngine0EPR,   "ShipEngine0EPR"  },
	{ ETech::ShipEngine0EMR,   "ShipEngine0EMR"  },
	{ ETech::ShipEngine1Power, "ShipEngine1Power"},
	{ ETech::ShipEngine1Mass,  "ShipEngine1Mass" },
	{ ETech::ShipEngine1EPR,   "ShipEngine1EPR"  },
	{ ETech::ShipEngine1EMR,   "ShipEngine1EMR"  },
	{ ETech::ShipEngine2Power, "ShipEngine2Power"},
	{ ETech::ShipEngine2Mass,  "ShipEngine2Mass" },
	{ ETech::ShipEngine2EPR,   "ShipEngine2EPR"  },
	{ ETech::ShipEngine2EMR,   "ShipEngine2EMR"  },
	{ ETech::ShipEnergy0,      "ShipEnergy0"     },
	{ ETech::ShipEnergy1,      "ShipEnergy1"     },
	{ ETech::ShipEnergy2,      "ShipEnergy2"     },
};

const TMap<ETech, FName> DemandRow = {
	{ ETech::CharHP,          "CharStrength"    },
	{ ETech::CharMass,        "CharStrength"    },
	{ ETech::CharBackpack,    "CharBackpack"    },
	{ ETech::CharShieldCold,  "CharShieldCold"  },
	{ ETech::CharShieldHeat,  "CharShieldHeat"  },
	{ ETech::CharShieldPress, "CharShieldPress" },
	{ ETech::CharEnginePower, "CharEngine"      },
	{ ETech::CharEngineMass,  "CharEngine"      },
	{ ETech::CharEngineEPR,   "CharEngine"      },
	{ ETech::CharEngineEMR,   "CharEngine"      },
	{ ETech::CharEnergy,      "CharEnergy"      },

	{ ETech::ShipHP,           "ShipStrength"   },
	{ ETech::ShipMass,         "ShipStrength"   },
	{ ETech::ShipBackpack,     "ShipBackpack"   },
	{ ETech::ShipShieldCold,   "ShipShieldCold" },
	{ ETech::ShipShieldHeat,   "ShipShieldHeat" },
	{ ETech::ShipShieldPress,  "ShipShieldPress"},
	{ ETech::ShipEngine0Power, "ShipEngine0"    },
	{ ETech::ShipEngine0Mass,  "ShipEngine0"    },
	{ ETech::ShipEngine0EPR,   "ShipEngine0"    },
	{ ETech::ShipEngine0EMR,   "ShipEngine0"    },
	{ ETech::ShipEngine1Power, "ShipEngine1"    },
	{ ETech::ShipEngine1Mass,  "ShipEngine1"    },
	{ ETech::ShipEngine1EPR,   "ShipEngine1"    },
	{ ETech::ShipEngine1EMR,   "ShipEngine1"    },
	{ ETech::ShipEngine2Power, "ShipEngine2"    },
	{ ETech::ShipEngine2Mass,  "ShipEngine2"    },
	{ ETech::ShipEngine2EPR,   "ShipEngine2"    },
	{ ETech::ShipEngine2EMR,   "ShipEngine2"    },
	{ ETech::ShipEnergy0,      "ShipEnergy0"    },
	{ ETech::ShipEnergy1,      "ShipEnergy1"    },
	{ ETech::ShipEnergy2,      "ShipEnergy2"    },
};

#define LOCTEXT_NAMESPACE "LevelData"
const TMap<ETech, FText> DispName = {
	{ ETech::CharHP,          LOCTEXT("CharHP",          "血量")   },
	{ ETech::CharMass,        LOCTEXT("CharMass",        "质量")   },
	{ ETech::CharBackpack,    LOCTEXT("CharBackpack",    "承重")   },
	{ ETech::CharShieldCold,  LOCTEXT("CharShieldCold",  "耐寒")   },
	{ ETech::CharShieldHeat,  LOCTEXT("CharShieldHeat",  "耐热")   },
	{ ETech::CharShieldPress, LOCTEXT("CharShieldPress", "耐压")   },
	{ ETech::CharEnginePower, LOCTEXT("CharEnginePower", "推力")   },
	{ ETech::CharEngineMass,  LOCTEXT("CharEngineMass",  "质量")   },
	{ ETech::CharEngineEPR,   LOCTEXT("CharEngineEPR",   "能耗比") },
	{ ETech::CharEngineEMR,   LOCTEXT("CharEngineEMR",   "能质比") },
	{ ETech::CharEnergy,      LOCTEXT("CharEnergy",      "能量")   },

	{ ETech::ShipHP,           LOCTEXT("CharHP",          "血量")   },
	{ ETech::ShipMass,         LOCTEXT("CharMass",        "质量")   },
	{ ETech::ShipBackpack,     LOCTEXT("CharBackpack",    "承重")   },
	{ ETech::ShipShieldCold,   LOCTEXT("CharShieldCold",  "耐寒")   },
	{ ETech::ShipShieldHeat,   LOCTEXT("CharShieldHeat",  "耐热")   },
	{ ETech::ShipShieldPress,  LOCTEXT("CharShieldPress", "耐压")   },
	{ ETech::ShipEngine0Power, LOCTEXT("CharEnginePower", "推力")   },
	{ ETech::ShipEngine0Mass,  LOCTEXT("CharEngineMass",  "质量")   },
	{ ETech::ShipEngine0EPR,   LOCTEXT("CharEngineEPR",   "能耗比") },
	{ ETech::ShipEngine0EMR,   LOCTEXT("CharEngineEMR",   "能质比") },
	{ ETech::ShipEngine1Power, LOCTEXT("CharEnergy",      "推力")   },
	{ ETech::ShipEngine1Mass,  LOCTEXT("CharEnergy",      "质量")   },
	{ ETech::ShipEngine1EPR,   LOCTEXT("CharEnergy",      "能耗比") },
	{ ETech::ShipEngine1EMR,   LOCTEXT("CharEnergy",      "能质比") },
	{ ETech::ShipEngine2Power, LOCTEXT("CharEnergy",      "推力")   },
	{ ETech::ShipEngine2Mass,  LOCTEXT("CharEnergy",      "质量")   },
	{ ETech::ShipEngine2EPR,   LOCTEXT("CharEnergy",      "能耗比") },
	{ ETech::ShipEngine2EMR,   LOCTEXT("CharEnergy",      "能质比") },
	{ ETech::ShipEnergy0,      LOCTEXT("CharEnergy",      "能量")   },
	{ ETech::ShipEnergy1,      LOCTEXT("CharEnergy",      "能量")   },
	{ ETech::ShipEnergy2,      LOCTEXT("CharEnergy",      "能量")   },
};
#undef LOCTEXT_NAMESPACE
}
