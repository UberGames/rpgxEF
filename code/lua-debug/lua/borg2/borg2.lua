-- Startup
function InitGame(levelTime, randomSeed, restart)
	game.Print("Lua Level Init...");
	game.Print("-map_restart ...");
		game.Print("--workaround-setup ...");
			ent = entity.Spawn();
			ent:SetClassname("target_levelchange");
			ent:SetTargetname("map_restart");
			ent:SetTarget("borg2");
			entity.CallSpawn(ent);
		game.Print("--trigger-setup ...");
		if  restart == 1 then
			game.ClientPrint(-1, "Please don't use map_restart, use map or devmap instead.");
			game.Print("Please don't use map_restart, use map or devmap instead.");
			ent = entity.Find("map_restart")
			entity.Use(ent)
		end      
	game.Print("-Reimplementing splashDamage, SplashRadius and material to existing misc_model_breakables...");
		entity.MMBRefit();
		
	game.Print("-Fixing some Standalone Breakables");
		game.Print("--disnode");
			ent = entity.Spawn();
			ent:SetClassname("misc_model_breakable");
			ent:SetHealth(1);
			ent:SetSpawnflags(1);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			ent:SetModel("models/mapobjects/borg/disnode.md3");
			mover.SetPosition(ent, 304, -790, 48);
			mover.SetAngles(ent, 0, 90, 0);
			entity.CallSpawn(ent);
			
		game.Print("--Plasmafilter 1");
			ent = entity.Spawn();
			ent:SetClassname("misc_model_breakable");
			ent:SetTarget("t188");
			ent:SetHealth(1);
			ent:SetSpawnflags(9);
			ent:SetSplashDamage(250);
			ent:SetSplashRadius(150);
			ent:SetModel("models/mapobjects/borg/tank.md3");
			mover.SetPosition(ent, 564, 452, 64);
			mover.SetAngles(ent, 0, 90, 0);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(6);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(2);
			ent:SetKeyValue("team", 2);
			entity.CallSpawn(ent);
			
		game.Print("--Plasmafilter 2");
			ent = entity.Spawn();
			ent:SetClassname("misc_model_breakable");
			ent:SetTarget("t189");
			ent:SetHealth(1);
			ent:SetSpawnflags(9);
			ent:SetSplashDamage(250);
			ent:SetSplashRadius(150);
			ent:SetModel("models/mapobjects/borg/tank.md3");
			mover.SetPosition(ent, 1131, -880, 232);
			mover.SetAngles(ent, 0, 180, 0);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(5);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
		game.Print("--Plasmafilter 3");
			ent = entity.Spawn();
			ent:SetClassname("misc_model_breakable");
			ent:SetTarget("t191");
			ent:SetHealth(1);
			ent:SetSpawnflags(9);
			ent:SetSplashDamage(250);
			ent:SetSplashRadius(150);
			ent:SetModel("models/mapobjects/borg/tank.md3");
			mover.SetPosition(ent, 828, -1692, 244);
			mover.SetAngles(ent, 0, 180, 0);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(3);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(4);
			ent:SetKeyValue("team", 2);
			entity.CallSpawn(ent);
			
	game.Print("-Some Decorative Work...");
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin2.md3");
		mover.SetPosition(ent, -325, 211, 47);
		mover.SetAngles(ent, 0, 180, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin.md3");
		mover.SetPosition(ent, -261, 63, 47);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin.md3");
		mover.SetPosition(ent, 388, -799, 47);
		mover.SetAngles(ent, 0, 90, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin2.md3");
		mover.SetPosition(ent, 993, 511, 94);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin.md3");
		mover.SetPosition(ent, 902, 241, 606);
		mover.SetAngles(ent, 0, 90, 0);
		entity.CallSpawn(ent)
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin.md3");
		mover.SetPosition(ent, 887, -517, 239);
		mover.SetAngles(ent, 0, 45, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin.md3");
		mover.SetPosition(ent, 364, -985, 110);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("misc_model_breakable");
		ent:SetSpawnflags(17);
		ent:SetModel("models/mapobjects/borg/plugin2.md3");
		mover.SetPosition(ent, 645, -1465, 255);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
	game.Print("-Final Room ...");
		game.Print("--Forcefield ...");
			ent = entity.Spawn();
			ent:SetClassname("misc_model_breakable");
			ent:SetTarget("t58");
			ent:SetHealth(1);
			ent:SetSpawnflags(1);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			ent:SetModel("models/mapobjects/borg/disnode.md3");
			mover.SetPosition(ent, 2080, -2006, 176);
			mover.SetAngles(ent, 0, 90, 0);
			entity.CallSpawn(ent);
			ent = entity.FindBModel(81);
			ent:SetClassname("func_forcefield");
			entity.CallSpawn(ent);
			
		game.Print("--Console Breakable trigger and kill...");
			ent = entity.FindBModel(100);
			ent:SetClassname("func_breakable");
			ent:SetHealth(1);
			ent:SetKeyValue("material", 1);
			ent:SetSplashDamage(999999);
			ent:SetSplashRadius(999999);
			mover.SetPosition(ent, 2016, -1576, 178);
			entity.CallSpawn(ent);
			
	game.Print("-lift fixes ...");
		game.Print("--Lift A ...");
			game.Print("---Clear Usable ...");
				ent = entity.FindBModel(98);
				entity.Remove(ent);
				
			game.Print("---reconfig Usable ...");
				ent = entity.FindBModel(57)
				ent:SetSpawnflags(520);
				ent:SetTargetname("liftcontrol-a");
				ent:SetTarget("lift-a-bool");
				ent:SetWait(11);
				ent:SetLuaUse("borglcarswitch");
				
			game.Print("---reconfig elevator ...");
				ent = entity.FindBModel(58)
				ent:SetClassname("func_door");
				ent:SetSpawnflags(32);
				ent:SetTargetname("lift-a");
				ent:SetKeyValue("angle", -2);
				ent:SetWait(5);
				ent:SetSpeed(50);
				ent:SetCount(-94);
				entity.CallSpawn(ent);
				
			game.Print("---set up step-on-trigger ...");
				ent = entity.Spawn();
				ent.SetupTrigger(ent, 128, 128, 8);
				ent:SetClassname("trigger_multiple");
				ent:SetTarget("lift-a-bool");
				ent:SetWait(1);
				entity.CallSpawn(ent);
				mover.SetPosition(ent, 1184, 384, 196);
				
			game.Print("---set up boolean ...");
				ent = entity.Spawn();
				ent:SetClassname("target_boolean");
				ent:SetTargetname("lift-a-bool");
				ent:SetFalsetarget("lift-a");
				ent:SetSwapname("lift-a-swap");
				entity.CallSpawn(ent);
				
			game.Print("---set up delay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_delay");
				ent:SetTargetname("lift-a");
				ent:SetTarget("lift-a-swap");
				ent:SetSpawnflags(1);
				ent:SetWait(15);
				entity.CallSpawn(ent);
				
			game.Print("---set up relay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_relay");
				ent:SetTargetname("lift-a");
				ent:SetTarget("lift-a-swap");
				ent:SetSpawnflags(8);
				entity.CallSpawn(ent);
				
		game.Print("--Lift B ...");
			game.Print("---reconfig Usable ...");
				ent = entity.FindBModel(48)
				ent:SetSpawnflags(8);
				ent:SetTargetname("liftcontrol-b");
				ent:SetTarget("lift-b-bool");
				ent:SetWait(11);
				ent:SetLuaUse("borglcarswitch");
				
			game.Print("---reconfig elevator ...");
				ent = entity.FindBModel(50)
				ent:SetClassname("func_door");
				ent:SetSpawnflags(32);
				ent:SetTargetname("lift-b");
				ent:SetKeyValue("angle", -2);
				ent:SetWait(5);
				ent:SetSpeed(50);
				ent:SetCount(-144);
				entity.CallSpawn(ent);
				
			game.Print("---set up step-on-trigger ...");
				ent = entity.Spawn();
				ent.SetupTrigger(ent, 96, 128, 8);
				ent:SetClassname("trigger_multiple");
				ent:SetTarget("lift-b-bool");
				ent:SetWait(1);
				entity.CallSpawn(ent);
				mover.SetPosition(ent, 768, -880, 196);
				
			game.Print("---set up boolean ...");
				ent = entity.Spawn();
				ent:SetClassname("target_boolean");
				ent:SetTargetname("lift-b-bool");
				ent:SetFalsetarget("lift-b");
				ent:SetSwapname("lift-b-swap");
				entity.CallSpawn(ent);
				
			game.Print("---set up delay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_delay");
				ent:SetTargetname("lift-b");
				ent:SetTarget("lift-b-swap");
				ent:SetSpawnflags(1);
				ent:SetWait(15);
				entity.CallSpawn(ent);
				
			game.Print("---set up relay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_relay");
				ent:SetTargetname("lift-b");
				ent:SetTarget("lift-b-swap");
				ent:SetSpawnflags(8);
				entity.CallSpawn(ent);
				
		game.Print("--Lift C ...");
			game.Print("---Clear Usable ...");
				ent = entity.FindBModel(99);
				entity.Remove(ent);
				
			game.Print("---reconfig Usable ...");
				ent = entity.FindBModel(60)
				ent:SetSpawnflags(8);
				ent:SetTargetname("liftcontrol-c");
				ent:SetTarget("lift-c-bool");
				ent:SetWait(11);
				ent:SetLuaUse("borglcarswitch");
				
			game.Print("---reconfig elevator ...");
				ent = entity.FindBModel(47)
				ent:SetClassname("func_door");
				ent:SetSpawnflags(32);
				ent:SetTargetname("lift-c");
				ent:SetKeyValue("angle", -2);
				ent:SetWait(5);
				ent:SetSpeed(50);
				ent:SetCount(-112);
				entity.CallSpawn(ent);
				
			game.Print("---set up step-on-trigger ...");
				ent = entity.Spawn();
				ent.SetupTrigger(ent, 128, 96, 8);
				ent:SetClassname("trigger_multiple");
				ent:SetTarget("lift-c-bool");
				ent:SetWait(1);
				entity.CallSpawn(ent);
				mover.SetPosition(ent, 336, -1296, 196);
				
			game.Print("---set up boolean ...");
				ent = entity.Spawn();
				ent:SetClassname("target_boolean");
				ent:SetTargetname("lift-c-bool");
				ent:SetFalsetarget("lift-c");
				ent:SetSwapname("lift-c-swap");
				entity.CallSpawn(ent);
				
			game.Print("---set up delay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_delay");
				ent:SetTargetname("lift-c");
				ent:SetTarget("lift-c-swap");
				ent:SetSpawnflags(1);
				ent:SetWait(15);
				entity.CallSpawn(ent);
				
			game.Print("---set up relay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_relay");
				ent:SetTargetname("lift-c");
				ent:SetTarget("lift-c-swap");
				ent:SetSpawnflags(8);
				entity.CallSpawn(ent);
				
		game.Print("--Lift D...");
			game.Print("---Segment reclassification ...");
				ent = entity.FindBModel(52)
				ent:SetClassname("func_borg_elevator");
				entity.CallSpawn(ent);
				
				ent = entity.FindBModel(54)
				ent:SetClassname("func_borg_elevator");
				entity.CallSpawn(ent); 
				
			--[[game.Print("---Metascripts ...");
				game.Print("---down1 ...");
					ent = entity.Spawn();
					ent:SetClassname("target_relay");
					ent:SetTargetname("down1");
					ent:SetLuaUse("liftddown1");
					entity.CallSpawn(ent);
					
					ent = entity.Spawn();
					ent:SetClassname("target_delay");
					ent:SetTargetname("down1");
					ent:SetTarget("down2");
					ent:SetWait(5);
					entity.CallSpawn(ent);
					
				game.Print("---down2 ...");
					ent = entity.Spawn();
					ent:SetClassname("target_relay");
					ent:SetTargetname("down2");
					ent:SetLuaUse("liftddown2");
					entity.CallSpawn(ent);
					
					ent = entity.Spawn();
					ent:SetClassname("target_delay");
					ent:SetTargetname("down2");
					ent:SetTarget("up2");
					ent:SetWait(5);
					entity.CallSpawn(ent);
					
				game.Print("---up2 ...");
					ent = entity.Spawn();
					ent:SetClassname("target_relay");
					ent:SetTargetname("up2");
					ent:SetLuaUse("liftdup2");
					entity.CallSpawn(ent);
					
					ent = entity.Spawn();
					ent:SetClassname("target_delay");
					ent:SetTargetname("up2");
					ent:SetTarget("up1");
					ent:SetWait(5);
					entity.CallSpawn(ent);
					
				game.Print("---up1 ...");
					ent = entity.Spawn();
					ent:SetClassname("target_relay");
					ent:SetTargetname("up1");
					ent:SetLuaUse("liftdup1");
					entity.CallSpawn(ent);
					
					ent = entity.Spawn();
					ent:SetClassname("target_delay");
					ent:SetTargetname("up1");
					ent:SetTarget("down1");
					ent:SetWait(5);
					entity.CallSpawn(ent);
					
			game.Print("---init ...");
				ent = entity.Spawn();
				ent:SetClassname("target_relay");
				ent:SetTarget("down1");
				entity.CallSpawn(ent);
				entity.Use(ent)
				
			game.Print("---fx-startspawn ...");
				ent = entity.Spawn();
				ent:SetClassname("fx_borg_bolt");
				ent:SetMessage("dual_lift_upper");
				ent:SetTarget("dual_lift_lower");
				entity.CallSpawn(ent);]]--
				
		game.Print("--Lift E ...");
			game.Print("---reconfig elevator ...");
				ent = entity.FindBModel(55)
				ent:SetClassname("func_door");
				ent:SetTargetname("lift-e");
				ent:SetSpawnflags(32);
				ent:SetKeyValue("angle", -2);
				ent:SetWait(5);
				ent:SetSpeed(50);
				ent:SetCount(-140);
				entity.CallSpawn(ent);
				
			game.Print("---set up step-on-trigger ...");
				ent = entity.Spawn();
				ent.SetupTrigger(ent, 96, 128, 8);
				ent:SetClassname("trigger_multiple");
				ent:SetTarget("lift-e-bool");
				ent:SetWait(1);
				entity.CallSpawn(ent);
				mover.SetPosition(ent, 1452, -1984, 380);
				
			game.Print("---set up boolean ...");
				ent = entity.Spawn();
				ent:SetClassname("target_boolean");
				ent:SetTargetname("lift-e-bool");
				ent:SetFalsetarget("lift-e");
				ent:SetSwapname("lift-e-swap");
				entity.CallSpawn(ent);
				
			game.Print("---set up delay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_delay");
				ent:SetTargetname("lift-e");
				ent:SetTarget("lift-e-swap");
				ent:SetSpawnflags(1);
				ent:SetWait(15);
				entity.CallSpawn(ent);
				
			game.Print("---set up relay ...");
				ent = entity.Spawn();
				ent:SetClassname("target_relay");
				ent:SetTargetname("lift-e");
				ent:SetTarget("lift-e-swap");
				ent:SetSpawnflags(8);
				entity.CallSpawn(ent);
				
	game.Print("-Midway Drop prep...");
		game.Print("--Visual Side Effects...");
			ent = entity.FindBModel(7);   
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			 
			ent = entity.FindBModel(8);   
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			 
			ent = entity.FindBModel(43);   
			ent:SetKeyValue("team", 2);
			ent:SetTargetname("exp");
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(45);   
			ent:SetKeyValue("team", 2);
			ent:SetTargetname("exp");
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(46);   
			ent:SetKeyValue("team", 2);
			ent:SetTargetname("exp");
			entity.CallSpawn(ent);
			  
		game.Print("--Drop control...");
			ent = entity.Spawn();
			ent:SetClassname("target_relay");
			ent:SetTargetname("exp");
			ent:SetLuaUse("midwaydrop1A");
			entity.CallSpawn(ent); 
			
			ent = entity.Spawn();
			ent:SetClassname("target_relay");
			ent:SetTargetname("exp");
			ent:SetLuaUse("midwaydrop2A");
			entity.CallSpawn(ent);
			     
			ent = entity.Spawn();
			ent:SetClassname("target_relay");
			ent:SetTargetname("exp");
			ent:SetLuaUse("midwaydrop3A");
			entity.CallSpawn(ent);
			  
			ent = entity.Spawn();
			ent:SetClassname("target_relay");
			ent:SetTargetname("exp");
			ent:SetLuaUse("midwaydrop4A");
			entity.CallSpawn(ent);  
			
		game.Print("--Convert to breakables...");
			ent = entity.FindBModel(40);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(41);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(42);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(44);
			ent:SetClassname("func_breakable");
			ent:SetSpawnflags(1);
			ent:SetKeyValue("material", 1);
			entity.CallSpawn(ent);
			
	game.Print("-Setting up spawnpoints...");
		entity.RemoveSpawns();
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, -424, 344, 28);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, -424, 288, 28);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, -368, 344, 28);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, -368, 288, 28);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, 1920, -1472, 152);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, 1984, -1472, 152);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, 2048, -1472, 152);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		
		ent = entity.Spawn();
		ent:SetClassname("info_player_deathmatch");
		mover.SetPosition(ent, 2112, -1472, 152);
		mover.SetAngles(ent, 0, 270, 0);
		entity.CallSpawn(ent);
		  
	game.Print("-Breakable-Mod...");
		game.Print("--func_breakable...");
			ent = entity.FindBModel(25);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(26);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
			
			ent = entity.FindBModel(21);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(22);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
			
			ent = entity.FindBModel(23);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(24);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
			
			ent = entity.FindBModel(19);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(20);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
			
			ent = entity.FindBModel(17);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(18);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
			
			ent = entity.FindBModel(15);
			ent:SetKeyValue("material", 2);
			ent:SetSplashDamage(75);
			ent:SetSplashRadius(75);
			entity.CallSpawn(ent);
			
			ent = entity.FindBModel(16);
			ent:SetKeyValue("team", 2)
			entity.CallSpawn(ent)
		--[[game.Print("--misc_model_breakable...");
			game.Print("---checking for local or dedicated...");
				if not entity.FindNumber(316) == nil then
				ent = entity.FindNumber(316);
				test = ent.GetClassname(ent);
				if test == "misc_model_breakable" then
				
			game.Print("----dedicated...");
				ent = entity.FindNumber(184);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(185);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(186);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(187);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(195);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(197);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent); 
				
				ent = entity.FindNumber(200);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(275);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(276);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(277);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(278);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(279);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(280);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(281);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(282);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(283);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(284);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(315);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(316);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				else
				
			game.Print("----local...");
				ent = entity.FindNumber(184);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(185);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(186);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(187);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(195);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(197);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(200);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(254);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(255);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(256);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(257);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(258);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(259);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(260);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(261);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(262);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(263);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(294);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(295);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				end
				else
				
			game.Print("----local...");
				ent = entity.FindNumber(184);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(185);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(186);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(187);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(195);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(197);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(200);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(254);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(255);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(256);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(257);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(258);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(259);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(260);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(261);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(262);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				ent = entity.FindNumber(263);
				ent:SetKeyValue("material", 2);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				
				--ent = entity.FindNumber(294);
				--ent:SetKeyValue("material", 1);
				--ent:SetSplashDamage(75);
				--ent:SetSplashRadius(75);
				--entity.CallSpawn(ent);
				
				ent = entity.FindNumber(295);
				ent:SetKeyValue("material", 1);
				ent:SetSplashDamage(75);
				ent:SetSplashRadius(75);
				entity.CallSpawn(ent);
				end]]--
				
	game.Print("-Bolt Fixes...");
		ent = entity.Spawn();
		ent:SetClassname("info_notnull");
		ent:SetTargetname("t54");
		entity.CallSpawn(ent);
		mover.SetPosition(ent, 1056, -144, 1056);
		
		ent = entity.Spawn();
		ent:SetClassname("info_notnull");
		ent:SetTargetname("t64");
		entity.CallSpawn(ent);
		mover.SetPosition(ent, 352, -32, 64);
