#include "MidiScore.hpp"
#include <vector>
#include <string>
#include <bitset>
#include <cmath>
#include <iostream>

MidiScore::MidiScore() {
    // Constructor
}

MidiScore::~MidiScore() {
    // Desctructor
}



std::vector<uint8_t> int16ToBytes(uint16_t value) {
    // Big endian
    std::vector<uint8_t> arrayOfByte(2);
    arrayOfByte[1] = value;
    arrayOfByte[0] = (value >> 8);
    return arrayOfByte;
}


std::vector<uint8_t> int32ToBytes(uint32_t value) {
    // Big endian
    std::vector<uint8_t> arrayOfByte(4);
    arrayOfByte[3] = value;
    arrayOfByte[2] = (value >> 8);
    arrayOfByte[1] = (value >> 16);
    arrayOfByte[0] = (value >> 24);
    return arrayOfByte;
}


std::vector<uint8_t> int64ToVLQBytes(uint64_t value) {
    std::string bin_string = std::bitset<8>(value).to_string();
    size_t nb_bits = bin_string.size();
    // Number of VLQ bytes
    unsigned int nb_vlq_bytes = floor(nb_bits / 7);
    if(nb_bits % 7 > 0) {
        nb_vlq_bytes += 1;
    }
    // Array of vlq bytes
    std::vector<uint8_t> array_vlq_bytes;
    // string storing the temporary byte being built
    std::string bin_byte_str;
    uint8_t temp_byte_value;
    for(unsigned int k=0; k < nb_vlq_bytes; k++) {
        if(k == 0) {
            unsigned int nb_bits_last_byte = nb_bits % 7;
            if(nb_bits_last_byte == 0) {
                nb_bits_last_byte = 7;
            }
            unsigned int nb_zeros = 7 - nb_bits_last_byte;
            if(k == nb_vlq_bytes - 1) {
                bin_byte_str = '0';
                for(unsigned int kzero = 0; kzero < nb_zeros; kzero++) {
                    bin_byte_str += '0';
                }
                bin_byte_str += bin_string;
            } else {
                bin_byte_str = '1';
                for(unsigned int kzero=0; kzero < nb_zeros; kzero++) {
                    bin_byte_str += '0';
                }
                for(unsigned int kbyte = 0; kbyte < nb_bits_last_byte; kbyte++) {
                    bin_byte_str += bin_string[kbyte];
                }
            }
        } else if(k == nb_vlq_bytes) {
            bin_byte_str = '0';
            for(size_t kbyte = nb_bits-7; kbyte < nb_bits; kbyte++) {
                bin_byte_str += bin_string[kbyte];
            }
        } else {
            bin_byte_str = "1";
            for(size_t kbyte = (nb_bits-(k+1)*7); kbyte < (nb_bits-k*7); kbyte++) {
                bin_byte_str += bin_string[kbyte];
            }
        }
        temp_byte_value = (uint8_t)std::bitset<8>(bin_byte_str).to_ulong();
        array_vlq_bytes.push_back(temp_byte_value);
    }
    return array_vlq_bytes;
}


MidiTrack::MidiTrack() {
    // Constructor
    this->meta_event_mode = true;
    this->delta_time_set = false;
    this->header = "MTrk";
}

MidiTrack::~MidiTrack() {
    // Destructor
}

uint32_t MidiTrack::getBufferSize() {
    return (uint32_t)this->buffer.size();
}


template<typename T>
void MidiTrack::appendToBuffer(const T & in_buffer) {
    this->buffer.insert(this->buffer.end(), in_buffer.begin(), in_buffer.end());
}


