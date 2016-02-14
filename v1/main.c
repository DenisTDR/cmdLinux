#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *getcwd(char *buf, size_t size);

void loop();
char *readLine();
void processCmd(char *line);
char **split(char *str, int *n);
int strStartsWithStr(char *big, char *small);

void nlCmd(int n, char **splitted);
void uniqCmd(int n, char **splitted);
void cdCmd(int n, char **splitted);
void strlwr(char *p);

int main() {
	
	loop();

	return 0;
}

char *crtDir;

void loop() {
	int read = 1;

	crtDir = (char *)malloc(256*sizeof(char));
	getcwd(crtDir, 256);
	printf("cd: %s\n", crtDir);

	while (read) {
		char *line = readLine();
		if (strcmp(line, "exit") == 0) {
			read = 0;
			break;
		}
		processCmd(line);
	}
	free(crtDir);
}

void processCmd(char *line) {
	int n;
	char **splitted = split(line, &n);

	if (n == 0)
		return;

	if (strcmp(splitted[0], "nl") == 0)
		nlCmd(n, splitted);
	else if (strcmp(splitted[0], "uniq") == 0)
		uniqCmd(n, splitted);
	else if (strcmp(splitted[0], "cd") == 0)
		cdCmd(n, splitted);
	else
		printf("Invalid command!\n");


	free(splitted);
	free(line);
	//printf("comanda are %d chestii\n", n);
}

char *readLine() {	
	static char *bfr = NULL;
	if (bfr == NULL) {
		bfr = (char *)malloc(256);
	}
	printf(">");
	fgets(bfr, 256, stdin);
	char *resized = strdup(bfr);
	resized[strlen(resized) - 1] = '\0';
	return resized;
}

char **split(char *str, int *n) {
	char *tmp = strdup(str);
	int maxN = 2;
	char **ret = (char **)malloc(maxN * sizeof(char *));
	(*n) = 0;
	char *pch = strtok(tmp, " ");
	while (pch != NULL) {
		if (*n == maxN) 
			ret = (char **)realloc(ret, (maxN += 2) * sizeof(char *));
		ret[*n] = pch;
		pch = strtok(NULL, " ");
		(*n)++;
	}
	return ret;
}

int strStartsWithStr(char *big, char *small) {
	return strstr(big, small) == big;
}

void cdCmd(int n, char **splitted){
	if(n != 2){
		printf("invalid parameters\n");
		return;
	}
	if(strcmp(splitted[1], ".") == 0)
		return;
	else if(strcmp(splitted[1], "/") == 0){
		crtDir[1] = '\0';
		return;
	}
	else if(strcmp(splitted[1], "..") == 0){
		if(strcmp(crtDir, "/") == 0)
			return;
		int len = strlen(crtDir) - 1;
		while(crtDir[len] != '/'){
			len --;
		}
		crtDir[len] = '\0';
	}
	else{
		char *tmp = (char*)malloc(strlen(crtDir) + strlen(splitted[1]) + 2);
		sprintf(tmp, "%s/%s", crtDir, splitted[1]);
		int len = strlen(tmp) - 1;
		while(tmp[len] == '/'){
			tmp[len] = '\0';
			len --;
		}
		free(crtDir);
		crtDir = tmp;
	}
}

