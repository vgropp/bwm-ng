/******************************************************************************
 *  bwm-ng parsing stuff                                                      *
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


/* prints a helpscreen and exists */
int printhelp() {
    print_version;
    printf("USAGE: bwm-ng [OPTION] ...");
#if CONFIG_FILE    
    printf(" [CONFIGFILE]\n");
#else
    printf("\n");
#endif    
    printf("displays current ethernet interfaces load in KB/s or Packets/s\n\n");
    printf("Options:\n");
#ifdef LONG_OPTIONS
    printf("  -t, --timeout <msec>    displays stats every <msec> (1msec = 1/1000sec)\n");
    printf("                          default: 500\n");
    printf("  -d, --dynamic           show values dynamicly (Byte KB or MB)\n");
    printf("  -a, --allif [mode]      where mode is one of:\n");
    printf("                          0=show only up (and selected) interfaces\n");
    printf("                          1=show all up interfaces (default)\n");
    printf("                          2=show all and down interfaces\n");
    printf("  -p, --packets           show packets/s instead of KB/s\n");
    printf("  -I, --interfaces <list> show only interfaces in <list> (comma seperated), or\n");
    printf("                          if list is prefaced with %% show all but interfaces\n");
    printf("                          in list\n");
    printf("  -S, --sumhidden         count hidden interfaces for total\n");
    printf("  -D, --daemon            fork into background and daemonize\n");
    printf("  -h, --help              displays this help\n");
    printf("  -V, --version           print version info\n");
	printf("\nInput:\n");
	printf("  -i, --input <method>    input method, one of:" INPUT_METHODS "\n");
#ifdef PROC_NET_DEV
    printf("  -f, --procfile <file>   filename to read raw data from. (" PROC_NET_DEV ")\n");
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    printf("  -n, --netstat <path>    use <path> as netstat binary\n");
#endif	
#endif    
	printf("\nOutput:\n");
    printf("  -o, --output <method>   output method, one of: " OUTPUT_METHODS "\n");
#ifdef CSV
    printf("  -C, --csvchar <char>    delimiter for csv\n");
	printf("  -F, --csvfile <file>    output file for csv (default stdout)\n");
#endif
#ifdef HTML
	printf("  -R, --htmlrefresh <num> meta refresh for html output\n");
	printf("  -H, --htmlheader        show <html> and <meta> frame for html output\n");
#endif
    printf("  -c, --count <num>       number of query/output for plain & csv\n");
    printf("                          (ie 1 for one single output)\n");
#else
    printf("  -t <msec>               displays stats every <msec> (1msec = 1/1000sec)\n");
    printf("                          default: 500\n");
    printf("  -d                      show values dynamicly (Byte KB or MB)\n");
    printf("  -a, [mode]              where mode is one of:\n");
    printf("                          0=show only up (and selected) interfaces\n");
    printf("                          1=show all up interfaces (default)\n");
    printf("                          2=show all and down interfaces\n");
    printf("  -p                      show packets/s instead of KB/s\n");
    printf("  -I <list>               show only interfaces in <list> (comma seperated), or\n");
    printf("                          if list is prefaced with %% show all but interfaces\n");
    printf("                          in list\n");
    printf("  -S                      count hidden interfaces for total\n");
    printf("  -D                      fork into background and daemonize\n");
    printf("  -h                      displays this help\n");
    printf("  -V                      print version info\n");
    printf("\nInput:\n");
    printf("  -i <method>             input method, one of:" INPUT_METHODS "\n");
#ifdef PROC_NET_DEV
    printf("  -f <file>               filename to read raw data from. (" PROC_NET_DEV ")\n");
#endif
#if ALLOW_NETSTATPATH    
#ifdef NETSTAT
    printf("  -n <path>               use <path> as netstat binary\n");
#endif
#endif    
    printf("\nOutput:\n");
    printf("  -o <method>             output method, one of: " OUTPUT_METHODS "\n");
#ifdef CSV
    printf("  -C <char>               delimiter for csv\n");
    printf("  -F <file>               output file for csv (default stdout)\n");
#endif
#ifdef HTML
    printf("  -R <num>                meta refresh for html output\n");
	printf("  -H, --htmlheader        show <html> and <meta> frame for html output\n");
#endif
    printf("  -c <num>                number of query/output for plain & csv\n");
    printf("                          (ie 1 for one single output)\n");
#endif
    printf("\n");
    exit(0);
}


