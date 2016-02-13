#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void loop();
char *readLine();
void processCmd(char *line);
char **split(char *str, int *n);
int strStartsWithStr(char *big, char *small);

void nlCmd(int n, char **splitted);
void uniqCmd(int n, char **splitted);

int main() {
	
	loop();

	return 0;
}



void loop() {
	int read = 1;
	while (read) {
		char *str = strdup("uniq exemplu.out");
		printf(">%s\n", str);
		processCmd(str);
		str = strdup("uniq exemplu.out -d");
		printf(">%s\n", str);
		processCmd(str);
		str = strdup("uniq exemplu.out -i");
		printf(">%s\n", str);
		processCmd(str);
		str = strdup("uniq exemplu.out -d -i");
		printf(">%s\n", str);
		processCmd(str);

		char *line = readLine();
		if (strcmp(line, "exit") == 0) {
			read = 0;
			break;
		}
		processCmd(line);
	}
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

void uniqCmd(int n, char **splitted) {
	int i, d, u;
	i = d = u = 0;
	FILE *in = stdin, *out = stdout;
	char *fin, *fout;

	for (int j = 1; j < n; j++) {
		if (strcmp(splitted[j], "-i") == 0) {
			i = 1;
		}
		else if (strcmp(splitted[j], "-d") == 0) {
			d = 1;
		}
		else if (strcmp(splitted[j], "-u") == 0) {
			u = 1;
		}
		else {
			if (in == stdin) {
				fin = splitted[j];
				in = fopen(fin, "r");
			}
			else {
				fout = splitted[j];
				out = fopen(fout, "w");
			}
		}
	}
	if (!in) {
		printf("Fisierul '%s' nu a fost gasit!\n", fin);
		return;
	}
	if (!out) {
		printf("Fisierul '%s' nu a fost gasit!\n", fout);
		return;
	}

	char *crtLine = (char *)malloc(256);
	char *lastLine = (char *)malloc(256);
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
		else {
			tmp = crtLine;
		}
		
		int neq = firstLine || strcmp(lastLine, tmp) != 0;
		if (firstLine) firstLine = 0;

		if (neq) {
			if (d) {
				flag = 0;
			}
			else {
				fprintf(out, "%s\n", crtLine);
			}
		}
		else {
			if (d) {
				if (!flag) {
					flag = 1;
					fprintf(out, "%s\n", lastLine);
				}
			}
		}

		strcpy(lastLine, tmp);
		if (i)
			free(tmp);
		//printf("->%s\n", lastLine);

	} while (!feof(in));
	fprintf(out, "\n");
	if (in != stdin)
		fclose(in);
	if (out != stdout)
		fclose(out);
}

void nlCmd(int n, char **splitted){
	int s, d;
	s = d = 0;
	char *sParam = "  ", *dParam, *fn;
	FILE *f = stdin;

	for (int i = 1; i < n; i++) {
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
		else {
			fn = splitted[i];
			f = fopen(fn, "r");
		}
	}

	if (!f) {
		printf("Fisierul '%s' nu a fost gasit!\n", fn);
		return;
	}
	int lineIndex = 0;
	char *crtLine = (char *)malloc(256);

	do {
		fgets(crtLine, 256, f);
		int len = strlen(crtLine);
		if (crtLine[len - 1] == '\n')
			crtLine[len - 1] = '\0';
		printf("\t%d%s%s\n", lineIndex, sParam, crtLine);
		lineIndex++;
	} while (!feof(f));
	
}
