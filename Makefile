all: Transcoded.mkv Transcoded2.mkv Copy.mkv Copy2.mkv Small.mkv Small2.mkv

Transcoded.mkv: Monody.mkv
	ffmpeg -ss 2:46 -i $< -t 57 $@

Transcoded2.mkv: Monody.mkv
	ffmpeg -i $< -ss 2:46 -t 57 $@

Copy.mkv: Monody.mkv
	ffmpeg -ss 2:46 -i $< -t 57 -c copy $@

Copy2.mkv: Monody.mkv
	ffmpeg -i $< -ss 2:46 -t 57 -c copy $@

Small.mkv: Monody.mkv
	ffmpeg -ss 2:46 -i $< -t 57 -s 640x360 $@

Small2.mkv: Monody.mkv
	ffmpeg -i $< -ss 2:46 -t 57 -s 640x360 $@
