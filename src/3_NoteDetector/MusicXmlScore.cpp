#include "MusicXmlScore.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <map>


std::string enum_to_string(ClefSign sign) {
    switch(sign){
        case ClefSign::G:
            return "G";
        case ClefSign::F:
            return "F";
        case ClefSign::C:
            return "C";
        case ClefSign::PERCUSSION:
            return "percussion";
        case ClefSign::TAB:
            return "TAB";
        case ClefSign::JIANPU:
            return "jianpu";
        case ClefSign::NONE:
            return "none";
        default:
            throw std::runtime_error("Input sign does not exists");
    }
}


std::string enum_to_string(Alter alter) {
    switch(alter){
        case Alter::NATURAL:
            return "0";
        case Alter::SHARP:
            return "1";
        case Alter::FLAT:
            return "-1";
        default:
            throw std::runtime_error("Input alter does not exists");
    }
}


std::string enum_to_string(Step step) {
    // No need to specify DO RE MI FA SOL LA SI DO
    // because it has same integer value as C D E F G A B
    switch(step){
        case Step::C:
            return "C";
        case Step::D:
            return "D";
        case Step::E:
            return "E";
        case Step::F:
            return "F";
        case Step::G:
            return "G";
        case Step::A:
            return "A";
        case Step::B :
            return "B";
        default:
            throw std::runtime_error("Input step does not exists");
    }
}


std::string enum_to_string(Type type) {
    switch(type){
        case Type::SIXTEENTH:
            return "16th";
        case Type::EIGHTH:
            return "eighth";
        case Type::QUARTER:
            return "quarter";
        case Type::HALF:
            return "half";
        case Type::WHOLE:
            return "whole";
        default:
            throw std::runtime_error("Input type does not exists");
    }
}


std::string enum_to_string(BeamOption beam) {
    switch(beam){
        case BeamOption::BEGIN:
            return "begin";
        case BeamOption::CONTINUE:
            return "continue";
        case BeamOption::END:
            return "end";
        default:
            throw std::runtime_error("Input beam option does not exists");
    }
}


std::string enum_to_string(SlurPlacement placement) {
    switch(placement){
        case SlurPlacement::BELOW:
            return "below";
        case SlurPlacement::ABOVE:
            return "above";
        default:
            throw std::runtime_error("Input slur placement option does not exists");
    }
}


std::string enum_to_string(SlurType type) {
    switch(type){
        case SlurType::START:
            return "start";
        case SlurType::CONTINUE:
            return "continue";
        case SlurType::STOP:
            return "stop";
        default:
            throw std::runtime_error("Input slur type option does not exists");
    }
}

std::string enum_to_string(KeyMode mode) {
    switch(mode){
        case KeyMode::MINOR:
            return "minor";
        case KeyMode::MAJOR:
            return "major";
        default:
            throw std::runtime_error("Input key mode option does not exists");
    }
}

// Element
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
Element::Element(const std::string & version, const std::string & encoding, bool standalone, const std::string & doctype/*=""*/) {
    // Constructor for root element only
    this->tag = "";
    this->parent = NULL;
    this->version = version;
    this->encoding = encoding;
    this->standalone = standalone;
    this->doctype = doctype;
}


Element::Element(const std::string & tag, Element *parent/*=NULL*/) {
    // Constructor
    this->tag = tag;
    this->parent = parent;
}


Element::Element(const Element & element) {
    this->tag = element.tag;
    this->parent = NULL;
    this->text = element.text;
    this->attributes = element.attributes;
    // Only for root element
    this->version = element.version;
    this->encoding = element.encoding;
    this->standalone = element.standalone;
    this->doctype = element.doctype;
    for(const auto & child: element.childs) {
        Element* el = new Element(*child);
        this->addChild(el);
    }
}

Element::~Element() {
    size_t N = this->childs.size();
    for(size_t k = 0; k < N; k++) {
        Element* child = this->childs[0];
        this->childs.erase(this->childs.begin());
        delete child;
    }
}


bool Element::isRoot() const {
    return this->tag.empty();
}


void Element::addChild(Element* element) {
    element->parent = this;
    if(element->isRoot()) {
        throw std::runtime_error("Cannot add a root element as a child element");
    }
    this->childs.push_back(element);
}


