﻿#include "MainLibrary.h"
#define LOCTEXT_NAMESPACE "TR"

#define ELEMENT(KEY, STR) { TEXT(STR), LOCTEXT(KEY, STR) }
TMap<FString, FText> UMainLibrary::TranslationMap = {
	// 以下来自C++源文件
	ELEMENT("Body", "躯体"),
	ELEMENT("Spaceship", "飞船"),
	ELEMENT("Pickup", "拾取了 {0} x{1}"),
	ELEMENT("Drive", "登上飞船"),
	ELEMENT("Menu", "连接基地"),
	ELEMENT("unknow", "？？？"),
	ELEMENT("BonusPoints", "到达{0}\n获得 {1} 探索点"),
	ELEMENT("CanotPut", "无法放入"),
	ELEMENT("Use", "使用"),
	ELEMENT("ToChar", "为躯体补充"),
	ELEMENT("ToShip", "为飞船补充"),
	ELEMENT("Discard", "丢弃"),
	ELEMENT("Points", "探索点数: {0}"),
	ELEMENT("CharHP", "机身强度"),
	ELEMENT("CharBackpack", "背包"),
	ELEMENT("CharShieldCold", "冷防护"),
	ELEMENT("CharShieldHeat", "热防护"),
	ELEMENT("CharShieldPress", "压力防护"),
	ELEMENT("CharEnginePower", "化学引擎"),
	ELEMENT("CharEnergy", "化学燃料仓"),
	ELEMENT("ShipHP", "机身强度"),
	ELEMENT("ShipBackpack", "存储仓"),
	ELEMENT("ShipEngine1Power", "核裂变引擎"),
	ELEMENT("ShipEngine2Power", "核聚变引擎"),
	ELEMENT("ShipEnergy0", "化学燃料仓"),
	ELEMENT("ShipEnergy1", "核裂变燃料仓"),
	ELEMENT("ShipEnergy2", "核聚变燃料仓"),
	ELEMENT("CharMoveForward", "向前或向后驱动"),
	ELEMENT("CharMoveRight", "左右驱动或(太空中)顺逆时针转动"),
	ELEMENT("CharMoveUp", "向上或向下驱动"),
	ELEMENT("CharJump", "(陆地上)跳跃"),
	ELEMENT("CharTurn", "左右转动视角"),
	ELEMENT("CharLookUp", "上下转动视角"),
	ELEMENT("CharLock", "锁定并显示星球信息"),
	ELEMENT("CharDrive", "登上飞船"),
	ELEMENT("CharPickup", "拾取"),
	ELEMENT("ShipMoveForward", "向前或向后推进"),
	ELEMENT("ShipMoveRight", "向左或向右偏转机身"),
	ELEMENT("ShipMoveUp", "向上或向下推进"),
	ELEMENT("ShipHold", "推进时按住自由转动视角"),
	ELEMENT("ShipTurn", "左右转动机身"),
	ELEMENT("ShipLookUp", "上下转动机身"),
	ELEMENT("ShipLock", "锁定并显示星球信息"),
	ELEMENT("ShipDrive", "脱离飞船"),

	// 以下来自UI界面
	ELEMENT("confirm", "确定"),
	ELEMENT("cancel", "取消"),
	ELEMENT("temperature", "温度"),
	ELEMENT("fluid pressure", "流体压力"),
	ELEMENT("mass", "质量"),
	ELEMENT("gravity", "重力"),
	ELEMENT("thrust max", "推力MAX"),
	ELEMENT("base", "基地"),
	ELEMENT("storage", "存储"),
	ELEMENT("technology", "科技"),
	ELEMENT("knowledge", "见闻"),
	ELEMENT("status", "状态"),
	ELEMENT("time", "时间"),
	ELEMENT("connected to base", "已连接基地"),
	ELEMENT("makeable item", "可加工物品"),
	ELEMENT("char info", "躯体状态"),
	ELEMENT("body", "机身"),
	ELEMENT("shield", "护盾"),
	ELEMENT("engine", "引擎"),
	ELEMENT("engine type", "引擎类型"),
	ELEMENT("fuel tank", "燃料仓"),
	ELEMENT("energy", "燃料"),
	ELEMENT("energy value", "燃料能量"),
	ELEMENT("energy", "燃料仓"),
	ELEMENT("HP", "HP"),
	ELEMENT("engine thrust", "引擎推力"),
	ELEMENT("power1", "化学动力"),
	ELEMENT("cold resistance", "耐冷"),
	ELEMENT("heat resistance", "耐热"),
	ELEMENT("press resistance", "耐压"),
	ELEMENT("current", "当前"),
	ELEMENT("next level", "下一级"),
	ELEMENT("upgrade demands", "升级所需"),
	ELEMENT("cannot upgrade", "无法升级"),
	ELEMENT("long press to upgrade", "长按升级"),
	ELEMENT("game is paused", "游戏已暂停"),
	ELEMENT("continue to play", "继续游戏"),
	ELEMENT("header page", "标题页面"),
	ELEMENT("quit the game", "退出游戏"),
	ELEMENT("rebirth", "重生"),
	ELEMENT("new game", "新游戏"),
	ELEMENT("continue", "继续"),
	ELEMENT("settings", "设置"),
	ELEMENT("notice", "提示"),
	ELEMENT("overwrite archive", "开始新游戏会覆盖已有存档"),
	ELEMENT("confirm quit", "未保存的进度将会丢失，是否仍要退出游戏"),
	ELEMENT("confirm header", "未保存的进度将会丢失，是否仍要返回标题页面"),
};

#undef LOCTEXT_NAMESPACE
