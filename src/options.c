/******************************************************************************
 *  bwm-ng handle cmdline and config file options                             *
 *                                                                            *
 *  Copyright (C) 2004 Volker Gropp (vgropp@pefra.de)                         *
 *                                                                            *
 *  for more info read README.                                                *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                            *
 *****************************************************************************/


#include "options.h"

#ifdef CONFIG_FILE
 /******************************************************************************
 * This is a replacement for strsep which is not portable (missing on Solaris).
 */
static char* getToken(char** str, const char* delims)
{
    char* token;

    if (*str==NULL) {
        /* No more tokens */
        return NULL;
    }

    token=*str;
    while (**str!='\0') {
        if (strchr(delims,**str)!=NULL) {
            **str='\0';
            (*str)++;
            return token;
        }
        (*str)++;
    }
    /* There is no other token */
    *str=NULL;
    return token;
}
/******************************************************************************/
#endif

inline int str2output_unit(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"bytes")) return BYTES_OUT;
        if (!strcasecmp(optarg,"bits")) return BITS_OUT;
        if (!strcasecmp(optarg,"packets")) return PACKETS_OUT;
        if (!strcasecmp(optarg,"errors")) return ERRORS_OUT;
    }
    return BYTES_OUT;
}

inline int str2output_type(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"rate")) return RATE_OUT;
        if (!strcasecmp(optarg,"max")) return MAX_OUT;
        if (!strcasecmp(optarg,"sum")) return SUM_OUT;
        if (!strcasecmp(optarg,"avg")) return AVG_OUT;
    }
    return RATE_OUT;
}
                    

inline int str2out_method(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"plain")) return PLAIN_OUT;
#ifdef HAVE_CURSES
            else
        if (!strcasecmp(optarg,"curses")) return CURSES_OUT;
#endif
#ifdef CSV
            else
        if (!strcasecmp(optarg,"csv")) return CSV_OUT;
#endif
#ifdef HTML
        else
        if (!strcasecmp(optarg,"html")) return HTML_OUT;
#endif
    }
    return -1;
}


inline int str2in_method(char *optarg) {
    if (optarg) {
#ifdef PROC_NET_DEV
        if (!strcasecmp(optarg,"proc")) return PROC_IN;
#endif
#ifdef NETSTAT
        if (!strcasecmp(optarg,"netstat")) return NETSTAT_IN;
#endif
#ifdef LIBSTATGRAB
        if (!strcasecmp(optarg,"libstat") || !strcasecmp(optarg,"statgrab") || !strcasecmp(optarg,"libstatgrab")) return LIBSTAT_IN;
#endif
#ifdef GETIFADDRS
        if (!strcasecmp(optarg,"getifaddrs")) return GETIFADDRS_IN;
#endif
#ifdef SYSCTL
        if (!strcasecmp(optarg,"sysctl")) return SYSCTL_IN;
#endif
    }
    return -1;
}


#ifdef CONFIG_FILE
char *trim_whitespace(char *str) {
char *dud = str;
int i;

   /* beginning whitespace first */
   while( isspace((int)*dud) )
      ++dud;
   /* now trailing whitespace */
   i = strlen(dud) - 1;
   while( isspace((int)dud[i]) )
      --i;
   dud[i+1] = 0;
   return dud;
}


