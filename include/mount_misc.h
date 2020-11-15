// mount NPDRM game  -> /dev_hdd0/game/<titleid>
// mount GAMEI game  -> /dev_usb*/GAMEI/<titleid>
// mount ROMS game   -> /dev_***/ROMS/ or *.SELF or *.ext (.ZIP.GBA.NES.UNIF.GB.GBC.DMG.MD.SMD.GEN.SMS.GG.SG.IOS.FLAC.NGP.NGC.PCE.SGX.VB.VBOY.WS.WSC.FDS.EXE.WAD.IWAD.SMC.FIG.SFC.GD3.GD7.DX2.BSX.SWC.A26.PAK.LUA.ADF.DMS.FDI.IPF.UAE.A78.MGW.LNX.VEC.J64.JAG.PRG.XFD.XEX)
// mount PS2 Classic -> *.BIN.ENC

#ifdef COBRA_ONLY
	// ------------------
	// mount GAMEI game
	// ------------------
	#ifdef MOUNT_GAMEI
	{
		char *pos = strstr(_path, "/GAMEI/");
		if(pos && !islike(_path, "/net"))
		{
			int tid_offset = 7; // folder is title_id
			char *slash = strstr(pos + tid_offset, "/"); if(slash) *slash = 0;

			if(strstr(pos + tid_offset, "_00-") == pos + 23) tid_offset += 7; // folder is content_id

			do_umount(false);

			sys_map_path(APP_HOME_DIR, _path);
			if(isDir(PKGLAUNCH_DIR)) sys_map_path(PKGLAUNCH_DIR, _path);

			get_value(map_title_id, pos + tid_offset, TITLE_ID_LEN);
			sprintf(_path, "/dev_hdd0/game/%s", map_title_id);
			sys_map_path(_path, _path0);

			sys_ppu_thread_sleep(1);
			launch_app_home_icon();

			mount_unk = EMU_GAMEI;
			goto exit_mount;
		}
	}
	#endif

	// ------------------
	// mount NPDRM game
	// ------------------
 #ifdef PKG_LAUNCHER
	char *ext = get_ext(_path);

	if(isDir(PKGLAUNCH_DIR))
	{
		if( !extcasecmp(_path, ".self", 5)    || (strcasestr(ARCHIVE_EXTENSIONS, ext) != NULL) ||
			((strstr(_path, "/PS3~") != NULL) && _IS(ext, ".zip"))
		)
		{
			ret = file_exists(_path);
			cobra_map_game(PKGLAUNCH_DIR, PKGLAUNCH_ID, true);
			save_file(PKGLAUNCH_DIR "/USRDIR/launch.txt", _path, SAVE_ALL);
			if(ret) launch_app_home_icon();
			goto mounting_done; //goto exit_mount;
		}
	}
 #endif
	if(islike(_path, HDD0_GAME_DIR) || islike(_path, _HDD0_GAME_DIR) )
	{
		ret = isDir(_path);

		if(!ret)
		{
			if(islike(_path0, HDD0_GAME_DIR))
			{
				sprintf(_path, "%s%s", HDD0_GAME_DIR, _path0 + 15); // use /dev_hdd0/game/
				if(not_exists(_path))
				{
					sprintf(_path, "%s%s", _HDD0_GAME_DIR, _path0 + 15); // use /dev_hdd0//game/ if GAMEI is enabled
				}
			}
			ret = isDir(_path);
		}

		do_umount(false);
		set_app_home(_path);

		if(launch_app_home_icon()) ret = true;

		mount_unk = EMU_MAX;
		goto exit_mount;
	}

	// ------------------
	// mount ROMS game
	// ------------------
 #ifdef MOUNT_ROMS
	if(isDir(PKGLAUNCH_DIR))
	{
		if(islike(_path, "/net")) ; else // mount ROMS in /net module

		if((strstr(_path, "/ROMS/") != NULL) || (strcasestr(_path, ".SELF") != NULL) || (strcasestr(ROMS_EXTENSIONS, ext) != NULL))
		{
			do_umount(false);

			// mount PKGLAUNCH as disc
			cobra_map_game(PKGLAUNCH_DIR, PKGLAUNCH_ID, true);

			if(file_exists(RETROARCH_DIR1))
			{
				//sys_map_path(PKGLAUNCH_DIR "/PS3_GAME/USRDIR/cores", RETROARCH_DIR1 "/USRDIR/cores");
				sys_map_path("/dev_bdvd/PS3_GAME/USRDIR/cores", RETROARCH_DIR1 "/USRDIR/cores");
				sys_map_path("/app_home/PS3_GAME/USRDIR/cores", RETROARCH_DIR1 "/USRDIR/cores");
			}
			else
			{
				//sys_map_path(PKGLAUNCH_DIR "/PS3_GAME/USRDIR/cores", RETROARCH_DIR2 "/USRDIR/cores");
				sys_map_path("/dev_bdvd/PS3_GAME/USRDIR/cores", RETROARCH_DIR2 "/USRDIR/cores");
				sys_map_path("/app_home/PS3_GAME/USRDIR/cores", RETROARCH_DIR2 "/USRDIR/cores");
			}

			// store rom path for PKGLAUNCH
			save_file(PKGLAUNCH_DIR "/USRDIR/launch.txt", _path, SAVE_ALL);
			copy_rom_media(_path);

			mount_unk = EMU_ROMS;
			goto mounting_done; //goto exit_mount;
		}
	}
 #endif // #ifdef MOUNT_ROMS

#endif // #ifdef COBRA_ONLY

	// ------------------
	// mount PS2 Classic
	// ------------------
	if(is_BIN_ENC(_path))
	{
		char temp[STD_PATH_LEN + 16];

		if(isDir(PS2_CLASSIC_PLACEHOLDER))
		{
			copy_in_progress = true, copied_count = 0;

 #ifndef LITE_EDITION
			if(c_firmware >= 4.65f)
			{   // Auto create "classic_ps2 flag" for PS2 Classic (.BIN.ENC) on rebug 4.65.2
				do_umount(false);
				enable_classic_ps2_mode();
			}
 #endif
			cellFsUnlink(PS2_CLASSIC_ISO_CONFIG);
			cellFsUnlink(PS2_CLASSIC_ISO_PATH);

			if(file_copy(_path, (char*)PS2_CLASSIC_ISO_PATH, COPY_WHOLE_FILE) >= 0)
			{
				copy_ps2config(temp, _path);

				copy_ps2savedata(temp, _path);

				copy_ps2icon(temp, _path);

				if(webman_config->fanc) restore_fan(SET_PS2_MODE); //set_fan_speed( ((webman_config->ps2temp*255)/100), 0);

				// create "wm_noscan" to avoid re-scan of XML returning to XMB from PS2
				save_file(WMNOSCAN, NULL, SAVE_ALL); ret = true;

				sprintf(temp, "\"%s\" %s", strrchr(_path, '/') + 1, STR_LOADED2);
			}
			else
				{sprintf(temp, "PS2 Classic\n%s", STR_ERROR); ret = false;}

			copy_in_progress = false;
		}
		else
		{
			sprintf(temp, "PS2 Classic Placeholder %s", STR_NOTFOUND);
			ret = false;
		}

		if(!(webman_config->minfo & 2)) show_msg(temp);

		goto exit_mount;
	}

 #ifndef LITE_EDITION
	if((c_firmware >= 4.65f) && strstr(_path, "/PS2ISO")!=NULL)
	{   // Auto remove "classic_ps2" flag for PS2 ISOs on rebug 4.65.2
		disable_classic_ps2_mode();
	}
 #endif