game.Print("Done");	
end     

-- Functions Midway Drop

function midwaydrop1A(ent, other, activator) 
	ent = entity.FindBModel(44);
	ent:SetLuaReached("midwaydrop1B");
	mover.ToPosition(ent, 400, 958, -462, 178);
end

function midwaydrop1B(ent)
	mover.Halt(ent);
	ent:SetLuaReached("midwaydrop1C");
	mover.ToPosition(ent, 400, 934, -374, -342);
end

function midwaydrop1C(ent)
	mover.Halt(ent);
	entity.Use(ent);
end

function midwaydrop2A(ent, other, activator) 
	ent = entity.FindBModel(41);
	ent:SetLuaReached("midwaydrop2B");
	mover.ToPosition(ent, 400, 802, -292, 0);
end

function midwaydrop2B(ent)
	mover.Halt(ent);
	ent:SetLuaReached("midwaydrop2C");
	mover.ToPosition(ent, 400, 818, -292, -376);
end

function midwaydrop2C(ent)
	mover.Halt(ent);
	entity.Use(ent);
end

function midwaydrop3A(ent, other, activator) 
	ent = entity.FindBModel(42);
	ent:SetLuaReached("midwaydrop3B");
	mover.ToPosition(ent, 400, 834, -192, -48);
end

