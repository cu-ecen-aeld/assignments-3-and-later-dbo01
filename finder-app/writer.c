#include <stdio.h>
#include <syslog.h>


int main(int argc, char **argv)
{
	char* str_content;
	char* f_name;
	FILE *fp = NULL;

	openlog(NULL, 0, LOG_USER);

	if (argc < 2)
	{
		syslog(LOG_ERR, "Wrong number of arguments %d", argc );
		return 1;
	}
	else
	{
		str_content = argv[2];
		f_name = argv[1];

		if((str_content == NULL) || (f_name == NULL))
		{
			syslog(LOG_ERR, "Wrong argument");
			return 1;
		}
		else
		{
			syslog(LOG_DEBUG, "Writing %s to %s", str_content, f_name );

        	fp = fopen(f_name ,"w");

			if ( fp != NULL)
			{
				if (0 > fprintf(fp, "%s", str_content))
				{
					syslog(LOG_DEBUG, "Writing to file failed" );
					return 1;
				}
				fclose(fp);
				return 0;
			}
			else
			{
				return 1;
			}

		}
	}




	printf("writer done\n");
}
