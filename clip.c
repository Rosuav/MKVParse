#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

int main() {
	printf("Hello, world\n");
	printf("Version: %X\n", avformat_version());
	AVFormatContext *pFormatContext = avformat_alloc_context();
}