void Element::addChild(Element* element, size_t index) {
    element->parent = this;
    if(element->isRoot()) {
        throw std::runtime_error("Cannot add a root element as a child element");
    }
    this->childs.insert(this->childs.begin() + index, element);
}


bool Element::hasChild(const std::string & tag) const {
    for(const auto & child: this->childs) {
        if(child->getTag() == tag) {
            return true;
        }
    }
    return false;
}


void Element::removeChild(size_t index) {
    Element* child = this->childs[index];
    this->childs.erase(this->childs.begin() + index);
    delete child;
}


void Element::removeFirstChild(const std::string & tag) {
    size_t index = this->findFirstChildIndex(tag);
    this->removeChild(index);
}


void Element::removeLastChild(const std::string & tag) {
    size_t index = this->findLastChildIndex(tag);
    this->removeChild(index);
}


void Element::removeAllChilds(const std::string & tag) {
    while(this->hasChild(tag)) {
        size_t index = this->findFirstChildIndex(tag);
        this->removeChild(index);
    }
}


Element* Element::subElement(const std::string & tag) {
    Element* new_element;
    new_element = new Element(tag, this);
    this->addChild(new_element);
    return new_element;
}


Element* Element::subElement(const std::string & tag, size_t index) {
    Element* new_element;
    new_element = new Element(tag, this);
    this->addChild(new_element, index);
    return new_element;
}


Element* Element::subElementAfterFirst(const std::string & tag, const std::string & tag_ref) {
    size_t index = this->findFirstChildIndex(tag_ref);
    return this->subElement(tag, index + 1);
}


Element* Element::subElementAfterLast(const std::string & tag, const std::string & tag_ref) {
    size_t index = this->findLastChildIndex(tag_ref);
    return this->subElement(tag, index + 1);
}


Element* Element::subElementBeforeFirst(const std::string & tag, const std::string & tag_ref) {
    size_t index = this->findFirstChildIndex(tag_ref);
    return this->subElement(tag, index);
}


Element* Element::subElementBeforeLast(const std::string & tag, const std::string & tag_ref) {
    size_t index = this->findLastChildIndex(tag_ref);
    return this->subElement(tag, index);
}


void Element::setText(const std::string & text) {
    if(this->isRoot()) {
        throw std::runtime_error("Cannot set text to a root element");
    }
    this->text = text;
}


std::string Element::getText() const {
    return this->text;
}


void Element::setAttr(const std::map<std::string, std::string> & attributes) {
    if(this->isRoot()) {
        throw std::runtime_error("Cannot set attributes to a root element");
    }
    this->attributes = attributes;
}


void Element::addAttr(const std::string & key, const std::string & value) {
    if(this->isRoot()) {
        throw std::runtime_error("Cannot add attributes to a root element");
    }
    this->attributes.insert( std::pair<std::string, std::string>(key, value));
}


std::map<std::string, std::string> Element::getAttr() const {
    return this->attributes;
}


Element* Element::getParent() const {
    return this->parent;
}


std::string Element::getTag() const {
    return this->tag;
}


std::vector<Element*> Element::findAllChilds(const std::string & tag) const {
    std::vector<Element*> child_elements;
    for(auto & ptr_child: this->childs) {
        if(ptr_child->getTag() == tag) {
            child_elements.push_back(ptr_child);
        }
    }
    return child_elements;
}


Element* Element::findFirstChild(const std::string & tag) const {
    // Search from the beginning to the end
    for(const auto & child: this->childs) {
        if(child->getTag() == tag) {
            return child;
        }
    }
    throw std::runtime_error("Child element with tag: '" + tag + "' does not exists");
}


Element* Element::findLastChild(const std::string & tag) const {
    // Search from the end to the beginning
    for(auto child_p = this->childs.rbegin() ; child_p != this->childs.rend(); child_p++ ) {
        if((*child_p)->getTag() == tag) {
            return *child_p;
        }
    }
    throw std::runtime_error("Child element with tag: '" + tag + "' does not exists");
}


size_t Element::findFirstChildIndex(const std::string & tag) const {
    // Search from the beginning to the end
    for(size_t k = 0; k < this->childs.size(); k++) {
        if(this->childs[k]->getTag() == tag) {
            return k;
        }
    }
    throw std::runtime_error("Child element with tag: '" + tag + "' does not exists");
}


