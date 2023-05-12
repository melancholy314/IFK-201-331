#include<stdio.h>  
#include<time.h>
#include<string.h>

int main() {
	FILE* file;
	if (fopen_s(&file, "C:/Users/kku/Desktop/test.kku", "r+")) {
		printf_s("Error on open file!\n");
		return 0;
	}
	//if (fopen_s(&file, "D:/1.txt", "r+")) {
	//	printf_s("Error on open file!\n");
	//	return 0;
	//}
	char buffer[1024] = { 0 };
	fread(buffer, sizeof(char), 1024, file);
	printf_s("% s", buffer);
	printf_s("\n");

	time_t rawtime;
	struct tm info;
	time(&rawtime);
	localtime_s(&info, &rawtime);

	memset(buffer, '\0', 1024);
	strcat_s(buffer, "Korotaev_Kirill_Korotaev_Kirill_Korotaev_Kirill_Korotaev_201-331");
	size_t len = strlen(buffer);

	fseek(file, 0, 0);
	fwrite(buffer, sizeof(char), len, file);

	fclose(file);

	getchar();

	return 0;


}