std::vector<uint8_t> MidiTrack::getBuffer() {
    // Check that the buffer is ready
    if(this->delta_time_set) {
        throw std::runtime_error("Uncompleted track: a delta time has been defined with no event following");
    }
    // Output buffer
    std::vector<uint8_t> output_buffer;
    // Adding the header
    output_buffer.assign(this->header.begin(), this->header.end());
    // Adding the size of the track
    uint32_t track_size = getBufferSize();
    std::vector<uint8_t> track_size_buffer = int32ToBytes(track_size);
    output_buffer.insert(output_buffer.end(), track_size_buffer.begin(), track_size_buffer.end());
    // Adding the buffer
    output_buffer.insert(output_buffer.end(), this->buffer.begin(), this->buffer.end());
    return output_buffer;
}


void MidiTrack::addDeltaTime(uint64_t ticks_number) {
    // Check that the buffer is ready
    if(this->delta_time_set) {
        throw std::runtime_error("A delta time has already been defined, an event is now expected");
    }
    std::vector<uint8_t> ticks_buffer = int64ToVLQBytes(ticks_number);
    this->appendToBuffer(ticks_buffer);
    this->delta_time_set = true;
}


void MidiTrack::addMidiEvent(std::string message, std::vector<uint8_t> data, uint8_t channel) {
    bool message_written = true;
    if(!this->delta_time_set) {
        message_written = false;
        throw std::runtime_error("You first need to defined a delta time before creating an event");
    } else if(message == "Note Off") {
        this->buffer.push_back(128 + channel);
        this->buffer.push_back(data[0]);
        this->buffer.push_back(data[1]);
    } else if(message == "Note On") {
        this->buffer.push_back(144 + channel);
        this->buffer.push_back(data[0]);
        this->buffer.push_back(data[1]);
    } else if(message == "Polyphonic Key Pressure") {
        this->buffer.push_back(160 + channel);
        this->buffer.push_back(data[0]);
        this->buffer.push_back(data[1]);
    } else if(message == "Controller Change") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(data[0]);
        this->buffer.push_back(data[1]);
    } else if(message == "Program Change") {
        this->buffer.push_back(192 + channel);
        this->buffer.push_back(data[0]);
    } else if(message == "Channel Key Pressure") {
        this->buffer.push_back(208 + channel);
        this->buffer.push_back(data[0]);
    } else if(message == "Pitch Bend") {
        this->buffer.push_back(224 + channel);
        this->buffer.push_back(data[0]);
        this->buffer.push_back(data[1]);
    } else if(message == "All Sound Off") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(120);
    } else if(message == "Reset All Controllers") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(121);
    } else if(message == "Local Control") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(122);
        this->buffer.push_back(data[0]);
    } else if(message == "All Notes Off") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(123);
    } else if(message == "Omni Mode Off") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(124);
    } else if(message == "Omni Mode On") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(125);
    } else if(message == "Mono Mode On") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(126);
        this->buffer.push_back(data[0]);
    } else if(message == "Poly Mode On") {
        this->buffer.push_back(176 + channel);
        this->buffer.push_back(127);
    } else {
        message_written = false;
        throw std::runtime_error("Wrong value of input 'message'");
    }
    
    if(message_written) {
        this->delta_time_set = false;
    }
}