size_t Element::findLastChildIndex(const std::string & tag) const {
    // Search from the end to the beginning
    for(int64_t k = this->childs.size() - 1; k >= 0; k--) {
        if(this->childs[k]->getTag() == tag) {
            return k;
        }
    }
    throw std::runtime_error("Child element with tag: '" + tag + "' does not exists");
}


bool Element::hasChilds() const {
    return (this->childs.size() != 0);
}


std::string Element::toString(unsigned int nb_space_indent) const {
    bool has_childs = this->hasChilds();
    bool has_text = (this->text.size() != 0);
    bool has_attr = (this->attributes.size() != 0);
    bool is_root = this->isRoot();
    // Indentation of itself and childs
    std::string main_indent(nb_space_indent, ' ');
    unsigned int nb_space_indent_child = nb_space_indent;
    if(!is_root) {
        nb_space_indent_child += 4;
    }
    std::string child_indent(nb_space_indent_child, ' ');
    // String containing the result
    std::string string_content;
    if(is_root) {
        string_content += main_indent + "<?xml";
        string_content += " version=\"" + this->version + "\"";
        string_content += " encoding=\"" + this->encoding + "\"";
        string_content += " standalone=\"";
        if(this->standalone) {
            string_content += "yes\"?>\n";
        } else {
            string_content += "no\"?>\n";
        }
        if(this->doctype.size() > 0) {
            string_content += main_indent + "<!DOCTYPE ";
            string_content += this->doctype + ">";
        }
    } else {
        // Start tag
        string_content += main_indent + "<" + this->tag;
        // Attributes
        if(has_attr) {
            for(auto const & attr: this->attributes) {
                string_content += " " + attr.first + "=\"" + attr.second + "\"";
            }
        }
        if(!has_text && !has_childs) {
            string_content += "/>\n";
            return string_content;
        } else if(!has_childs){
            string_content += ">";
        } else {
            string_content += ">\n";
        }
        // Text content
        if(has_text){
            if(has_childs) {
                string_content += child_indent + this->text + "\n";
            }
            else {
                string_content += this->text;
            }
        }
    }
    // Childs content
    for(auto const & child: this->childs) {
        string_content += child->toString(nb_space_indent_child);
    }
    if(!is_root) {
        // End tag
        if(has_childs) {
            string_content += main_indent + "</" + this->tag + ">" + "\n";
        } else {
            string_content += "</" + this->tag + ">" + "\n";
        }
    }
    return string_content;
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// AttributesElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
AttributesElement::AttributesElement(Element *parent): Element("attributes", parent)  {
    // Constructor
}


AttributesElement::~AttributesElement() {
    // Destructor
}


int AttributesElement::getDivisions() const {
    if(!this->hasChild("divisions")){
        throw std::runtime_error("Divisions element does not exist");
    }
    Element* div_el = this->findFirstChild("divisions");
    std::string div_str = div_el->getText();
    return std::stoi(div_str);
}


std::pair<int, int> AttributesElement::getTime() const {
    if(!this->hasChild("time")){
        throw std::runtime_error("Time element does not exist");
    }
    Element* time_el = this->findFirstChild("time");
    Element* beats_el = time_el->findFirstChild("beats");
    Element* beattype_el = time_el->findFirstChild("beat-type");
    std::string beats_str = beats_el->getText();
    std::string beattype_str = beattype_el->getText();
    return std::make_pair(std::stoi(beats_str), std::stoi(beattype_str));
}


void AttributesElement::setDivisions(unsigned int divisions) {
    Element* divisions_el;
    if(this->hasChild("divisions")){
        divisions_el = this->findFirstChild("divisions");
    } else {
        divisions_el = this->subElement("divisions", 0);
    }
    divisions_el->setText(std::to_string(divisions));
}


Element* AttributesElement::getKeyElement() {
    Element* key_el;
    if(this->hasChild("key")) {
        key_el = this->findFirstChild("key");
    } else if(this->hasChild("divisions")) {
        key_el = this->subElementAfterFirst("key", "divisions");
    } else {
        key_el = this->subElement("key", 0);
    }
    return key_el;
}


void AttributesElement::setMode(KeyMode mode) {
    Element* key_el = this->getKeyElement();
    Element* mode_el;
    if(key_el->hasChild("mode")) {
        mode_el = key_el->findFirstChild("mode");
    } else {
        mode_el = key_el->subElement("mode");
    }
    mode_el->setText(enum_to_string(mode));
}


void AttributesElement::setFifths(KeyFifths fifths) {
    Element* key_el = this->getKeyElement();
    Element* fifths_el;
    if(key_el->hasChild("fifths")) {
        fifths_el = key_el->findFirstChild("fifths");
    } else {
        fifths_el = key_el->subElement("fifths", 0);
    }
    fifths_el->setText(std::to_string(int(fifths)));
}


void AttributesElement::setKey(KeyFifths fifths, KeyMode mode) {
    this->setFifths(fifths);
    this->setMode(mode);
}


void AttributesElement::setTime(int beats, int beattype) {
    Element* time_el;
    if(this->hasChild("time")) {
        time_el = this->findFirstChild("time");
    } else if(this->hasChild("key")) {
        time_el = this->subElementAfterFirst("time", "key");
    } else if(this->hasChild("divisions")) {
        time_el = this->subElementAfterFirst("time", "divisions");
    } else {
        time_el = this->subElement("time", 0);
    }
    Element* beats_el;
    if(time_el->hasChild("beats")) {
        beats_el = time_el->findFirstChild("time");
    } else {
        beats_el = time_el->subElement("beats", 0);
    }
    beats_el->setText(std::to_string(beats));
    Element* beattype_el;
    if(time_el->hasChild("beat-type")) {
        beattype_el = time_el->findFirstChild("beat-type");
    } else {
        beattype_el = time_el->subElement("beat-type");
    }
    beattype_el->setText(std::to_string(beattype));
}


void AttributesElement::setClef(ClefSign sign, int line/*=-99*/) {
    // sign : 'G' 'F' or 'C'
    Element* clef_el;
    if(this->hasChild("clef")) {
        clef_el = this->findFirstChild("clef");
    } else {
        clef_el = this->subElement("clef");
    }
    Element* sign_el;
    if(clef_el->hasChild("sign")) {
        sign_el = clef_el->findFirstChild("sign");
    } else {
        sign_el = clef_el->subElement("sign", 0);
    }
    Element* line_el;
    if(clef_el->hasChild("line")) {
        line_el = clef_el->findFirstChild("line");
    } else {
        line_el = clef_el->subElement("line");
    }
    sign_el->setText(enum_to_string(sign));
    if(line != -99) {
        line_el->setText(std::to_string(line));
    } else if(sign == ClefSign::G) {
        line_el->setText("2");
    } else if(sign == ClefSign::C) {
        line_el->setText("3");
    } else if(sign == ClefSign::F) {
        line_el->setText("4");
    } else {
        throw std::runtime_error("The key sign '" + enum_to_string(sign) + "' is not compatible.");
    }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// NoteElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
NoteElement::NoteElement(bool is_a_note, Element *parent/*=NULL*/): Element("note", parent)  {
    // Constructor
    if(is_a_note) {
        if(!this->subElement("pitch")) {
            throw std::runtime_error("Failed to add the 'pitch' element");
        }
    } else {
        if(!this->subElement("rest")) {
            throw std::runtime_error("Failed to add the 'rest' element");
        }
    }
}


NoteElement::~NoteElement() {
    // Destructor
}


Element* NoteElement::subElementRightPlace(const std::string & tag) {
    int ind_start = -1;
    for(int k = 0; k < (int)NOTE_TAGS_ORDER.size(); k++) {
        if(NOTE_TAGS_ORDER[k] == tag) {
            ind_start = k;
            break;
        }
    }
    if(ind_start < 0) {
        throw std::runtime_error("The tag: \"" + tag + "\" does not make part of a 'note' element"); 
    }
    for(int k = ind_start - 1; k >= 0; k--) {
        if(this->hasChild(NOTE_TAGS_ORDER[k])) {
            return(this->subElementAfterFirst(tag, NOTE_TAGS_ORDER[k]));
        }
    }
    throw std::runtime_error("The tag: \"" + tag + "\" was not added, this should not happen, NOTE_TAGS_ORDER constant must be wrong"); 
}


bool NoteElement::isANote() const {
    return this->hasChild("pitch");
}


void NoteElement::setStep(Step step) {
    if(!this->isANote()) {
        throw std::runtime_error("Cannot set pitch for a rest, it doesn't make sense");
    }
    // Get pitch element
    Element* pitch_el = this->findFirstChild("pitch");
    // Step
    Element* step_el;
    if(pitch_el->hasChild("step")) {
        step_el = pitch_el->findFirstChild("step");
    } else {
        step_el = pitch_el->subElement("step", 0);
    }
    step_el->setText(enum_to_string(step));
}


void NoteElement::setAlter(Alter alter) {
    if(!this->isANote()) {
        throw std::runtime_error("Cannot set pitch for a rest, it doesn't make sense");
    }
    // Get pitch element
    Element* pitch_el = this->findFirstChild("pitch");
    // Alter
    Element* alter_el;
    if(pitch_el->hasChild("alter")) {
        alter_el = pitch_el->findFirstChild("alter");
    } else {
        alter_el = pitch_el->subElementAfterFirst("alter", "step");
    }
    alter_el->setText(enum_to_string(alter));
}


void NoteElement::setOctave(int octave) {
    if(!this->isANote()) {
        throw std::runtime_error("Cannot set pitch for a rest, it doesn't make sense");
    }
    // Get pitch element
    Element* pitch_el = this->findFirstChild("pitch");
    // Octave
    Element* octave_el;
    if(pitch_el->hasChild("octave")) {
        octave_el = pitch_el->findFirstChild("octave");
    } else {
        octave_el = pitch_el->subElement("octave");
    }
    octave_el->setText(std::to_string(octave));
}


void NoteElement::setPitch(Step step, int octave) {
    this->setStep(step);
    // Get pitch element
    Element* pitch_el = this->findFirstChild("pitch");
    if(pitch_el->hasChild("alter")) {
        pitch_el->removeFirstChild("alter");
    }
    this->setOctave(octave);
}


void NoteElement::setPitch(Step step, int octave, Alter alter) {
    this->setStep(step);
    this->setAlter(alter);
    this->setOctave(octave);
}


void NoteElement::setDuration(int duration) {
    Element* duration_el;
    if(this->hasChild("duration")) {
        duration_el = this->findFirstChild("duration");
    } else {
        duration_el = this->subElementRightPlace("duration");
    }
    duration_el->setText(std::to_string(duration));
}


int NoteElement::getDuration() const {
    Element* duration_el;
    if(!this->hasChild("duration")) {
        throw std::runtime_error("Duration element does not exist");
    }
    duration_el = this->findFirstChild("duration");
    int duration = std::stoi(duration_el->getText());
    return duration;
}


void NoteElement::setType(Type type) {
    Element* type_el;
    if(this->hasChild("type")) {
        type_el = this->findFirstChild("type");
    } else {
        type_el = this->subElementRightPlace("type");
    }
    type_el->setText(enum_to_string(type));
}


void NoteElement::setDot(bool dotted/*=true*/) {
    if(dotted && !this->hasChild("dot")) {
        this->subElementRightPlace("dot");
    } else if(!dotted && this->hasChild("dot")) {
        this->removeFirstChild("dot");
    }
}


void NoteElement::setTimeModification(int actual_note, int normal_note) {
    Element* timemod_el;
    if(this->hasChild("time-modification")) {
        timemod_el = this->findFirstChild("time-modification");
    } else {
        timemod_el = this->subElementRightPlace("time-modification");
    }
    Element* actualnotes_el;
    if(timemod_el->hasChild("actual-notes")) {
        actualnotes_el = timemod_el->findFirstChild("actual-notes");
    } else {
        actualnotes_el = timemod_el->subElement("actual-notes");
    }
    Element* normalnotes_el;
    if(timemod_el->hasChild("normal-notes")) {
        normalnotes_el = timemod_el->findFirstChild("normal-notes");
    } else {
        normalnotes_el = timemod_el->subElement("normal-notes");
    }
    actualnotes_el->setText(std::to_string(actual_note));
    normalnotes_el->setText(std::to_string(normal_note));
}


void NoteElement::setTripletTimeModification(bool triplet/*=true*/) {
    if(triplet) {
        this->setTimeModification(3, 2);
    } else {
        this->removeAllChilds("time-modification");
    }
}


void NoteElement::setBeams(const Beams & beams) {
    this->removeAllChilds("beam");
    for (auto const& beam : beams) {
        Element* beam_el = this->subElementRightPlace("beam");
        beam_el->addAttr("number", std::to_string(beam.first));
        beam_el->setText(enum_to_string(beam.second));
    }
}


void NoteElement::setSlurs(const Slurs & slurs) {
    Element* notations_el;
    if(this->hasChild("notations")) {
        notations_el = this->findFirstChild("notations");
        notations_el->removeAllChilds("slur");
    } else if(!slurs.empty()){
        notations_el = this->subElementRightPlace("notations");
    }

    for (auto const& slur : slurs) {
        Element* slur_el = notations_el->subElement("slur");
        slur_el->addAttr("number", std::to_string(slur.first));
        slur_el->addAttr("placement", enum_to_string(slur.second.placement));
        slur_el->addAttr("type", enum_to_string(slur.second.type));
    }
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// PartElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
PartElement::PartElement(const std::string & id, Element *parent=NULL): Element("part", parent)  {
    // Constructor
    this->addAttr("id", id);
    this->nb_measures = 0;
}


PartElement::~PartElement() {
    // Destructor
}


int PartElement::getNumberOfMeasures() const {
    return this->nb_measures;
}


int PartElement::getLastDivisionsInfo() const {
    std::vector<Element*> list_measures_el = this->findAllChilds("measure");
    AttributesElement *attributes_el = NULL;
    for(auto measure_p = list_measures_el.rbegin() ; measure_p != list_measures_el.rend(); measure_p++ ) {
        if(!(*measure_p)->hasChild("attributes")) {
            continue;
        }
        attributes_el = dynamic_cast<AttributesElement*>((*measure_p)->findFirstChild("attributes"));
        if(attributes_el->hasChild("divisions")) {
            return attributes_el->getDivisions();
        }
    }
    return -1;
}


std::pair<int, int> PartElement::getLastTimeInfo() const {
    std::vector<Element*> list_measures_el = this->findAllChilds("measure");
    AttributesElement *attributes_el = NULL;
    for(auto measure_p = list_measures_el.rbegin() ; measure_p != list_measures_el.rend(); measure_p++ ) {
        if(!(*measure_p)->hasChild("attributes")) {
            continue;
        }
        attributes_el = dynamic_cast<AttributesElement*>((*measure_p)->findFirstChild("attributes"));
        if(attributes_el->hasChild("time")) {
            return attributes_el->getTime();
        }
    }
    return std::make_pair(-1, -1);
}


int PartElement::getDurationLastMeasure() const {
    Element* measure_el = this->getLastMeasure();
    if(!measure_el) {
        throw std::runtime_error("Still any measures created");
    }
    std::vector<Element*> list_note_el = measure_el->findAllChilds("note");

    int sum_duration = 0;
    for(const auto & note_el: list_note_el) {
        sum_duration += dynamic_cast<NoteElement*>(note_el)->getDuration();
    }
    return sum_duration;
}


int PartElement::getDurationRequired() const {
    int divisions = this->getLastDivisionsInfo();
    std::pair<int, int> time = this->getLastTimeInfo();
    if((divisions < 0) || (time.first < 0) || (time.second < 0)) {
        throw std::runtime_error("Attributes not defined");
    }
    // time.first <=> beat
    // time.second <=> beat-type
    int duration_required = (int)(divisions * time.first * ( 4.0 / time.second));
    return duration_required;
}


bool PartElement::isLastMeasureComplete() const {
    Element* measure_el = this->getLastMeasure();
    if(!measure_el) {
        return true;
    }
    int duration_required = this->getDurationRequired();
    int duration = this->getDurationLastMeasure();
    return(duration_required == duration);
}


Element* PartElement::addMeasure() {
    if(!this->isLastMeasureComplete()) {
        throw std::runtime_error("Cannot add a measure, the past measure is not completed yet");
    }
    this->nb_measures += 1;
    Element* measure_el = this->subElement("measure");
    measure_el->addAttr("number", std::to_string(this->nb_measures));
    return measure_el;
}


Element* PartElement::getLastMeasure() const {
    if(this->hasChild("measure")) {
        return(this->findLastChild("measure"));
    }
    return NULL;
}


void PartElement::pushNote(NoteElement* note_el) {
    // Get the required measure
        // If the measure is completed, create a new one
        // If this is the first measure and no attributes is specified throw an error
        // If the measure is not completed, but the note duration is highter than the available duration remaining in the measure, throw an error as well
    Element* measure_el = this->getLastMeasure();
    // If no measures exists, return an error because the first element OF THE FIRST MEASURE must be an attribute element
    if(!measure_el) {
        throw std::runtime_error("Cannot add a note as first element of the first measure, an attribute must be set first");
    }
    if(this->isLastMeasureComplete()) {
        measure_el = this->addMeasure();
    }
    int duration_required = this->getDurationRequired();
    int duration = this->getDurationLastMeasure();
    if(duration + note_el->getDuration() > duration_required) {
        throw std::runtime_error("The duration of the note exceed the size of the measure");
    }
    measure_el->addChild(note_el);
}


void PartElement::pushAttributes(AttributesElement* attr_el) {
    Element* measure_el = this->getLastMeasure();
    // If no measures exists, create a new one
    if(!measure_el) {
        measure_el = this->addMeasure();
    }
    if(measure_el->hasChilds()) {
        throw std::runtime_error("Cannot add attributes element if a note or attributes has already been added to the measure");
    }
    measure_el->addChild(attr_el);
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////








// MusicXmlScore
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
MusicXmlScore::MusicXmlScore(const std::string & title/*="Untitled"*/, const std::string & composer/*="Unknown"*/): Element("1.0", "UTF-8", false, MUSICXML_DOCTYPE) {
    // score-partwise
    this->partwise_el = this->subElement("score-partwise");
    this->partwise_el->addAttr("version", "3.0");
    // title
    Element* work_el = this->partwise_el->subElement("work");
    this->worktitle_el = work_el->subElement("work-title");
    this->worktitle_el->setText(title);
    // Composer
    Element* identification_el = this->partwise_el->subElement("identification");
    this->composer_el = identification_el->subElement("creator");
    this->composer_el->addAttr("type", "composer");
    this->composer_el->setText(composer);
    // Rights
    Element* rights_el = identification_el->subElement("rights");
    rights_el->setText("Copyright © 2019 ScoreListo, Inc.");
    // part-list
    this->partlist_el = this->partwise_el->subElement("part-list");
}

MusicXmlScore::MusicXmlScore(const MusicXmlScore & score): Element(score) {
    this->partwise_el = score.partwise_el;
    this->worktitle_el = score.worktitle_el;
    this->composer_el = score.composer_el;
    this->partlist_el = score.partlist_el;
}

MusicXmlScore::~MusicXmlScore() {
    // Destructor
}


void MusicXmlScore::updateTitle(const std::string & title) {
    this->worktitle_el->setText(title);
}


void MusicXmlScore::updateComposer(const std::string & composer) {
    this->composer_el->setText(composer);
}


PartElement* MusicXmlScore::getPart(const std::string & id_part) const {
    std::vector<Element*> listpart_el = this->partwise_el->findAllChilds("part");
    for(const auto & part_el: listpart_el) {
        if(part_el->getAttr()["id"] == id_part) {
            return dynamic_cast<PartElement*>(part_el);
        }
    }
    return NULL;
}


void MusicXmlScore::addPart(const std::string & id_part, const std::string & instrument/*="piano"*/, const std::string & name/*="solo"*/) {
    if(this->getPart(id_part)) {
        throw std::runtime_error("A part with this id already exists");
    }
    Element* scorepart_el = this->partlist_el->subElement("score-part");
    scorepart_el->addAttr("id",id_part);

    Element* partname_el = scorepart_el->subElement("part-name");
    partname_el->setText(name);

    Element* scoreinstru_el = scorepart_el->subElement("score-instrument");
    scoreinstru_el->addAttr("id", id_part + "-I1");

    Element* instruname_el = scoreinstru_el->subElement("instrument-name");
    instruname_el->setText(instrument);

    PartElement* part_el = new PartElement(id_part, this->partwise_el);
    this->partwise_el->addChild(part_el);
}


void MusicXmlScore::pushNote(NoteElement* note_el, const std::string & id_part) {
    PartElement* part_el = this->getPart(id_part);
    if(!part_el) {
        throw std::runtime_error("Part with this ID does not exists");
    }
    part_el->pushNote(note_el);
}


void MusicXmlScore::pushAttributes(AttributesElement* attr_el, const std::string & id_part) {
    PartElement* part_el = this->getPart(id_part);
    if(!part_el) {
        throw std::runtime_error("Part with this ID does not exists");
    }
    part_el->pushAttributes(attr_el);
}
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
