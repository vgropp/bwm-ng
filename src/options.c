/******************************************************************************
 *  bwm-ng handle cmdline and config file options                             *
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA *
 *                                                                            *
 *****************************************************************************/

#define EXTERN 
#include "global_vars.h"
#include "options.h"

#ifdef CONFIG_FILE
static char* getToken(char** str, const char* delims);
char *trim_whitespace(char *str);
int read_config(const char *config_file);
#endif
static inline int str2output_unit(char *optarg);
#if EXTENDED_STATS
static inline int str2output_type(char *optarg);
#endif
static inline int str2out_method(char *optarg);
static inline int str2in_method(char *optarg);

#ifdef CONFIG_FILE
 /******************************************************************************
 * This is a replacement for strsep which is not portable (missing on Solaris).
 */
static char* getToken(char** str, const char* delims) {
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

static inline int str2output_unit(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"bytes")) return BYTES_OUT;
        if (!strcasecmp(optarg,"bits")) return BITS_OUT;
        if (!strcasecmp(optarg,"packets")) return PACKETS_OUT;
        if (!strcasecmp(optarg,"errors")) return ERRORS_OUT;
    }
    return BYTES_OUT;
}

#if EXTENDED_STATS
static inline int str2output_type(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"rate")) return RATE_OUT;
        if (!strcasecmp(optarg,"max")) return MAX_OUT;
        if (!strcasecmp(optarg,"sum")) return SUM_OUT;
        if (!strcasecmp(optarg,"avg")) return AVG_OUT;
    }
    return RATE_OUT;
}
#endif  

static inline int str2out_method(char *optarg) {
    if (optarg) {
        if (!strcasecmp(optarg,"plain")) return PLAIN_OUT;
#ifdef HAVE_CURSES
            else
        if (!strcasecmp(optarg,"curses")) return CURSES_OUT;
	    else
        if (!strcasecmp(optarg,"curses2")) return CURSES2_OUT;
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


static inline int str2in_method(char *optarg) {
    if (optarg) {
#ifdef PROC_NET_DEV
        if (!strcasecmp(optarg,"proc")) return PROC_IN;
#endif
#ifdef NETSTAT
        if (!strcasecmp(optarg,"netstat")) return NETSTAT_IN;
#endif
#ifdef LIBSTATGRAB
        if (!strcasecmp(optarg,"libstat") || !strcasecmp(optarg,"statgrab") || !strcasecmp(optarg,"libstatgrab")) return LIBSTAT_IN;
		  if (!strcasecmp(optarg,"libstatdisk")) return LIBSTATDISK_IN;
#endif
#ifdef GETIFADDRS
        if (!strcasecmp(optarg,"getifaddrs")) return GETIFADDRS_IN;
#endif
#if DEVSTAT_IN
		  if (!strcasecmp(optarg,"devstat")) return DEVSTAT_IN;
#endif
#ifdef SYSCTL
        if (!strcasecmp(optarg,"sysctl")) return SYSCTL_IN;
#endif
#if SYSCTLDISK_IN
		  if (!strcasecmp(optarg,"sysctldisk")) return SYSCTLDISK_IN;
#endif
#ifdef PROC_DISKSTATS
		  if (!strcasecmp(optarg,"disk")) return DISKLINUX_IN;
#endif		  
#ifdef WIN32
		  if (!strcasecmp(optarg,"win32")) return WIN32_IN;
#endif			  
#ifdef HAVE_LIBKSTAT
			if (!strcasecmp(optarg,"kstat")) return KSTAT_IN;
			if (!strcasecmp(optarg,"kstatdisk")) return KSTATDISK_IN;
#endif 
#if IOSERVICE_IN
			if (!strcasecmp(optarg,"ioservice")) return IOSERVICE_IN;
#endif
    }
    return -1;
}


#ifdef CONFIG_FILE
char *trim_whitespace(char *str) {
char *dud = str;
int i;

   /* beginning whitespace first */
   while( (int)*dud && isspace((int)*dud) )
      ++dud;
   /* now trailing whitespace */
   i = strlen(dud) - 1;
   while( i>=0 && isspace((int)dud[i]) )
      --i;
   dud[i+1] = 0;
   return dud;
}


int read_config(const char *config_file) {
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
#ifdef PROC_DISKSTATS
    } else if( strcasecmp( token, "DISKSTATSFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_DISKSTATS_FILE,value);		  
    } else if( strcasecmp( token, "PARTITIONSFILE" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(PROC_PARTITIONS_FILE,value);		  
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    } else if( strcasecmp( token, "NETSTAT" ) == 0 ) {
        if (value && (strlen(value)<PATH_MAX)) strcpy(NETSTAT_FILE,value);
#endif
#endif
    } else if( strcasecmp( token, "INPUT" ) == 0 ) {
        if (value) input_method=str2in_method(value);
	 } else if( strcasecmp( token, "ANSIOUT" ) == 0 ) {
		 if (value) ansi_output=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "DYNAMIC" ) == 0 ) {
        if (value) dynamic=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "UNIT" ) == 0 ) {
        if (value) output_unit=str2output_unit(value);
#if EXTENDED_STATS
    } else if( strcasecmp( token, "TYPE" ) == 0 ) {
        if (value) output_type=str2output_type(value);
    } else if( strcasecmp( token, "AVGLENGTH" ) == 0 ) {
        if (value) avg_length=atoi(value)*1000;
#endif        
    } else if( strcasecmp( token, "ALLIF" ) == 0 ) {
        if (value) show_all_if=value[0];
    } else if( strcasecmp( token, "INTERFACES" ) == 0 ) {
        if (value) iface_list=(char *)strdup(value);
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
            if (!out_file) deinit(1, "failed to open outfile\n");
            if (out_file_path) free(out_file_path);
            out_file_path=(char *)strdup(value);
        }
#endif
    } else if( strcasecmp( token, "COUNT" ) == 0 ) {
        if (value) output_count=atol(value);
    } else if( strcasecmp( token, "DAEMON" ) == 0 ) {
        if (value) daemonize=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "SUMHIDDEN" ) == 0 ) {
        if (value) sumhidden=value[0]=='0' ? 0 : 1;
#if IOSERVICE_IN
	 } else if( strcasecmp( token, "LONGDISKNAMES" ) == 0) {
		 if (value) long_darwin_disk_names=value[0]=='0' ? 0 : 1;
#endif
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
    static struct passwd *pwd_entry;
    char *str;
#endif
#ifdef LONG_OPTIONS
    int option_index = 0;
    static struct option long_options[] = {
        {"timeout", 1, 0, 't'},
#ifdef PROC_NET_DEV
        {"procfile",1,0,'f'},
#endif
#ifdef PROC_DISKSTATS
			{"diskstatsfile",1,0,1000},
			{"partitionsfile",1,0,1001},
#endif		  
#if NETSTAT && ALLOW_NETSTATPATH
        {"netstat",1,0,'n'},
#endif
#if IOSERVICE_IN
		  {"longdisknames",0,0,1002},
#endif
        {"input",1,0,'i'},
        {"dynamic",1,0,'d'},
        {"help", 0, 0, 'h'},
        {"version",0,0,'V'},
        {"allif",1,0,'a'},
        {"unit",1,0,'u'},
		  {"ansiout",0,0,'N'},
#if EXTENDED_STATS        
        {"type",1,0,'T'},
        {"avglength",1,0,'A'},
#endif        
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
        {"htmlheader",1,0,'H'},
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
            snprintf(str,strlen(pwd_entry->pw_dir)+14,"%s/.bwm-ng.conf",pwd_entry->pw_dir);
            read_config(str);
            free(str);
        }
#endif    
    }
    /* reset getopt again  */
    optind=1;
