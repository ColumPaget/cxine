/*
Copyright (c) 2019 Colum Paget <colums.projects@googlemail.com>
* SPDX-License-Identifier: GPL-3.0
*/

#include "plugins.h"



typedef const char *(*PLUGIN_DESC_FUNC)(xine_t *, const char *);

void DisplayPluginInfo(xine_t *xine, const char *title, const char *const *list, PLUGIN_DESC_FUNC description_func)
{
    const char *ptr=NULL;
    int i;

    printf("%s: \n", title);
    for (i=1; list[i] !=NULL; i++)
    {
        //for some reason looking up a description for xvmc causes a segfault
        if (strcmp(list[i], "xvmc")==0) ptr="";
        //for some reason looking up a description for opengl2 causes a segfault
        else if (strcmp(list[i], "opengl2")==0) ptr="";
        else ptr=description_func(xine, list[i]);
        if (! ptr) ptr="";
        printf("	%s - %s\n",list[i], ptr);
    }

    printf("\n");
}



void CXineDisplayPlugins(TConfig *Config)
{
    DisplayPluginInfo(Config->xine, "audio", xine_list_audio_output_plugins(Config->xine), (PLUGIN_DESC_FUNC) xine_get_audio_driver_plugin_description);
    DisplayPluginInfo(Config->xine, "video", xine_list_video_output_plugins(Config->xine),  (PLUGIN_DESC_FUNC) xine_get_video_driver_plugin_description);
    DisplayPluginInfo(Config->xine, "demux", xine_list_demuxer_plugins(Config->xine),  (PLUGIN_DESC_FUNC) xine_get_demux_plugin_description);
    DisplayPluginInfo(Config->xine, "input", xine_list_input_plugins(Config->xine),  (PLUGIN_DESC_FUNC) xine_get_input_plugin_description);
    DisplayPluginInfo(Config->xine, "postprocess", xine_list_post_plugins(Config->xine),  (PLUGIN_DESC_FUNC) xine_get_post_plugin_description);
}


void CXineListPluginApi(xine_post_t *plug)
{
    xine_post_in_t *plug_input;
    xine_post_api_t *api;
    xine_post_api_descr_t *api_descr;
    xine_post_api_parameter_t *parm;

    plug_input = (xine_post_in_t *) xine_post_input(plug, "parameters");
    if (plug_input)
    {
        api = (xine_post_api_t *) plug_input->data;

        api_descr = api->get_param_descr();
        parm=api_descr->parameter;
        while (parm->type != POST_PARAM_TYPE_LAST)
        {
            parm++;
        }
    }


}


void CXineClearAudioPlugins(TConfig *Config)
{
    xine_post_out_t *audio_out;

    audio_out =xine_get_audio_source(Config->stream);
    xine_post_wire(audio_out, Config->ao_port);
}


void CXineAddAudioPostPlugins(TConfig *Config)
{
    xine_post_t *plug;
    xine_post_in_t *plug_input;
    xine_post_out_t *audio_out;
    char *name=NULL;
    const char *ptr;

    ptr=rstrtok(Config->audio_plugins, ",", &name);
    while (ptr)
    {
        plug=xine_post_init(Config->xine, name, 0, &Config->ao_port, &Config->vo_port);
        //printf("audio post-process plugin: %s\n", name);
        if (plug)
        {
            audio_out =xine_get_audio_source(Config->stream);
            plug_input=xine_post_input(plug, "audio in");
            xine_post_wire(audio_out, plug_input);
        }
        else fprintf(stderr, "ERROR: Failed to load postprocess plugin '%s'\n",name);

        ptr=rstrtok(ptr, ",", &name);
    }

    destroy(name);
}


void CXineAddVideoPostPlugins(xine_t *xine, xine_stream_t *stream, xine_audio_port_t **ao_port, xine_video_port_t **vo_port)
{
    xine_post_t *plug;
    xine_post_in_t *plug_input;
    xine_post_out_t *video_out;

    plug=xine_post_init(xine, "invert", 0, ao_port, vo_port);
    video_out =xine_get_video_source(stream);
    plug_input=xine_post_input(plug, "video in");
    xine_post_wire(video_out, plug_input);
}