template<typename T>
void MidiTrack::addMetaEvent(std::string event_name, T data) {
    /*
    data type must be:
        - uint16_t              in case of  message == "Sequence Number"
        - std::string           in case of  message == "Text Event" ||
                                            message == "Copyright Notice" ||
                                            message == "Sequence/Track Name" ||
                                            message == "Instrument Name" ||
                                            message == "Lyric" ||
                                            message == "Marker" ||
                                            message == "Cue Point" ||
                                            message == "Sequencer-Specific Meta-event"
        - std::vector<uint8_t>  in case of  message == "SMTPE Offset" ||
                                            message == "Time Signature" ||
                                            message == "Key Signature"
        - uint8_t               in case of  message == "MIDI Channel Prefix"
        -                       in case of  message = "Set Tempo"
    */
    bool message_written = true;
    if(!this->delta_time_set) {
      message_written = false;
      throw std::runtime_error("You first need to defined a delta time before creating an event");
    } else if(!this->meta_event_mode) {
      message_written = false;
      throw std::runtime_error("You no longer can defined a meta event, a midi event has already been written");
    // Sequence Number
    } else if(event_name == "Sequence Number") {
        this->buffer.push_back(255);
        this->buffer.push_back(0);
        this->buffer.push_back(2);
        std::vector<uint8_t> int16_bytes = int16ToBytes(data);
        this->appendToBuffer(int16_bytes);
    // Text Event
    } else if(event_name == "Text Event") {
        this->buffer.push_back(255);
        this->buffer.push_back(1);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Copyright Notice
    } else if(event_name == "Copyright Notice") {
        this->buffer.push_back(255);
        this->buffer.push_back(2);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Sequence/Track Name
    } else if(event_name == "Sequence/Track Name") {
        this->buffer.push_back(255);
        this->buffer.push_back(3);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Instrument Name
    } else if(event_name == "Instrument Name") {
        this->buffer.push_back(255);
        this->buffer.push_back(4);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Lyric
    } else if(event_name == "Lyric") {
        this->buffer.push_back(255);
        this->buffer.push_back(5);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Marker
    } else if(event_name == "Marker") {
        this->buffer.push_back(255);
        this->buffer.push_back(6);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // Cue Point
    } else if(event_name == "Cue Point") {
        this->buffer.push_back(255);
        this->buffer.push_back(7);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    // MIDI Channel Prefix
    } else if(event_name == "MIDI Channel Prefix") {
        this->buffer.push_back(255);
        this->buffer.push_back(32);
        this->buffer.push_back(1);
        this->buffer.push_back(data);
    // End of Track
    } else if(event_name == "End of Track") {
        this->buffer.push_back(255);
        this->buffer.push_back(47);
        this->buffer.push_back(0);
    // Set Tempo
    } else if(event_name == "Set Tempo") {
        this->buffer.push_back(255);
        this->buffer.push_back(81);
        this->buffer.push_back(3);
        std::vector<uint8_t> tempo_buffer = int32ToBytes(data);
        this->buffer.push_back(tempo_buffer[1]);
        this->buffer.push_back(tempo_buffer[2]);
        this->buffer.push_back(tempo_buffer[3]);
    // SMTPE Offset
    } else if(event_name == "SMTPE Offset") {
        this->buffer.push_back(255);
        this->buffer.push_back(84);
        this->buffer.push_back(5);
        this->buffer.push_back(data[0]); //hh
        this->buffer.push_back(data[1]); //mm
        this->buffer.push_back(data[2]); //ss
        this->buffer.push_back(data[3]); //fr
        this->buffer.push_back(data[4]); //ff
    // Time Signature
    } else if(event_name == "Time Signature") {
        this->buffer.push_back(255);
        this->buffer.push_back(88);
        this->buffer.push_back(4);
        this->buffer.push_back(data[0]); //nn
        this->buffer.push_back(data[1]); //dd
        this->buffer.push_back(data[2]); //cc
        this->buffer.push_back(data[3]); //bb
    // Key Signature
    } else if(event_name == "Key Signature") {
        this->buffer.push_back(255);
        this->buffer.push_back(89);
        this->buffer.push_back(2);
        this->buffer.push_back(data[0]); //sf
        this->buffer.push_back(data[1]); //mi
    // Sequencer-Specific Meta-event
    } else if(event_name == "Sequencer-Specific Meta-event") {
        this->buffer.push_back(255);
        this->buffer.push_back(127);
        std::vector<uint8_t> size_buffer = int64ToVLQBytes(data.size());
        this->appendToBuffer(size_buffer);
        this->appendToBuffer(data);
    } else {
      message_written = false;
      throw std::runtime_error("Wrong value of type");
    }
    if(message_written) {
      this->delta_time_set = false;
    }
}