int read_config(char *config_file) {
FILE *fp;
char *buffer;
char *token, *value;

  if (config_file==NULL) return -1;

  if( (fp = fopen( config_file, "r" ) ) == NULL ) {
     return -1;
  }

  buffer = (char*)malloc( sizeof(char) * 4096 );

  while( fgets( buffer, 4096, fp ) ) {
    value = trim_whitespace( buffer );
    token = getToken( &value, "=" );
    if( token == NULL )  /* ignore this line if there isn't a token/value pair */
        continue;
    token = trim_whitespace( token );

    if( strcasecmp( token, "TIMEOUT" ) == 0 ) {
        if (value && atol(value)>0) { delay=atol(value); }
#ifdef PROC_NET_DEV
    } else if( strcasecmp( token, "PROCFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_FILE,value);
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    } else if( strcasecmp( token, "NETSTAT" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(NETSTAT_FILE,value);
#endif
#endif
    } else if( strcasecmp( token, "INPUT" ) == 0 ) {
        if (value) input_method=str2in_method(value);
    } else if( strcasecmp( token, "DYNAMIC" ) == 0 ) {
        if (value) dynamic=value[0]=='0' ? 1 : 0;
    } else if( strcasecmp( token, "UNIT" ) == 0 ) {
        if (value) output_unit=str2output_unit(value);
    } else if( strcasecmp( token, "TYPE" ) == 0 ) {
        if (value) output_type=str2output_type(value);
    } else if( strcasecmp( token, "ALLIF" ) == 0 ) {
        if (value) show_all_if=value[0];
    } else if( strcasecmp( token, "INTERFACES" ) == 0 ) {
        if (value) iface_list=strdup(value);
    } else if( strcasecmp( token, "OUTPUT" ) == 0 ) {
        if (value) output_method=str2out_method(value);
#ifdef CSV
    } else if( strcasecmp( token, "CSVCHAR" ) == 0 ) {
        if (value) csv_char=value[0];
#endif
#if CSV || HTML
    } else if( strcasecmp( token, "OUTFILE" ) == 0 ) {
        if (value) { 
            if (out_file) fclose(out_file);
            out_file=fopen(value,"a"); 
            if (out_file_path) free(out_file_path);
            out_file_path=strdup(value);
        }
#endif
    } else if( strcasecmp( token, "COUNT" ) == 0 ) {
        if (value) output_count=atol(value);
    } else if( strcasecmp( token, "DAEMON" ) == 0 ) {
        if (value) daemonize=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "SUMHIDDEN" ) == 0 ) {
        if (value) sumhidden=value[0]=='0' ? 0 : 1;
#ifdef HTML
    } else if( strcasecmp( token, "HTMLREFRESH" ) == 0 ) {
        if (value && atol(value)>0) { html_refresh=atol(value); }
    } else if( strcasecmp( token, "HTMLHEADER" ) == 0 ) {
        if (value) html_header=value[0]=='0' ? 0 : 1;
#endif
    }
  }
  free(buffer);
  fclose(fp);

 return 0;
}
#endif



void get_cmdln_options(int argc, char *argv[]) {
	int o;
#if CONFIG_FILE && HAVE_GETPWUID
    struct passwd *pwd_entry;
    char *str;
#endif
#ifdef LONG_OPTIONS
    int option_index = 0;
    static struct option long_options[] = {
        {"timeout", 1, 0, 't'},
#ifdef PROC_NET_DEV
        {"procfile",1,0,'f'},
#endif
#if NETSTAT && ALLOW_NETSTATPATH
        {"netstat",1,0,'n'},
#endif
        {"input",1,0,'i'},
        {"dynamic",1,0,'d'},
        {"help", 0, 0, 'h'},
        {"version",0,0,'V'},
        {"allif",1,0,'a'},
        {"unit",1,0,'u'},
        {"type",1,0,'T'},
        {"interfaces",1,0,'I'},
        {"sumhidden",1,0,'S'},
        {"output",1,0,'o'},
#ifdef CSV
        {"csvchar",1,0,'C'},
        {"csvfile",1,0,'F'},
#endif
        {"count",1,0,'c'},
        {"daemon",1,0,'D'},
#ifdef HTML
        {"htmlrefresh",1,0,'R'},
        {"htmlheader",0,0,'H'},
#endif
        {0,0,0,0}
    };
#endif
#ifdef CONFIG_FILE
    /* loop till first non option argument */
    opterr=0;
    while (1) {
#ifdef LONG_OPTIONS
        o=getopt_long (argc,argv,SHORT_OPTIONS,long_options, &option_index);
#else
        o=getopt (argc,argv,SHORT_OPTIONS);
#endif
        if (o==-1) break;
    }
    opterr=1;
    if (optind < argc) {
        read_config(argv[optind]);
    } else {
        read_config("/etc/bwm-ng.conf");
#ifdef HAVE_GETPWUID    
        pwd_entry=getpwuid(getuid());
        if (pwd_entry!=NULL) {
            str=(char*)malloc(strlen(pwd_entry->pw_dir)+14);
            snprintf(str,strlen(pwd_entry->pw_dir)+13,"%s/.bwm-ng.conf",pwd_entry->pw_dir);
            read_config(str);
            free(str);
        }
#endif    
    }
#endif
    /* reset getopt again  */
    optind=1;
    /* get command line arguments, kinda ugly, wanna rewrite it? */
    while (1) {
#ifdef LONG_OPTIONS
        o=getopt_long (argc,argv,SHORT_OPTIONS,long_options, &option_index);
#else
		o=getopt (argc,argv,SHORT_OPTIONS);
#endif		
        if (o==-1) break;
        switch (o) {
            case '?': printf("unknown option: %s\n",argv[optind-1]);
                      exit(0);
                      break;
            /* ugly workaround to handle optional arguments for all platforms */                      
            case ':': if (!strcmp(argv[optind-1],"-a") || !strcasecmp(argv[optind-1],"--allif")) 
                            show_all_if=1;
                      else if (!strcmp(argv[optind-1],"-d") || !strcasecmp(argv[optind-1],"--dynamic"))
                            dynamic=1;
                      else if (!strcmp(argv[optind-1],"-D") || !strcasecmp(argv[optind-1],"--daemon"))
                            daemon=1;
                      else if (!strcmp(argv[optind-1],"-S") || !strcasecmp(argv[optind-1],"--sumhidden"))
                            sumhidden=1;    
                          else {
                              printf("%s requires an argument!\n",argv[optind-1]);
                              exit(1);
                          }
                      }
                      break;
			case 'D':
				if (optarg) daemonize=atoi(optarg);
				break;
#ifdef HTML
			case 'R':
				if ((optarg) && atol(optarg)>0) { html_refresh=atol(optarg); }
				break;
			case 'H':
				html_header=1;
				break;
#endif
			case 'c':
				if (optarg) output_count=atol(optarg);
				break;
#if CSV || HTML
            case 'F':
                if (optarg) { 
                    if (out_file) fclose(out_file);
                    out_file=fopen(optarg,"a"); 
                    if (out_file_path) free(out_file_path);
                    out_file_path=strdup(optarg);
                }
                break;
#endif
#ifdef CSV
			case 'C':
				if (optarg) csv_char=optarg[0];
				break;
#endif
            case 'h':
                cmdln_printhelp();
                break;
#ifdef PROC_NET_DEV
			case 'f':
                if (optarg && (strlen(optarg)<PATH_MAX)) strcpy(PROC_FILE,optarg);
                break;
#endif			
			case 'i':
                if (optarg) {
                    input_method=str2in_method(optarg);
                }
				break;				
            case 'I':
                if (optarg) iface_list=strdup(optarg);
                break;
            case 'S':
                if (optarg) sumhidden=atoi(optarg);
                break;
            case 'o':
                if (optarg) {
                    output_method=str2out_method(optarg);
                }
                break;
            case 'a':
                if (optarg) show_all_if=atoi(optarg);
                break;
            case 't':
                if ((optarg) && atol(optarg)>0) { delay=atol(optarg); }
                break;
            case 'T':
                output_type=str2output_type(optarg);
                break;
            case 'd':
                if (optarg) dynamic=atoi(optarg);
                break;
            case 'u':
                output_unit=str2output_unit(optarg);
                break;
#if NETSTAT && ALLOW_NETSTATPATH
            case 'n':
                if (optarg && (strlen(optarg)<PATH_MAX)) strcpy(NETSTAT_FILE,optarg);
                break;
#endif                
            case 'V':
                print_version;
                exit(0);
                break;
        }
    }
    if (iface_list==NULL && show_all_if==1) show_all_if=2;
    return;
}