#endif
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
                      exit(EXIT_FAILURE);
                      break;
            /* ugly workaround to handle optional arguments for all platforms */                      
            case ':': if (!strcmp(argv[optind-1],"-a") || !strcasecmp(argv[optind-1],"--allif")) 
                            show_all_if=1;
                      else if (!strcmp(argv[optind-1],"-d") || !strcasecmp(argv[optind-1],"--dynamic"))
                            dynamic=1;
                      else if (!strcmp(argv[optind-1],"-D") || !strcasecmp(argv[optind-1],"--daemon"))
                            daemonize=1;
#ifdef HTML                      
                      else if (!strcmp(argv[optind-1],"-H") || !strcasecmp(argv[optind-1],"--htmlheader"))
                            html_header=1;
#endif                      
                      else if (!strcmp(argv[optind-1],"-S") || !strcasecmp(argv[optind-1],"--sumhidden"))
                            sumhidden=1;    
                          else {
                              printf("%s requires an argument!\n",argv[optind-1]);
                              exit(EXIT_FAILURE);
                          }
                      break;
#ifdef PROC_DISKSTATS
			case 1000:
				if (strlen(optarg)<PATH_MAX) 
					strcpy(PROC_DISKSTATS_FILE,optarg);
				break;
         case 1001:
            if (strlen(optarg)<PATH_MAX)
               strcpy(PROC_PARTITIONS_FILE,optarg);
				break;
#endif				
#if IOSERVICE_IN
			case 1002:
				long_darwin_disk_names=!long_darwin_disk_names;
				break;
#endif
			case 'D':
				if (optarg) daemonize=atoi(optarg);
				break;
#ifdef HTML
			case 'R':
				if ((optarg) && atol(optarg)>0) { html_refresh=atol(optarg); }
				break;
			case 'H':
				if (optarg) html_header=atoi(optarg);
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
                    if (!out_file) deinit(1, "failed to open outfile\n");
                    if (out_file_path) free(out_file_path);
                    out_file_path=(char *)strdup(optarg);
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
                if (optarg) iface_list=(char *)strdup(optarg);
                break;
            case 'S':
                if (optarg) sumhidden=atoi(optarg);
                break;
            case 'o':
                if (optarg) {
                    output_method=str2out_method(optarg);
                }
                break;
				case 'N':
					 ansi_output=!ansi_output;
            case 'a':
                if (optarg) show_all_if=atoi(optarg);
                break;
            case 't':
                if ((optarg) && atol(optarg)>0) { delay=atol(optarg); }
                break;
#if EXTENDED_STATS                
            case 'T':
                output_type=str2output_type(optarg);
                break;
            case 'A':
                if (optarg) avg_length=atoi(optarg)*1000;
                break;
#endif                
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
                exit(EXIT_SUCCESS);
                break;
        }
    }
    if (iface_list==NULL && show_all_if==1) show_all_if=2;
#if EXTENDED_STATS    
    /* default init of avg_length */
    if (avg_length==0) {
        if (delay<AVG_LENGTH/2) 
            avg_length=AVG_LENGTH; 
        else  
            avg_length=(delay*2)+1;
    } else /* avg_length was set via cmdline or config file, better check it */
        if (delay*2>=avg_length) deinit(1, "avglength needs to be a least twice the value of timeout\n");
#endif    
	 if ((output_unit==ERRORS_OUT && !net_input_method(input_method)) || 
			 (output_unit==PACKETS_OUT && input_method==LIBSTATDISK_IN)) 
		output_unit=BYTES_OUT;
    return;
}

