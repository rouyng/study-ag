#include <stdio.h> // fopen()
#include <stdlib.h> // malloc()
#include <string.h> // strncpy()
#include "libQRNG.h"

#ifdef _WIN32
#include <time.h>
#define ATOLL _atoi64
#else
#include <sys/time.h>
#define ATOLL atoll
#endif

#define MAX_WRITE_BUFFER 50*16*65536
#define FILENAME_LENGTH 255

#define QRNG_USERNAME_LENGTH 64+1
#define QRNG_PASSWORD_LENGTH 64+1

typedef enum _qrng_download_rc {
	QRNG_DOWNLOAD_SUCCESS = 0,
	QRNG_DOWNLOAD_INVALID_SIZE,
	QRNG_DOWNLOAD_MALLOC_FAILED,
	QRNG_DOWNLOAD_FOPEN_FAILED,
	QRNG_DOWNLOAD_CONNECT_SSL_FAILED,
	QRNG_DOWNLOAD_CONNECT_FAILED,
	QRNG_DOWNLOAD_FWRITE_FAILED
} qrng_download_rc;

void quit(int returncode) {
	printf("Please press return to exit...");
	/* program blocked (waiting for user input) */
	getchar();

	exit(returncode);
}

unsigned int ms_clock() /* portable ms-timer */
{
#ifdef _WIN32
	return (clock()*1000) / CLOCKS_PER_SEC ;
#else
	struct timeval currenttime;
	unsigned int mtime, seconds, useconds;    
	gettimeofday(&currenttime, NULL);
	seconds  = currenttime.tv_sec;
	useconds = currenttime.tv_usec;
	mtime = seconds*1000 + useconds/1000;
	return mtime;
#endif
}

void show_progress(long long bytes_downloaded, long long bytes_count_download, unsigned int duration_lastbuffer, int read_bytes, unsigned int duration) {
	static int progressbar_length = 24;
	int i;

	printf("\r%3lld%% [", bytes_downloaded * 100 / bytes_count_download);
	for (i = 1; i <= progressbar_length; i++) {
		if ( (bytes_downloaded * progressbar_length / bytes_count_download) > i) {
			printf("=");
		}
		if ( (bytes_downloaded * progressbar_length / bytes_count_download) == i) {
			printf(">");
		}
		if ( (bytes_downloaded * progressbar_length / bytes_count_download) < i) {
			printf(" ");
		}
	}
	printf("]");
	printf(" %.1lf/%.0lf MB", (double)bytes_downloaded / 1024 / 1024, (double)bytes_count_download / 1024 / 1024);
	printf(", %.2lfK/s", (double)read_bytes / duration_lastbuffer);
	printf(", eta %.0lfs", (double)duration / 1000 / bytes_downloaded * bytes_count_download - duration/1000);
	printf("     "); /* blank out previous chars */
	fflush(stdout);
}