void uniqCmd(int n, char **splitted) {
	int i, d, u, du;
	i = d = u = du = 0;
	FILE *in = stdin, *out = stdout;
	int j;
	for (j = 1; j < n; j++) {
		if (strcmp(splitted[j], "-i") == 0) {
			i = 1;
		}
		else if (strcmp(splitted[j], "-d") == 0) {
			d = 1;
			du = d&&u;
		}
		else if (strcmp(splitted[j], "-u") == 0) {
			u = 1;
			du = d&&u;
		}
		else if(strcmp(splitted[j], ">") == 0){
			j++;
			if(splitted[j] == NULL){
				printf("invalid redirection file!\n");
				return;
			}
			if(!openFile(&out, splitted[j], "w"))
				return;
		}
		else{
			if (in == stdin) {
				if(!openFile(&in, splitted[j], "r"))
					return;
			}
			else
				if(!openFile(&out, splitted[j], "w"))
					return;			
		}
	}

	char *crtLine = (char *)malloc(256);
	char *lastLine = (char *)malloc(256);
	char *lastLineLwr = (char *)malloc(256);
	char *tmp;
	int firstLine = 1;
	int flag = 0;
	do {
		fgets(crtLine, 256, in);

		int len = strlen(crtLine);
		if (crtLine[len - 1] == '\n')
			crtLine[len - 1] = '\0';

		if (i) {
			tmp = strdup(crtLine);
			strlwr(tmp);
		}
		else 
			tmp = crtLine;
		
		int neq = firstLine || strcmp(i?lastLineLwr:lastLine, tmp) != 0;

		if (neq) {
			if(u)
			{
				if(!flag && !du && !firstLine)
					fprintf(out, "%s\n", lastLine);
				flag = 0;
			}
			else if (d) 
				flag = 0;
			else if(!du)
					fprintf(out, "%s\n", crtLine);			
		}
		else {
			if(u)
				flag = 1;
			else if (d) 
				if (!flag) {
					flag = 1;
					if(!du)	
						fprintf(out, "%s\n", lastLine);
				}
		}

		if (firstLine) firstLine = 0;
		strcpy(lastLine, crtLine);
		if(i){
			strcpy(lastLineLwr, tmp);
			free(tmp);
		}
	} while (!feof(in));

	if (in != stdin)
		fclose(in);
	if (out != stdout)
		fclose(out);
	free(crtLine);
	free(lastLine);
	free(lastLineLwr);
}

void nlCmd(int n, char **splitted){
	int s, d;
	s = d = 0;
	char *sParam = "  ", *dParam;
	FILE *f = stdin;
	FILE *out = stdout;
	int i;
	for (i = 1; i < n; i++) {
		if (strcmp(splitted[i], "-s") == 0) {
			s = 1;
			sParam = splitted[i + 1];
			i++;
		}
		else if (strcmp(splitted[i], "-d") == 0) {
			d = 1;
			dParam = splitted[i + 1];
			i++;
		}
		else if(strcmp(splitted[i], ">") == 0){
			i++;
			if(splitted[i] == NULL){
				printf("invalid redirection file!\n");
				return;
			}
			if(!openFile(&out, splitted[i], "w"))
				return;
		}
		else {
			if(!openFile(&f, splitted[i], "r"))
				return;
		}
	}

	int lineIndex = 0;
	char *crtLine = (char *)malloc(256);

	do {
		fgets(crtLine, 256, f);
		int len = strlen(crtLine);
		if (crtLine[len - 1] == '\n')
			crtLine[len - 1] = '\0';
		printf("w: \t%d%s%s\n", lineIndex, sParam, crtLine);
		fprintf(out, "\t%d%s%s\n", lineIndex, sParam, crtLine);
		fflush(out);
		lineIndex++;
	} while (!feof(f));

	free(crtLine);
	if(out != stdout)
		fclose(out);
}


void strlwr(char *p){
	while(*p){
		*p = tolower(*p);
		p++;
	}
}

int openFile(FILE **f, char *fileName, char *mode){
	char *tmp = fileName;
	if(fileName[0] != '/'){
		tmp = (char *)malloc(strlen(fileName) + strlen(crtDir) + 3);
		sprintf(tmp, "%s/%s", crtDir, fileName);
	}
	printf("openning file '%s' with mode '%s'\n", tmp, mode);
	*f = fopen(tmp, mode);
	if(!(*f)){
		printf("Can't open file '%s' !\n", tmp);
	}
	if(tmp!=fileName)
		free(tmp);
	return *f != NULL;
}