#include "g_local.h"

#define MAX_INI_STR_LEN 128
#define MAX_INI_SIZE 1024L

char *INI_Find(FILE *fh, const char *section, const char *key)
{
	long fsize;
	char _ini_file[MAX_INI_SIZE];
	static char _ini_ret[MAX_INI_STR_LEN];
	char *line, *value;
	char cur_section[MAX_INI_STR_LEN];

	memset(&_ini_ret, 0, MAX_INI_STR_LEN);
	memset(&cur_section, 0, MAX_INI_STR_LEN);
	memset(&_ini_file, 0, MAX_INI_SIZE);

	if(!fh) {
		gi.bprintf(PRINT_HIGH, "INI_Find: file handle for INI_Find was NULL\n");
		return NULL;
	}

	fseek(fh, 0, SEEK_END);
	fsize = ftell(fh);
	rewind(fh);

	if(fsize > MAX_INI_SIZE) {
		gi.bprintf(PRINT_HIGH, "INI_Find: max file size is %ld, file to be read is %ld!\n", MAX_INI_SIZE, fsize);
		return NULL;
	}

	if(fread(&_ini_file, fsize, 1, fh) < 1) {
		gi.bprintf(PRINT_HIGH, "INI_Find: read failed: %d\n", ferror(fh));
		return NULL;
	}

	line = strtok(_ini_file, "\n");
	do {
		if(strlen(line) > 2 && line[0] != ';') {

			// remove DOS line endings
			if(line[strlen(line)-1] == 0x0D)
				line[strlen(line)-1] = 0x00;

			if(line[0] == '[' && line[strlen(line)-1] == ']') {
				memset(&cur_section, 0, MAX_INI_STR_LEN);
				strncpy(cur_section, line+1, strlen(line)-2);
			} else {
				value = strstr(line, "=");
				if(!value || value == line) {
					gi.bprintf(PRINT_HIGH, "INI_Find: invalid key/value pair: \"%s\"\n", line);
				} else {
					*value = 0; // null the delimeter, now line points to key and value+1 to value
					value++;

					/* this handles NULL (empty) section properly */
					if(section == NULL || section[0] == 0) {
					        if(cur_section[0] == 0 && strcmp(line, key) == 0) {
							strncpy(_ini_ret, value, MAX_INI_STR_LEN);
							return _ini_ret;
						}
					} else if(strcmp(section, cur_section) == 0 && strcmp(line, key) == 0) {
						strncpy(_ini_ret, value, MAX_INI_STR_LEN);
						return _ini_ret;
					}
				}
			}
		}
		line = strtok(NULL, "\n");
	} while(line != NULL);

	return NULL;
}
