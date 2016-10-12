
void error(char *msg){
	if(msg==NULL) printf("Erro.\n");
	else printf("%s\n", msg);
	exit(EXIT_FAILURE);
}