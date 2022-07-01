#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavformat/avformat.h>

void bomb(int ret, const char *fn, int line) {
	char err[64]; av_strerror(ret, err, sizeof err);
	printf("%s:%d: Unexpected error %d: %s\n", fn, line, ret, err);
	exit(1);
}
#define DIE() bomb(ret, __FILE__, __LINE__)

int main() {
	AVFormatContext *s = NULL;
	int ret = avformat_open_input(&s, "file:Monody.mkv", NULL, NULL);
	if (ret < 0) DIE();

	const char *outfile = "Clip.mkv";
	AVFormatContext *output;
	avformat_alloc_output_context2(&output, NULL, NULL, outfile);
	if (!output) {
		printf("Unable to calculate output format for %s\n", outfile);
		exit(1);
	}
	AVStream *streams[s->nb_streams];
	AVCodecContext *incodecs[s->nb_streams];
	AVCodecContext *outcodecs[s->nb_streams];

	int videoidx = -1;
	for (int i = 0; i < s->nb_streams; ++i) {
		AVStream *strm = s->streams[i];
		printf("Stream %d: time base %d/%d, frames %ld\n",
			i, strm->time_base.num, strm->time_base.den, strm->nb_frames);
		if (strm->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) videoidx = i;
		streams[i] = avformat_new_stream(output, NULL);
		streams[i]->id = i;
		const AVCodec *codec = avcodec_find_decoder(strm->codecpar->codec_id);
		incodecs[i] = avcodec_alloc_context3(codec);
		outcodecs[i] = avcodec_alloc_context3(codec);
		avcodec_open2(incodecs[i], codec, NULL);
		avcodec_open2(outcodecs[i], codec, NULL);
	}
	if (videoidx == -1) {printf("No video stream\n"); exit(0);}

	ret = avio_open(&output->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0) DIE();
	AVDictionary *opt = NULL;
	ret = avformat_write_header(output, &opt);
	if (ret < 0) DIE();

	AVStream *strm = s->streams[videoidx];
	//Our "goal" PTS is defined by a timestamp, here given in seconds, divided by the time_base.
	int start = 166, length = 57; //2:46
	int64_t goal = start * strm->time_base.den / strm->time_base.num;
	int64_t end = goal + length * strm->time_base.den / strm->time_base.num;
	printf("Goal: %ld End: %ld\n", goal, end);
	enum {SEEKING, TRANSCODING, COPYING, FINISHING} mode = SEEKING;
	while (mode != FINISHING) {
		AVPacket pkt;
		if (av_read_frame(s, &pkt) < 0) break;
		if (pkt.stream_index == videoidx) {
			if (mode == SEEKING && pkt.pts >= goal) {
				mode = TRANSCODING;
				printf("Start frame: %ld\n", pkt.pts);
			} else if (mode == TRANSCODING && (pkt.flags & 1)) {
				printf("Got a key frame: %ld\n", pkt.pts);
				mode = COPYING;
			} else if (mode == COPYING && pkt.pts >= end) {
				printf("Finishing! %ld\n", pkt.pts);
				mode = FINISHING;
			}
		}
		if (mode == TRANSCODING || mode == COPYING) {
			//For now, transcode the whole way.
			avcodec_send_packet(incodecs[pkt.stream_index], &pkt);
			while (1) {
				AVFrame frame;
				int ret = avcodec_receive_frame(incodecs[pkt.stream_index], &frame);
				if (ret < 0) break;
				ret = avcodec_send_frame(outcodecs[pkt.stream_index], &frame);
				while (1) {
					AVPacket outpkt;
					ret = avcodec_receive_packet(outcodecs[pkt.stream_index], &outpkt);
					if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;
					else if (ret < 0) DIE();
					outpkt.stream_index = pkt.stream_index;
					av_interleaved_write_frame(output, &outpkt);
				}
			}
		}
		av_packet_unref(&pkt);
	}
	avformat_close_input(&s);
}
