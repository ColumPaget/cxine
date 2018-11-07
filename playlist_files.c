#include "playlist_files.h"
#include "playlist.h"
#include "xine/xmlparser.h"

void PLSPlaylistLoad(TStringList *List, const char *MRL)
{
FILE *f;
char *Tempstr=NULL, *Token=NULL, *Quoted=NULL;
const char *ptr;

f=fopen(MRL, "r");
if (f)
{
	Tempstr=realloc(Tempstr, 256);
	while (fgets(Tempstr, 255, f))
	{
		Tempstr=xine_chomp(Tempstr);
		ptr=rstrtok(Tempstr, "=", &Token);
		if (strncmp(Token, "File", 4)==0) 
		{
			PlaylistAdd(ptr, "");
		}
	}
	fclose(f);
}

destroy(Tempstr);
destroy(Quoted);
destroy(Token);
}


void M3UPlaylistLoad(TStringList *List, const char *MRL)
{
FILE *f;
char *Tempstr=NULL, *Quoted=NULL;

f=fopen(MRL, "r");
if (f)
{
	Tempstr=realloc(Tempstr, 256);
	while (fgets(Tempstr, 255, f))
	{
		Tempstr=xine_chomp(Tempstr);

		if ( StrLen(Tempstr) && (*Tempstr != '#') ) PlaylistAdd(Tempstr, "");
	}
	fclose(f);
}

destroy(Tempstr);
destroy(Quoted);
}


xml_node_t *PlayListXMLLoad(const char *MRL)
{
	struct stat FStat;
	char *Tempstr=NULL;
  xml_node_t *root=NULL;
  xml_parser_t *parser;
	FILE *f;

	stat(MRL, &FStat);
	Tempstr=calloc(FStat.st_size +20, sizeof(char));
	f=fopen(MRL, "r");
	fread(Tempstr, FStat.st_size, sizeof(char), f);
	fclose(f);

	printf("PARSE: %s\n", Tempstr);
  parser = xml_parser_init_r (Tempstr, FStat.st_size, XML_PARSER_CASE_INSENSITIVE);
	if (parser)
	{
  xml_parser_build_tree_r(parser, &root);
  xml_parser_finalize_r (parser);
	}

	destroy(Tempstr);

	return(root);
}



void XSPFPlaylistParseItem(TStringList *List, xml_node_t *item)
{
  xml_node_t *node;
	const char *p_title=NULL, *p_path=NULL;
	char *Tempstr=NULL;

	for (node = item->child; node != NULL; node = node->next) 
	{
		if (strcasecmp(node->name, "title")==0) p_title = node->data;
		else if (strcasecmp(node->name, "location")==0) p_path = node->data;
	}

	if (p_path) PlaylistAdd(p_path, p_title);

	destroy(Tempstr);
}


void XSPFPlaylistParseTracklist(TStringList *List, xml_node_t *parent) 
{
  xml_node_t *node;

  for (node = parent->child; node; node = node->next) 
	{
       if (strcasecmp (node->name, "tracklist")==0) XSPFPlaylistParseTracklist(List, node);
			 else if (strcasecmp (node->name, "track")==0) XSPFPlaylistParseItem(List, node);
   }
}

void XSPFPlaylistLoad(TStringList *List, const char *MRL) 
{
  xml_node_t *root;

	root=PlayListXMLLoad(MRL);
	if (root)
	{
  if (strcasecmp (root->name, "playlist")==0) XSPFPlaylistParseTracklist(List, root); 
  xml_parser_free_tree(root);
	}
}



void ASXPlaylistParseItem(TStringList *List, xml_node_t *item)
{
  xml_node_t *node;
	const char *p_title=NULL, *p_path=NULL;
	char *Tempstr=NULL;

	for (node = item->child; node != NULL; node = node->next) 
	{
		printf("ASX: %s\n", node->name);
		if (strcasecmp(node->name, "title")==0) p_title = node->data;
		else if (strcasecmp(node->name, "ref")==0) p_path = xml_parser_get_property (node, "href");
	}

	if (p_path) PlaylistAdd(p_path, p_title);

	destroy(Tempstr);
}


void ASXPlaylistParseTracklist(TStringList *List, xml_node_t *parent) 
{
  xml_node_t *node;

  for (node = parent->child; node; node = node->next) 
	{
			if (strcasecmp (node->name, "entry")==0) ASXPlaylistParseItem(List, node);
   }
}

void ASXPlaylistLoad(TStringList *List, const char *MRL) 
{
  xml_node_t *root;

	root=PlayListXMLLoad(MRL);
  if (root)
	{
		if (strcasecmp(root->name, "asx")==0) ASXPlaylistParseTracklist(List, root); 
		xml_parser_free_tree(root);
	}
}


int IsPlaylist(const char *MRL)
{
const char *ptr;

ptr=strrchr(MRL, '.');
if (ptr)
{
if (strcasecmp(ptr, ".pls")==0) return(TRUE);
else if (strcasecmp(ptr, ".xspf")==0) return(TRUE);
else if (strcasecmp(ptr, ".m3u")==0) return(TRUE);
else if (strcasecmp(ptr, ".asx")==0) return(TRUE);
}

return(FALSE);
}



int PlaylistLoad(TStringList *List, const char *MRL)
{
const char *ptr;

ptr=strrchr(MRL, '.');
if (ptr && (strcasecmp(ptr, ".pls")==0)) PLSPlaylistLoad(List, MRL);
else if (ptr && (strcasecmp(ptr, ".xspf")==0)) XSPFPlaylistLoad(List, MRL);
else if (ptr && (strcasecmp(ptr, ".m3u")==0)) M3UPlaylistLoad(List, MRL);
else if (ptr && (strcasecmp(ptr, ".asx")==0)) ASXPlaylistLoad(List, MRL);
else return(FALSE);

return(TRUE);
}
