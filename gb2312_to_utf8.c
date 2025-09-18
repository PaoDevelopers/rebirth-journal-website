#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>

#define BUF_SIZE 4096

int main(void) {
	char inbuf[BUF_SIZE];
	char outbuf[BUF_SIZE * 4];
	size_t inbytes;

	iconv_t cd = iconv_open("UTF-8", "GB2312");
	if (cd == (iconv_t)(-1)) {
		perror("iconv_open");
		return 1;
	};

	while (!feof(stdin)) {
		inbytes = fread(inbuf, 1, BUF_SIZE, stdin);
		if (inbytes == 0) break;

		char *inptr = inbuf;
		while (inbytes > 0) {
			char *outptr = outbuf;
			size_t outbytes = sizeof(outbuf);
			size_t res = iconv(cd, &inptr, &inbytes, &outptr, &outbytes);

			if (res == (size_t)(-1)) {
				if (errno == EILSEQ || errno == EINVAL) {
					const char replacement[] = "?";
					fwrite(replacement, 1, strlen(replacement), stdout);

					inptr++;
					inbytes--;
					continue;
				} else if (errno == E2BIG) {
					fwrite(outbuf, 1, sizeof(outbuf) - outbytes, stdout);
					continue;
				} else {
					perror("iconv");
					iconv_close(cd);
					return 1;
				};
			};

			fwrite(outbuf, 1, sizeof(outbuf) - outbytes, stdout);
		};
	};

	iconv_close(cd);
	return 0;
};
