#include <iostream>
#include <fstream>
#include <cmath>
#include "cxxopts.hpp"
#include "PitchDetector.hpp"
#include "HistogramStepDetector.hpp"
#include "NoteDetector.hpp"
#include "AudioReader.hpp"
#include "FFmpegAudioReader.hpp"
#include "McLeodPitchExtractorMethod.hpp"
#include "json.hpp"

using json = nlohmann::json;

cxxopts::ParseResult parse(int argc, char* argv[]) {
  try
  {
    cxxopts::Options options(argv[0], " - A program to extract sheet music from single instrument recording -");
    options.positional_help("[-i, --input] /path/to/input/file");
    options.show_positional_help();

    options.add_options()
        ("help", "Print help")
        ("i, input",    "Input audio/video file [.mp3, .wav, .ogg, etc...] or json file for remaining saved conversion", cxxopts::value<std::string>())
        ("o, output",   "Output file, .xml for MusicXML format, .mid for midi format, .json to save temporary conversion"
                        "(if you set '--complete' option, the output must be a folder)", cxxopts::value<std::string>())
        ("positional",  "input,output: these are the arguments that can be entered without an option", cxxopts::value<std::vector<std::string>>())
        ("pitch-conversion",    "Perform only the pitch conversion, the input file must be a media file [audio/video], "
                                "the output file will be a json file containing the pitch along track.")
        ("step-conversion", "Perform only until the step conversion, the input file must be a media file [audio/video] "
                            "OR a json file containing a result of pitch conversion, the output file will be either a json "
                            "file [default] containing the detected step OR a midi file without rhythm interpolation.")
        ("complete",    "Perform all the steps and save all temporary results and scores in the specified folder set by '--output'")
        ;

    options.parse_positional({"input", "output", "positional"});

    auto result = options.parse(argc, argv);
    // Help function
    if (result.count("help"))
    {
      std::cout << options.help({"", "Group"}) << std::endl;
      exit(0);
    }
    // Input file
    if (result.count("input") == 0) {
        throw cxxopts::OptionException("No input file specified");
        exit(1);
    } else if (result.count("input") > 1) {
        throw cxxopts::OptionException("Input option specified multiple times.");
        exit(1);
    }
    // Output file
    if (result.count("output") > 1) {
        throw cxxopts::OptionException("Ouput option specified multiple times.");
        exit(1);
    }


    return result;
  } catch (const cxxopts::OptionException& e)
  {
    std::cout << "error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}


// Pitch Detector
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void writePitchResult(const PitchResult & pitch_result, std::string filepath) {
    json j = {
        {"period_s", pitch_result.period_s},
        {"f0_hz", pitch_result.f0_hz},
        {"pitch_st", pitch_result.pitch_st},
        {"energy", pitch_result.energy}
    };
    std::ofstream outfile;
    outfile.open(filepath);
    outfile << j.dump(4) << std::endl;
    outfile.close();
}
void loadPitchResult(PitchResult & pitch_result, const std::string & filepath) {
    if(!exists(filepath)) {
        throw std::runtime_error("File does not exists");
    }
    std::ifstream infile;
    infile.open(filepath);
    std::string pitch_result_str;
    std::string line_str;
    while(std::getline(infile, line_str)) {
        pitch_result_str += line_str;
    }
    infile.close();
    auto j = json::parse(pitch_result_str);
    pitch_result.period_s = j["period_s"].get<double>();
    pitch_result.f0_hz = j["f0_hz"].get<double>();
    for(json::iterator it = j["pitch_st"].begin(); it != j["pitch_st"].end(); ++it) {
        if((*it).is_null()) {
            pitch_result.pitch_st.push_back(std::numeric_limits<double>::quiet_NaN());
        } else {
            pitch_result.pitch_st.push_back(*it);
        }
    }
    for(json::iterator it = j["energy"].begin(); it != j["energy"].end(); ++it) {
        if((*it).is_null()) {
            pitch_result.energy.push_back(std::numeric_limits<double>::quiet_NaN());
        } else {
            pitch_result.energy.push_back(*it);
        }
    }
}
PitchResult performPitchDetection(const std::string & filepath) {
    if(!exists(filepath)) {
        throw std::runtime_error("File does not exists");
    }
    // Parameters 
    AudioReaderParameters reader_parameters;
    reader_parameters.windowstimesize_s = 20e-3;
    reader_parameters.period_s = 1e-3;
    reader_parameters.resample_rate_hz = 44100;

    McLeodParameters mcleod_parameters;
    mcleod_parameters.cutoff = 0.97;
    mcleod_parameters.small_cutoff = 0.5;
    mcleod_parameters.lower_pitch_cutoff = 50.0;

    // Audio reader
    FFmpegAudioReader audio_reader(filepath);

    // Pitch extractor method
    McLeodPitchExtractorMethod mcleod_method(mcleod_parameters);

    // Pitch detector
    PitchDetector pitch_detector(&audio_reader, &mcleod_method);
    PitchResult result = pitch_detector.perform(nullptr, reader_parameters);

    return result;
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



// Step Detector
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void writeStepResult(const StepResult & step_result, std::string filepath) {
    json j;
    j["offset_s"] = step_result.offset_s;
    j["notes"] = json::array();
    for(const auto & note: step_result.notes) {
        j["notes"].push_back(
            {
                {"is_a_note", note.is_a_note},
                {"length_s", note.length_s},
                {"pitch_st", note.pitch_st},
                {"energy", note.energy},
                {"linked", note.linked}
            }
        );
    }
    std::ofstream outfile;
    outfile.open(filepath);
    outfile << j.dump(4) << std::endl;
    outfile.close();
}
void loadStepResult(StepResult & step_result, std::string filepath) {
    if(!exists(filepath)) {
        throw std::runtime_error("File does not exists");
    }
    std::ifstream infile;
    infile.open(filepath);
    std::string step_result_str;
    std::string line_str;
    while(std::getline(infile, line_str)) {
        step_result_str += line_str;
    }
    infile.close();
    auto j = json::parse(step_result_str);
    step_result.offset_s = j["offset_s"].get<double>();
    for(json::iterator it = j["notes"].begin(); it != j["notes"].end(); ++it) {
        AnalogNote note;
        note.is_a_note = (*it)["is_a_note"].get<bool>();
        note.length_s = (*it)["length_s"].get<double>();
        if((*it)["pitch_st"].is_null()) {
            note.pitch_st = std::numeric_limits<double>::quiet_NaN();
        } else {
            note.pitch_st = (*it)["pitch_st"].get<double>();
        }
        if((*it)["energy"].is_null()) {
            note.energy = std::numeric_limits<double>::quiet_NaN();
        } else {
            note.energy = (*it)["energy"].get<double>();
        }
        note.linked = (*it)["linked"].get<bool>();
        step_result.notes.push_back(note);
    }
}
StepResult performStepDetection(const PitchResult & pitch_result) {
    // Parameters
    StepParameters parameters;
    parameters.min_note_length_s = 100e-3;
    parameters.min_note_gap_st = 2/3.0;
    parameters.median_filter_width_s = 20e-3;
    parameters.min_pitch_st = 0.0;
    parameters.max_pitch_st = 200.0;
    // Step detector
    HistogramStepDetector step_detector(pitch_result);
    StepResult result = step_detector.perform(nullptr, parameters);
    return result;
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



// Note Detector
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
void writeScore(const MusicXmlScore & score, std::string filepath) {
    std::string score_str = score.toString();
    std::ofstream outfile;
    outfile.open(filepath);
    outfile << score_str;
    outfile.close();
}
MusicXmlScore performNoteDetection(const StepResult & step_result) {
    // Parameters
    RhythmParameters parameters;
    parameters.delay_min_s = 0.3;
    parameters.delay_max_s = 1.5;
    parameters.error_max = 10.0;
    parameters.max_delay_var = 0.5;
    parameters.combinations_masked = {};
    // Rhythm detector
    NoteDetector note_detector(step_result);
    MusicXmlScore score = note_detector.perform(nullptr, parameters);
    return score;
}
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////



int main(int argc, char* argv[])
{
    auto options = parse(argc, argv);
    // Path Input file
    std::string infilepath = options["input"].as<std::string>();
    // Path Output file
    std::string outfilepath;
    if(options.count("output")) {
        outfilepath = options["output"].as<std::string>();
    } else {
        outfilepath = infilepath + ".json";
    }

    // PitchResult pitch_result;
    // loadPitchResult(pitch_result, "pitch_result.json");

    // StepResult step_result;
    // loadStepResult(step_result, "step_result.json");

    // Pitch extraction
    PitchResult pitch_result = performPitchDetection(infilepath);
    writePitchResult(pitch_result, "pitch_result.json");

    // Step detection
    StepResult step_result = performStepDetection(pitch_result);
    writeStepResult(step_result, "step_result.json");

    // Note detection
    MusicXmlScore score = performNoteDetection(step_result);
    writeScore(score, "score.xml");

    return 0;
}