int main(int argc, char* argv[])
{
	char qrng_username[QRNG_USERNAME_LENGTH];
	char qrng_password[QRNG_PASSWORD_LENGTH];
	char filename[FILENAME_LENGTH];
	char bytes_to_be_read_buffer[11];
	int bytes_to_be_read, max_write_buffer_user;
	long long bytes_downloaded, bytes_count_download = -1;
	int use_ssl = 0;

	int retcode, retcode_file, actual_bytes_read;
	FILE *fp = NULL;
	char *buffer = NULL;
	unsigned int ms_starttime, ms_elapsed, ms_starttime_buffer;

	printf("This demo program is intended to download and save random data\n");
	printf("from the QRNG service at http://qrng.physik.hu-berlin.de.\n");
	printf("It uses the library libQRNG which is also available at the QRNG website.\n");
	printf("\n");
	printf("using QRNG library: %s\n", qrng_libQRNG_version);
	printf("\n");

	if (argc > 4) {
		strncpy(filename, argv[1], FILENAME_LENGTH);
		bytes_count_download = ATOLL(argv[2])*1024*1024;
		strncpy(qrng_username, argv[3], QRNG_USERNAME_LENGTH);
		strncpy(qrng_password, argv[4], QRNG_PASSWORD_LENGTH);

		if (bytes_count_download < 1) {
			printf("Please specify how much MB shall be downloaded. (invalid input: %lld MB)\n", bytes_count_download);
			quit(QRNG_DOWNLOAD_INVALID_SIZE);
		}
		if (argc > 5) {
			use_ssl = 1;
		}
	} else {
		printf("Switching to interactive-mode. Alternatively you can also use the following syntax:\n");
		printf("\n");
		printf("qrngdownload [filename] [number of MB to download] [username] [password] [write here anything to enable SSL, e.g. SSL]\n\n");

		printf("Please enter the filename of the destination file: ");
		fgets(filename, FILENAME_LENGTH, stdin);
		/* remove \n */
		filename[strlen(filename)-1] = '\0';
		while (bytes_count_download < 1) {
			printf("Please specify how much MB shall be downloaded: ");
			fgets(bytes_to_be_read_buffer, 11, stdin);
			bytes_count_download = ATOLL(bytes_to_be_read_buffer) * 1024 * 1024;
		}
		printf("Please enter your username and password now.\nIf you do not have any account for the QRNG service yet,\nplease register first at http://qrng.physik.hu-berlin.de/register/\n");
		printf("\n");
		printf("Username: ");
		fgets(qrng_username, QRNG_USERNAME_LENGTH, stdin); qrng_username[strlen(qrng_username)-1] = '\0';
		printf("Password: ");
		fgets(qrng_password, QRNG_PASSWORD_LENGTH, stdin); qrng_password[strlen(qrng_password)-1] = '\0';
		printf("\n");
		printf("No SSL will be used. If you want to use SSL, give all details\n");
		printf("as command line parameters and write anything as fifth parameter\n");
		printf("\n");
	}

	/* adjust buffer */
	if (bytes_count_download < MAX_WRITE_BUFFER) {
		max_write_buffer_user = bytes_count_download;
	} else {
		max_write_buffer_user = MAX_WRITE_BUFFER;
	}

	/* malloc */
	if ((buffer = (char*)malloc(max_write_buffer_user)) == NULL) {
		fprintf(stderr, "Failed to malloc %d MB memory\n", max_write_buffer_user / 1024 / 1024);
		quit(QRNG_DOWNLOAD_MALLOC_FAILED);
	}

	/* open file */
	fp = fopen(filename, "wb");
	if(!fp)
	{
		fprintf(stderr, "ERROR: Cannot open output file \"%s\"\n", filename);
		quit(QRNG_DOWNLOAD_FOPEN_FAILED);
	}

	/* connect */
	if (use_ssl) {
		retcode = qrng_connect_SSL(qrng_username, qrng_password);
		if (retcode != 0) {
			fprintf(stderr, "ERROR: Failed to connect to QRNG service using SSL: %s\n", qrng_error_strings[retcode]);
			quit(QRNG_DOWNLOAD_CONNECT_SSL_FAILED);
		}
		printf("Successfully connected to QRNG service using SSL\n");
	} else {
		retcode = qrng_connect(qrng_username, qrng_password);
		if (retcode != 0) {
			fprintf(stderr, "ERROR: Failed to connect QRNG service: %s\n", qrng_error_strings[retcode]);
			quit(QRNG_DOWNLOAD_CONNECT_FAILED);
		}
		printf("Successfully connected to QRNG service\n\n");
	}

	/* download */
	bytes_downloaded = 0;
	ms_starttime = ms_clock();
	while (bytes_downloaded <  bytes_count_download) {
		if ((bytes_count_download - bytes_downloaded) < max_write_buffer_user) {
			bytes_to_be_read = (bytes_count_download - bytes_downloaded);
		} else {
			bytes_to_be_read = max_write_buffer_user;
		}
		ms_starttime_buffer = ms_clock();
		retcode = qrng_get_byte_array(buffer, bytes_to_be_read, & actual_bytes_read);
		bytes_downloaded += actual_bytes_read;
		if (retcode != 0) {
			fprintf(stderr, "ERROR: Failed to retrieve data from QRNG service: %s\n", qrng_error_strings[retcode]);
			break;
		}
		retcode_file = fwrite(buffer, 1, actual_bytes_read, fp);
		if(retcode_file != actual_bytes_read)
		{
			fprintf(stderr, "ERROR: Failed to write data to file.\n");
			quit(QRNG_DOWNLOAD_FWRITE_FAILED);
		}
		show_progress(bytes_downloaded, bytes_count_download, ms_clock()-ms_starttime_buffer, actual_bytes_read, ms_clock()-ms_starttime);
	}
	ms_elapsed = ms_clock()-ms_starttime;

	printf("\n");
	printf("received data was written to file: %s\n", filename);
	fclose(fp);
	free(buffer);
	qrng_disconnect();

	if (bytes_downloaded != bytes_count_download) {
		fprintf(stderr, "ERROR: Could not download all data. Requested: %lld, got: %lld\n", bytes_count_download, bytes_downloaded);
	}
	printf("Downloaded: %lld MB, average Speed: %3.2lf MB/s, duration: %us\n", bytes_downloaded/1024/1024, (double)bytes_downloaded/1024/1024/ms_elapsed*1000, ms_elapsed / 1000);
	printf("\n");

	quit(QRNG_DOWNLOAD_SUCCESS);
}
