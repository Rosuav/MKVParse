#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

int main() {
	AVFormatContext *s = NULL;
	int ret = avformat_open_input(&s, "file:Monody.mkv", NULL, NULL);
	if (ret < 0) abort();
	int videoidx = -1;
	for (int i = 0; i < s->nb_streams; ++i) {
		AVStream *strm = s->streams[i];
		printf("Stream %d: time base %d/%d, frames %ld\n",
			i, strm->time_base.num, strm->time_base.den, strm->nb_frames);
		if (strm->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) videoidx = i;
	}
	if (videoidx == -1) {printf("No video stream\n"); exit(0);}
	while (1) {
		AVPacket pkt;
		if (av_read_frame(s, &pkt) < 0) break;
		if (pkt.stream_index == videoidx) {
			if (pkt.flags && 1) printf("[%d] Frame: %ld->%ld, flg %x\n", pkt.stream_index, pkt.pts, pkt.duration, pkt.flags);
		}
		av_packet_unref(&pkt);
	}
	avformat_close_input(&s);
}