function midwaydrop3B(ent)
	mover.Halt(ent);
	ent:SetLuaReached("midwaydrop3C");
	mover.ToPosition(ent, 400, 858, -192, -492);
end

function midwaydrop3C(ent)
	mover.Halt(ent);
	entity.Use(ent);
end

function midwaydrop4A(ent, other, activator) 
	ent = entity.FindBModel(40);
	ent:SetLuaReached("midwaydrop4B");
	mover.ToPosition(ent, 400, 914, -66, 124);
end

function midwaydrop4B(ent)
	mover.Halt(ent);
	ent:SetLuaReached("midwaydrop4C");
	mover.ToPosition(ent, 400, 918, -130, -404);
end

function midwaydrop4C(ent)
	mover.Halt(ent);
	entity.Use(ent);
end

--Sounds

function borglcarswitch(ent, other, activator)
    sound.PlaySound(entity.FindBModel(48), "sound/movers/switches/borglcarswitch.mp3", 0);
end

-- Functions Lift D

function liftddown1(ent, other, activator)
	ent = entity.FindBModel(52);
	mover.ToPosition(ent, 12, 768, -1984, 224);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
end

function liftddown2(ent, other, activator)
	ent = entity.FindBModel(52); 
	mover.ToPosition(ent, 30, 768, -1984, 104);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
	ent = entity.FindBModel(54);
	mover.ToPosition(ent, 30, 768, -1984, 104);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
end

function liftdup2(ent, other, activator)
	ent = entity.FindBModel(52);
	mover.ToPosition(ent, 30, 768, -1984, 224);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
	ent = entity.FindBModel(54);
	mover.ToPosition(ent, 30, 768, -1984, 224);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
end

function liftdup1(ent, other, activator)
	ent = entity.FindBModel(52);
	mover.ToPosition(ent, 12, 768, -1984, 272);
    sound.PlaySound(ent, "sound/movers/doors/largedoorstart.mp3", 0);
end

function liftdreachedsound(ent, other, activator)
    sound.PlaySound(entity.FindBModel(52), "sound/movers/doors/largedoorstop.mp3", 0);
end