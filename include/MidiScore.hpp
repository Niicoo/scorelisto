#ifndef MIDI_SCORE
#define MIDI_SCORE

#include <vector>
#include <string>

std::vector<uint8_t> int16ToBytes(uint16_t value);
std::vector<uint8_t> int32ToBytes(uint32_t value);
std::vector<uint8_t> int64ToVLQBytes(uint64_t value);



class MidiTrack {
public:
    // Constructor
    MidiTrack();
    // Destructor
    virtual ~MidiTrack();
    // Functions
    uint32_t getBufferSize();
    template<typename T>
    void appendToBuffer(const T & in_buffer);
    std::vector<uint8_t> getBuffer();
    void addDeltaTime(uint64_t ticks_number);
    void addMidiEvent(std::string message, std::vector<uint8_t> data={}, uint8_t channel=0);
    template<typename T>
    void addMetaEvent(std::string event_name, T data);
private:
    std::vector<uint8_t> buffer;
    bool meta_event_mode;
    bool delta_time_set;
    std::string header;
};




class MidiScore {
public:
    // Constructor
    MidiScore();
    // Destructor
    virtual ~MidiScore();
private:
    std::vector<MidiTrack> tracks;
    unsigned int PPQ;
    int format;
};




#endif /* MIDI_SCORE */
