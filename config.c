#include "config.h"
#include "osd.h"
#include "pwd.h"




TConfig *ConfigInit(xine_t *xine)
{
TConfig *Config=NULL;
char *Tempstr=NULL; 

  Config=(TConfig *) calloc(1, sizeof(TConfig));
	Config->xine=xine;
  Config->playlist=StringListCreate(0,NULL);

	//Cache media for 48 hours
	Config->cache_maxage=3600 * 48;

  Tempstr=rstrcpy(Tempstr, xine_get_homedir());
	Tempstr=rstrcat(Tempstr, "/.cxine/cxine.conf");
	xine_config_load(xine, Tempstr);

	xine_config_register_string(xine, "media.dvd.device", "/dev/dvd", "Default DVD device for cxine", "", 1, 0, NULL);
	xine_config_register_string(xine, "media.dvd.language", "en", "Default DVD language for cxine", "", 1, 0, NULL);
	xine_config_register_num(xine, "media.dvd.region", 1, "Default DVD region for cxine (1 to 8)", "", 1, 0, NULL);

	Config->background=rstrcpy(Config->background, xine_config_register_string(xine, "cxine.background", "", "Background image for cxine", "", 1, 0, NULL));

  Config->audio_compression=xine_config_register_num(xine, "cxine.audio_compression", 150, "Audio compression level", "", 1, 0, NULL);
  Config->top_osd_text=rstrcpy(Config->top_osd_text, xine_config_register_string (xine, "cxine.top_osd", DEFAULT_TOPOSD_STRING, "Default text for top OSD", "", 1, 0, NULL));
  Config->bottom_osd_text=rstrcpy(Config->bottom_osd_text, xine_config_register_string (xine, "cxine.bottom_osd", DEFAULT_BOTTOMOSD_STRING, "Default text for bottom OSD", "", 1, 0, NULL));

  Config->flags |= CONFIG_CONTROL;
  Config->control_pipe=-1;
  Config->nowplay_pipe=-1;

	Config->vo_driver=rstrcpy(Config->vo_driver, xine_config_register_string (xine, "cxine.vo_driver", "auto", "Default video driver for cxine", "", 1, 0, NULL));
  Config->ao_driver=rstrcpy(Config->ao_driver, xine_config_register_string (xine, "cxine.ao_driver", "auto", "Default audio driver for cxine", "", 1, 0, NULL));


  Tempstr=rstrcpy(Tempstr, xine_get_homedir());
  Tempstr=rstrcat(Tempstr, "/.cxine/cache/");
	Config->cache_dir=rstrcpy(Config->cache_dir, xine_config_register_string (xine, "cxine.cachedir", Tempstr, "Cache directory for cxine downloads", "", 1, 0, NULL));

	Tempstr=rstrcpy(Tempstr, xine_get_homedir());
	Tempstr=rstrcat(Tempstr, "/.cxine/control.pipe");
	Config->control_pipe_path=rstrcpy(Config->control_pipe_path, xine_config_register_string (xine, "cxine.control_pipe_path", Tempstr, "path for CXine control pipe", "", 1, 0, NULL));

	Config->nowplay_pipe_path=rstrcpy(Config->nowplay_pipe_path, xine_config_register_string (xine, "cxine.nowplay_pipe_path", "", "path for CXine 'now playing/ pipe", "", 1, 0, NULL));


	Config->path_prefix=rstrcpy(Config->path_prefix, xine_config_register_string (xine, "cxine.path_prefix", "", "Prefix to apply to MRLs", "", 1, 0, NULL));
	Config->audio_plugins=rstrcpy(Config->audio_plugins, xine_config_register_string (xine, "cxine.audio_plugins", "", "Audio plugins to load", "", 1, 0, NULL));

	if (xine_config_register_bool(xine, "cxine.bookmark", TRUE, "Bookmarks (Start playing a track from where we were last)", "", 1, 0, NULL)) Config->flags |=CONFIG_BOOKMARK;
	if (xine_config_register_bool(xine, "cxine.persist", FALSE, "Keep cxine open (persistent) even if nothing playing", "", 1, 0, NULL)) Config->flags |=CONFIG_PERSIST;
	if (xine_config_register_bool(xine, "cxine.show_osd", FALSE, "Display On Screen Display in cxine", "", 1, 0, NULL)) Config->flags |=CONFIG_OSD;
	if (xine_config_register_bool(xine, "cxine.disable_screensaver", FALSE, "Disable X11 screensaver", "", 1, 0, NULL)) Config->flags |= DISABLE_SCREENSAVER;



	destroy(Tempstr);
  return(Config);
}





