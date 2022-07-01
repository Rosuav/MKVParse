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
	const char *outfile = "Clip.mkv";

	AVFormatContext *output;
	avformat_alloc_output_context2(&output, NULL, NULL, outfile);
	if (!output) {
		printf("Unable to calculate output format for %s\n", outfile);
		exit(1);
	}
	const AVOutputFormat *fmt = output->oformat;
	if (fmt->video_codec == AV_CODEC_ID_NONE || fmt->audio_codec == AV_CODEC_ID_NONE) {
		//Clipping to just audio can be done without issues in vanilla ffmpeg.
		//Clipping to just video is probably best done with vanilla ffmpeg too.
		printf("Output format lacks audio/video\n");
		exit(1);
	}
	const AVCodec *audio_codec = avcodec_find_encoder(fmt->audio_codec);
	const AVCodec *video_codec = avcodec_find_encoder(fmt->video_codec);
	ret = avio_open(&output->pb, outfile, AVIO_FLAG_WRITE);
	if (ret < 0) abort();

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
		av_packet_unref(&pkt);
	}
	avformat_close_input(&s);
}
