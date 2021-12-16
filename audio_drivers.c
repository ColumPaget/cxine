#include "audio_drivers.h"
#include "plugins.h"
#include "config.h"
#include <ctype.h>

/*
# device used for mono output
# string, default: default
#audio.device.alsa_default_device:default

# device used for stereo output
# string, default: plug:front:default
#audio.device.alsa_front_device:plug:front:default

# sound card can do mmap
# bool, default: 0
#audio.device.alsa_mmap_enable:0

# device used for 5.1-channel output
# string, default: iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2
#audio.device.alsa_passthrough_device:iec958:AES0=0x6,AES1=0x82,AES2=0x0,AES3=0x2

# device used for 4-channel output
# string, default: plug:surround40:0
#audio.device.alsa_surround40_device:plug:surround40:0

# device used for 5.1-channel output
# string, default: plug:surround51:0
#audio.device.alsa_surround51_device:plug:surround51:0

# OSS audio device name
# { auto  /dev/dsp  /dev/sound/dsp }, default: 0
#audio.device.oss_device_name:auto
*/


//parse an alsa device name. This can either be a number, which we map to hw:1 hw:2 etc
//or it can be a device name, or it can be an mplayer-style device value
char *CXineParseAlsaDevice(char *RetStr, const char *DevString)
{
const char *ptr;

						ptr=DevString;
            if (! StrLen(ptr)) RetStr=rstrcpy(RetStr, "plug:default");
            else if (isdigit(*ptr))
            {
                RetStr=rstrcpy(RetStr,"hw:");
                RetStr=rstrcat(RetStr,ptr);
            }
						else if (strncmp(ptr, "device=", 7)==0)
						{
								ptr+=7;
								if (strncmp(ptr, "hw=", 3)==0) 
								{
									RetStr=rstrcpy(RetStr, "hw:");
									RetStr=rstrcat(RetStr, ptr+3);
								}
								else 
								{
									RetStr=rstrcpy(RetStr, ptr);
								}
						} 
            else RetStr=rstrcpy(RetStr, ptr);

return(RetStr);
}

xine_audio_port_t *CXineOpenAudioDriver(const char *Spec)
{
    char *Type=NULL, *Tempstr=NULL;
    xine_audio_port_t *ao_port=NULL, *ao_old=NULL;
    const char *ptr;
    static const char * const devname_opts[] = {"auto", "/dev/dsp", "/dev/sound/dsp", NULL};

    ptr=rstrtok(Spec,":",&Type);

    if (StrLen(ptr))
    {
        if (strcmp(Type, "oss")==0)
        {
            xine_config_register_enum(Config->xine, "audio.device.oss_device_name", 1, (char **)devname_opts, "OSS audio device name","", 10, NULL, NULL);

            //libxine's handling of oss is rather broken. The device number is just appended to the device name, so a device number of '0' means '/dev/dsp0'. But the kernel tends to call that device '/dev/dsp'. Hence we have to pass -1, that signals 'no device number' to use '/dev/dsp'. But what if /dev/dsp0 really exists? So we have to check for that scenario too.
            if (strcmp(ptr, "0")==0)
            {
                if (access("/dev/dsp0", F_OK) !=0) ptr="-1";
            }

            CXineConfigModifyOrCreate(Config->xine, "num:audio.device.oss_device_number", ptr, "OSS audio device number, -1 for none");
        }
        else if (strcmp(Type, "alsa")==0)
        {
						Tempstr=CXineParseAlsaDevice(Tempstr, ptr);
printf("ALSA: [%s]\n", Tempstr);
            CXineConfigModifyOrCreate(Config->xine, "audio.device.alsa_default_device", Tempstr, "ALSA device for mono output");
            CXineConfigModifyOrCreate(Config->xine, "audio.device.alsa_front_device", Tempstr, "ALSA device for stereo output");
        }
        else if (strcmp(Type, "jack")==0)
        {
            if (StrLen(ptr)) CXineConfigModifyOrCreate(Config->xine, "audio.device.jack_device_name", ptr, "Jack output device");
        }
        else if (strcmp(Type, "sun")==0)
        {
            xine_config_register_filename(Config->xine, "audio.device.sun_audio_device", ptr, XINE_CONFIG_STRING_IS_DEVICE_NAME, "Sun output device","", XINE_CONFIG_SECURITY, NULL, NULL);
        }
        else if (strcmp(Type, "pulse")==0)
        {
            if (StrLen(ptr)) CXineConfigModifyOrCreate(Config->xine, "audio.device.pulseaudio_device", ptr, "Pulseaudio output device");
        }


    }

    ao_port=xine_open_audio_driver(Config->xine, Type, NULL);
    if (! ao_port) fprintf(stderr, "ERROR: unable to open audio driver '%s'\n", Spec);
    else
    {
        ao_old=Config->ao_port;
        Config->ao_port=ao_port;
        if (Config->stream) xine_post_wire_audio_port(xine_get_audio_source(Config->stream), Config->ao_port);
        if (ao_old) xine_close_audio_driver(Config->xine, ao_old);
    }

    destroy(Tempstr);
    destroy(Type);

    return(ao_port);
}


//returns true of stream is audio only
static int CXineAudioOnly(TConfig *Config)
{
    if  (! xine_get_stream_info(Config->stream, XINE_STREAM_INFO_HAS_AUDIO)) return(FALSE);
    if (Config->flags & CONFIG_NOVIDEO) return(TRUE);
    if (! xine_get_stream_info(Config->stream, XINE_STREAM_INFO_HAS_VIDEO)) return(TRUE);
    return(FALSE);
}



xine_audio_port_t *CXineOpenAudio()
{
    xine_audio_port_t *ao_port=NULL;
    char *Token=NULL;
    const char *ptr;


    CXineOpenAudioDriver("none");
    if (Config->ao_curr==NULL) Config->ao_curr=Config->ao_driver;
    ptr=rstrtok(Config->ao_curr, ",", &Token);
    while (ptr)
    {
        ao_port=CXineOpenAudioDriver(Token);
        if (ao_port)
        {
            printf("audio out: %s\n", Token);
            break;
        }

        //if we get here we failed to open the audio output, so update curr to point to the next one
        Config->ao_curr=ptr;
        ptr=rstrtok(Config->ao_curr,",",&Token);
    }

    if (CXineAudioOnly(Config)) CXineAddAudioPostPlugins(Config);

    destroy(Token);
    return(ao_port);
}

void CXineCloseAudio()
{
    CXineOpenAudioDriver("none");
}

xine_audio_port_t *CXineCycleAudio()
{
    char *Token=NULL;

    Config->ao_curr=rstrtok(Config->ao_curr, ",", &Token);
    if (StrLen(Config->ao_curr)==0) Config->ao_curr=Config->ao_driver;

    destroy(Token);

    return(CXineOpenAudio());
}


