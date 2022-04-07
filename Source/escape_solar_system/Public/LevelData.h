// Copyright 2020 H₂S. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LevelData.generated.h"


USTRUCT(BlueprintType)
struct FLevelDemand
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
struct FLevelDemandList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLevelDemand> List = { FLevelDemand(), FLevelDemand(), FLevelDemand(), FLevelDemand(), FLevelDemand() };
};

USTRUCT(BlueprintType)
struct FLevelValueList : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> List = {0, 0, 0, 0, 0};
};

UENUM(BlueprintType)
enum class ELevel : uint8
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

namespace LV
{
const TMap<ELevel, FName> ValueRow = {
	{ ELevel::CharHP,          "CharHP"          },
	{ ELevel::CharMass,        "CharMass"        },
	{ ELevel::CharBackpack,    "CharBackpack"    },
	{ ELevel::CharShieldCold,  "CharShieldCold"  },
	{ ELevel::CharShieldHeat,  "CharShieldHeat"  },
	{ ELevel::CharShieldPress, "CharShieldPress" },
	{ ELevel::CharEnginePower, "CharEnginePower" },
	{ ELevel::CharEngineMass,  "CharEngineMass"  },
	{ ELevel::CharEngineEPR,   "CharEngineEPR"   },
	{ ELevel::CharEngineEMR,   "CharEngineEMR"   },
	{ ELevel::CharEnergy,      "CharEnergy"      },

	{ ELevel::ShipHP,           "ShipHP"          },
	{ ELevel::ShipMass,         "ShipMass"        },
	{ ELevel::ShipBackpack,     "ShipBackpack"    },
	{ ELevel::ShipShieldCold,   "ShipShieldCold"  },
	{ ELevel::ShipShieldHeat,   "ShipShieldHeat"  },
	{ ELevel::ShipShieldPress,  "ShipShieldPress" },
	{ ELevel::ShipEngine0Power, "ShipEngine0Power"},
	{ ELevel::ShipEngine0Mass,  "ShipEngine0Mass" },
	{ ELevel::ShipEngine0EPR,   "ShipEngine0EPR"  },
	{ ELevel::ShipEngine0EMR,   "ShipEngine0EMR"  },
	{ ELevel::ShipEngine1Power, "ShipEngine1Power"},
	{ ELevel::ShipEngine1Mass,  "ShipEngine1Mass" },
	{ ELevel::ShipEngine1EPR,   "ShipEngine1EPR"  },
	{ ELevel::ShipEngine1EMR,   "ShipEngine1EMR"  },
	{ ELevel::ShipEngine2Power, "ShipEngine2Power"},
	{ ELevel::ShipEngine2Mass,  "ShipEngine2Mass" },
	{ ELevel::ShipEngine2EPR,   "ShipEngine2EPR"  },
	{ ELevel::ShipEngine2EMR,   "ShipEngine2EMR"  },
	{ ELevel::ShipEnergy0,      "ShipEnergy0"     },
	{ ELevel::ShipEnergy1,      "ShipEnergy1"     },
	{ ELevel::ShipEnergy2,      "ShipEnergy2"     },
};

const TMap<ELevel, FName> DemandRow = {
	{ ELevel::CharHP,          "CharStrength"    },
	{ ELevel::CharMass,        "CharStrength"    },
	{ ELevel::CharBackpack,    "CharBackpack"    },
	{ ELevel::CharShieldCold,  "CharShieldCold"  },
	{ ELevel::CharShieldHeat,  "CharShieldHeat"  },
	{ ELevel::CharShieldPress, "CharShieldPress" },
	{ ELevel::CharEnginePower, "CharEngine"      },
	{ ELevel::CharEngineMass,  "CharEngine"      },
	{ ELevel::CharEngineEPR,   "CharEngine"      },
	{ ELevel::CharEngineEMR,   "CharEngine"      },
	{ ELevel::CharEnergy,      "CharEnergy"      },

	{ ELevel::ShipHP,           "ShipStrength"   },
	{ ELevel::ShipMass,         "ShipStrength"   },
	{ ELevel::ShipBackpack,     "ShipBackpack"   },
	{ ELevel::ShipShieldCold,   "ShipShieldCold" },
	{ ELevel::ShipShieldHeat,   "ShipShieldHeat" },
	{ ELevel::ShipShieldPress,  "ShipShieldPress"},
	{ ELevel::ShipEngine0Power, "ShipEngine0"    },
	{ ELevel::ShipEngine0Mass,  "ShipEngine0"    },
	{ ELevel::ShipEngine0EPR,   "ShipEngine0"    },
	{ ELevel::ShipEngine0EMR,   "ShipEngine0"    },
	{ ELevel::ShipEngine1Power, "ShipEngine1"    },
	{ ELevel::ShipEngine1Mass,  "ShipEngine1"    },
	{ ELevel::ShipEngine1EPR,   "ShipEngine1"    },
	{ ELevel::ShipEngine1EMR,   "ShipEngine1"    },
	{ ELevel::ShipEngine2Power, "ShipEngine2"    },
	{ ELevel::ShipEngine2Mass,  "ShipEngine2"    },
	{ ELevel::ShipEngine2EPR,   "ShipEngine2"    },
	{ ELevel::ShipEngine2EMR,   "ShipEngine2"    },
	{ ELevel::ShipEnergy0,      "ShipEnergy0"    },
	{ ELevel::ShipEnergy1,      "ShipEnergy1"    },
	{ ELevel::ShipEnergy2,      "ShipEnergy2"    },
};

#define LOCTEXT_NAMESPACE "LevelData"
const TMap<ELevel, FText> DispName = {
	{ ELevel::CharHP,          LOCTEXT("CharHP",          "血量")   },
	{ ELevel::CharMass,        LOCTEXT("CharMass",        "质量")   },
	{ ELevel::CharBackpack,    LOCTEXT("CharBackpack",    "承重")   },
	{ ELevel::CharShieldCold,  LOCTEXT("CharShieldCold",  "耐寒")   },
	{ ELevel::CharShieldHeat,  LOCTEXT("CharShieldHeat",  "耐热")   },
	{ ELevel::CharShieldPress, LOCTEXT("CharShieldPress", "耐压")   },
	{ ELevel::CharEnginePower, LOCTEXT("CharEnginePower", "推力")   },
	{ ELevel::CharEngineMass,  LOCTEXT("CharEngineMass",  "质量")   },
	{ ELevel::CharEngineEPR,   LOCTEXT("CharEngineEPR",   "能耗比") },
	{ ELevel::CharEngineEMR,   LOCTEXT("CharEngineEMR",   "能质比") },
	{ ELevel::CharEnergy,      LOCTEXT("CharEnergy",      "能量")   },

	{ ELevel::ShipHP,           LOCTEXT("CharHP",          "血量")   },
	{ ELevel::ShipMass,         LOCTEXT("CharMass",        "质量")   },
	{ ELevel::ShipBackpack,     LOCTEXT("CharBackpack",    "承重")   },
	{ ELevel::ShipShieldCold,   LOCTEXT("CharShieldCold",  "耐寒")   },
	{ ELevel::ShipShieldHeat,   LOCTEXT("CharShieldHeat",  "耐热")   },
	{ ELevel::ShipShieldPress,  LOCTEXT("CharShieldPress", "耐压")   },
	{ ELevel::ShipEngine0Power, LOCTEXT("CharEnginePower", "推力")   },
	{ ELevel::ShipEngine0Mass,  LOCTEXT("CharEngineMass",  "质量")   },
	{ ELevel::ShipEngine0EPR,   LOCTEXT("CharEngineEPR",   "能耗比") },
	{ ELevel::ShipEngine0EMR,   LOCTEXT("CharEngineEMR",   "能质比") },
	{ ELevel::ShipEngine1Power, LOCTEXT("CharEnergy",      "推力")   },
	{ ELevel::ShipEngine1Mass,  LOCTEXT("CharEnergy",      "质量")   },
	{ ELevel::ShipEngine1EPR,   LOCTEXT("CharEnergy",      "能耗比") },
	{ ELevel::ShipEngine1EMR,   LOCTEXT("CharEnergy",      "能质比") },
	{ ELevel::ShipEngine2Power, LOCTEXT("CharEnergy",      "推力")   },
	{ ELevel::ShipEngine2Mass,  LOCTEXT("CharEnergy",      "质量")   },
	{ ELevel::ShipEngine2EPR,   LOCTEXT("CharEnergy",      "能耗比") },
	{ ELevel::ShipEngine2EMR,   LOCTEXT("CharEnergy",      "能质比") },
	{ ELevel::ShipEnergy0,      LOCTEXT("CharEnergy",      "能量")   },
	{ ELevel::ShipEnergy1,      LOCTEXT("CharEnergy",      "能量")   },
	{ ELevel::ShipEnergy2,      LOCTEXT("CharEnergy",      "能量")   },
};
#undef LOCTEXT_NAMESPACE
}
