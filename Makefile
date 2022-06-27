all: Transcoded.mkv Transcoded2.mkv Copy.mkv Copy2.mkv Copy3.mkv Small.mkv Small2.mkv ebml_matroska.xml

Transcoded.mkv: Monody.mkv Makefile
	ffmpeg -y -ss 2:46 -i $< -t 57 $@

Transcoded2.mkv: Monody.mkv Makefile
	ffmpeg -y -i $< -ss 2:46 -t 57 $@

Copy.mkv: Monody.mkv Makefile
	ffmpeg -y -ss 2:46 -i $< -t 57 -c copy $@

Copy2.mkv: Monody.mkv Makefile
	ffmpeg -y -i $< -ss 2:46 -t 57 -c copy $@

Copy3.mkv: Monody.mkv Makefile
	ffmpeg -y -ss 2:30 -i $< -ss 16 -t 57 -c copy $@

Small.mkv: Monody.mkv Makefile
	ffmpeg -y -ss 2:46 -i $< -t 57 -s 640x360 $@

Small2.mkv: Monody.mkv Makefile
	ffmpeg -y -i $< -ss 2:46 -t 57 -s 640x360 $@

ebml_matroska.xml:
	wget https://raw.githubusercontent.com/ietf-wg-cellar/matroska-specification/master/ebml_matroska.xml