void ConfigDefaults(TConfig *Config)
{
char *Tempstr=NULL; 


	Config->vo_driver=rstrcpy(Config->vo_driver, "auto");
	CXineConfigModify(Config->xine, "cxine.vo_driver", Config->vo_driver);
	Config->ao_driver=rstrcpy(Config->ao_driver, "auto");
	CXineConfigModify(Config->xine, "cxine.ao_driver", Config->ao_driver);
	Config->background=rstrcpy(Config->background, "");
	CXineConfigModify(Config->xine, "cxine.background", Config->background);
	Config->audio_compression=150;
	CXineConfigNumericModify(Config->xine, "cxine.audio_compression", Config->audio_compression);
	CXineConfigModify(Config->xine, "media.dvd.device", "/dev/dvd");
	CXineConfigModify(Config->xine, "media.dvd.language", "en");
	CXineConfigNumericModify(Config->xine, "media.dvd.region", 1);
  Config->top_osd_text=rstrcpy(Config->top_osd_text, DEFAULT_TOPOSD_STRING);
	CXineConfigModify(Config->xine, "cxine.top_osd", Config->background);
  Config->bottom_osd_text=rstrcpy(Config->bottom_osd_text, DEFAULT_BOTTOMOSD_STRING);
	CXineConfigModify(Config->xine, "cxine.bottom_osd", Config->background);
  Config->cache_dir=rstrcpy(Config->cache_dir, xine_get_homedir());
  Config->cache_dir=rstrcat(Config->cache_dir, "/.cxine/cache/");
	CXineConfigModify(Config->xine, "cxine.cachedir", Config->cache_dir);
	Config->control_pipe_path=rstrcpy(Config->control_pipe_path, xine_get_homedir());
	Config->control_pipe_path=rstrcat(Config->control_pipe_path, "/.cxine/control.pipe");
	CXineConfigModify(Config->xine, "cxine.control_pipe_path", Config->control_pipe_path);
	Config->nowplay_pipe_path=rstrcat(Config->nowplay_pipe_path,"");
	CXineConfigModify(Config->xine, "cxine.nowplay_pipe_path", Config->nowplay_pipe_path);
	Config->path_prefix=rstrcpy(Config->path_prefix, "");

	Config->path_prefix=rstrcpy(Config->path_prefix, "");
	CXineConfigModify(Config->xine, "cxine.path_prefix", Config->path_prefix);
	Config->path_prefix=rstrcpy(Config->audio_plugins, "");
	CXineConfigModify(Config->xine, "cxine.audio_plugins", Config->audio_plugins);

  Config->flags = CONFIG_CONTROL | CONFIG_BOOKMARK;
	CXineConfigNumericModify(Config->xine, "cxine.bookmark", 1);
	CXineConfigNumericModify(Config->xine, "cxine.persist", 0);
	CXineConfigNumericModify(Config->xine, "cxine.show_osd", 0);
	CXineConfigNumericModify(Config->xine, "cxine.disable_screensaver", 0);



  Config->control_pipe=-1;
  Config->nowplay_pipe=-1;


	destroy(Tempstr);
}



void CXineConfigModify(xine_t *xine, const char *Key, const char *Value)
{
xine_cfg_entry_t entry;

if (xine_config_lookup_entry(xine, Key, &entry))
{
	//seems these strings are internally freed by libxine, so we can't copy over them
//	entry.str_value=rstrcpy(entry.str_value, Value);
	entry.str_value=strdup(Value);

	switch (entry.type)
	{
	case XINE_CONFIG_TYPE_BOOL:    
	if (atoi(entry.str_value)) entry.num_value=1;
	else entry.num_value=0;
	break;

	case XINE_CONFIG_TYPE_NUM:
	entry.num_value=atoi(entry.str_value);
	break;
	}
	
	xine_config_update_entry (xine, &entry);
}
}


void CXineConfigNumericModify(xine_t *xine, const char *Key, int Value)
{
char *Tempstr=NULL;

	Tempstr=realloc(Tempstr, 40);
	snprintf(Tempstr, 40, "%d", Value);
	CXineConfigModify(xine, Key, Tempstr);

	destroy(Tempstr);
}



void CXineConfigSave(TConfig *Config)
{
char *Tempstr=NULL;

	CXineConfigModify(Config->xine, "cxine.vo_driver", Config->vo_driver);
	CXineConfigModify(Config->xine, "cxine.ao_driver", Config->ao_driver);
	CXineConfigModify(Config->xine, "cxine.keygrabs", Config->keygrabs);
	CXineConfigModify(Config->xine, "cxine.path_prefix", Config->path_prefix);
	CXineConfigModify(Config->xine, "cxine.audio_plugins", Config->audio_plugins);
	CXineConfigModify(Config->xine, "cxine.control_pipe_path", Config->control_pipe_path);
	CXineConfigModify(Config->xine, "cxine.nowplay_pipe_path", Config->nowplay_pipe_path);
	CXineConfigModify(Config->xine, "cxine.background", Config->background);

	CXineConfigNumericModify(Config->xine, "cxine.audio_compression", Config->audio_compression);
	CXineConfigNumericModify(Config->xine, "cxine.show_osd", Config->flags & CONFIG_OSD);
	CXineConfigNumericModify(Config->xine, "cxine.persist", Config->flags & CONFIG_PERSIST);
	CXineConfigNumericModify(Config->xine, "cxine.bookmark", Config->flags & CONFIG_BOOKMARK);
	CXineConfigNumericModify(Config->xine, "cxine.disable_screensaver", Config->flags & DISABLE_SCREENSAVER);

  Tempstr=rstrcpy(Tempstr, xine_get_homedir());
	Tempstr=rstrcat(Tempstr, "/.cxine/cxine.conf");	
printf("SAVE: %s\n",Tempstr);
	xine_config_save(Config->xine, Tempstr);

	destroy(Tempstr);
}

