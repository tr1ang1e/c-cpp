#include <unistd.h>			// getlogin(), chown()
#include <stdio.h>			// printf()
#include <stdlib.h>
#include <errno.h>			// perror()
#include <sys/types.h>		// getpwnam()
#include <pwd.h>			// getpwnam()

int main(int argc, char** argv)
{
	int result = EXIT_FAILURE;
	printf("%d\n", result);

	char* userName = getlogin();
	if (userName == NULL)
		goto exit;

	struct passwd* userData = getpwnam(userName);
	if (userData == NULL)
	{
		perror(userName);
		goto exit;
	}

	printf("uid: %d\n", userData->pw_uid);
	printf("gid: %d\n", userData->pw_gid);
	printf("nam: %s\n", userData->pw_name);

 	result = chown(argv[1], userData->pw_uid, userData->pw_gid);
	if (result == -1)
	{
		perror("chown");
		goto exit;
	}
	
	printf("Ownership was changed successfully\n");
	result = EXIT_SUCCESS;

exit:
	
	exit(result);
}
