# Specifying the sources files
set(LIB_SOURCES
	common_tools.cpp
	MidiScore.cpp
	1_PitchDetector/PitchDetector.cpp
	1_PitchDetector/AudioReader.cpp
	1_PitchDetector/FFmpegAudioReader.cpp
	1_PitchDetector/PitchExtractorMethod.cpp
	1_PitchDetector/McLeodPitchExtractorMethod.cpp
	2_StepDetector/StepDetector.cpp
	2_StepDetector/HistogramStepDetector.cpp
	2_StepDetector/HysteresisThreshold.cpp
	2_StepDetector/CumulativeSumThreshold.cpp
	3_NoteDetector/RhythmDetector/Dijkstra.cpp
	3_NoteDetector/RhythmDetector/RhythmDetector.cpp
	3_NoteDetector/HeightDetector/HeightDetector.cpp
	3_NoteDetector/MusicXmlScore.cpp
	3_NoteDetector/NoteDetector.cpp
)

# Generate the shared library from the sources
add_library(scorelisto STATIC ${LIB_SOURCES})