int str2out_method(char *optarg) {
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
    return 0;
}


int str2in_method(char *optarg) {
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
    return 0;
}


#ifdef CONFIG_FILE
char *trim_whitespace(char *str) {
char *dud = str;
int i;

   // beginning whitespace first
   while( isspace((int)*dud) )
      ++dud;
   // now trailing whitespace
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
        if (value) show_kb=value[0]=='0' ? 1 : 0;
    } else if( strcasecmp( token, "ALLIF" ) == 0 ) {
        if (value) show_all_if=value[0];
    } else if( strcasecmp( token, "PACKETS" ) == 0 ) {
        if (value) show_packets=value[0]=='0' ? 0 : 1;
    } else if( strcasecmp( token, "INTERFACES" ) == 0 ) {
        if (value) iface_list=strdup(value);
    } else if( strcasecmp( token, "OUTPUT" ) == 0 ) {
        if (value) output_method=str2out_method(value);
#ifdef CSV
    } else if( strcasecmp( token, "CSVCHAR" ) == 0 ) {
        if (value) csv_char=value[0];
    } else if( strcasecmp( token, "CSVFILE" ) == 0 ) {
        if (value) { 
            if (csv_file) fclose(csv_file);
            csv_file=fopen(value,"a"); 
        }
#endif
    } else if( strcasecmp( token, "COUNT" ) == 0 ) {
        if (value) output_count=atol(value);
    } else if( strcasecmp( token, "DAEMON" ) == 0 ) {
        if (value) daemonize=value[0]=='0' ? 0 : 1;
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
#ifdef CONFIG_FILE    
#ifdef HAVE_GETPWUID
    struct passwd *pwd_entry;
    char *str;
#endif    
#endif
#ifdef LONG_OPTIONS
    int option_index = 0;
    static struct option long_options[] = {
        {"timeout", 1, 0, 't'},
#ifdef PROC_NET_DEV
        {"procfile",1,0,'f'},
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
        {"netstat",1,0,'n'},
#endif
#endif
        {"input",1,0,'i'},
        {"dynamic",0,0,'d'},
        {"help", 0, 0, 'h'},
        {"version",0,0,'V'},
        {"allif",1,0,'a'},
        {"packets",0,0,'p'},
        {"interfaces",1,0,'I'},
        {"sumhidden",0,0,'S'},
        {"output",1,0,'o'},
#ifdef CSV
        {"csvchar",1,0,'C'},
        {"csvfile",1,0,'F'},
#endif
        {"count",1,0,'c'},
        {"daemon",0,0,'D'},
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
    if (optind < argc) {
        read_config(argv[optind]);
    }
#endif
    optind=0; //reset getopt again
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
            case ':': if (!strcmp(argv[optind-1],"-a") || !strcasecmp(argv[optind-1],"--allif")) 
                          show_all_if=1;
                      else {
                          printf("%s requires an argument!\n",argv[optind-1]);
                          exit(1);
                      }
                      break;
			case 'D':
				daemonize=1;
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
#ifdef CSV
            case 'F':
                if (optarg) { 
                    if (csv_file) fclose(csv_file);
                    csv_file=fopen(optarg,"a"); 
                }
                break;
			case 'C':
				if (optarg) csv_char=optarg[0];
				break;
#endif
            case 'h':
                printhelp();
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
                sumhidden=1;
                break;
            case 'o':
                if (optarg) {
                    output_method=str2out_method(optarg);
                }
                break;
            case 'a':
                if (optarg) show_all_if=atoi(optarg);
                break;
            case 'p':
                show_packets=1;
                break;
            case 't':
                if ((optarg) && atol(optarg)>0) { delay=atol(optarg); }
                break;
            case 'd':
                show_kb=0;
                break;
#if ALLOW_NETSTATPATH
#ifdef NETSTAT                
            case 'n':
                if (optarg && (strlen(optarg)<PATH_MAX)) strcpy(NETSTAT_FILE,optarg);
                break;
#endif
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

