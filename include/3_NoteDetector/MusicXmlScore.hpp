#ifndef MUSIC_XML_SCORE
#define MUSIC_XML_SCORE

#include <string>
#include <vector>
#include <map>


enum class ClefSign {
    G,
    F,
    C,
    PERCUSSION,
    TAB,
    JIANPU,
    NONE
};
enum class Step {
    C = 0,
    D = 2,
    E = 4,
    F = 5,
    G = 7,
    A = 9,
    B = 11,
    DO = 0,
    RE = 2,
    MI = 4,
    FA = 5,
    SOL = 7,
    LA = 9,
    SI = 11
};
enum class Alter {
    NATURAL = 0,
    SHARP = 1,
    FLAT = -1
};
enum class KeyFifths {
    ZERO = 0,
    // Sharp
    ONE_SHARP = 1,
    TWO_SHARPS = 2,
    THREE_SHARPS = 3,
    FOUR_SHARPS = 4,
    FIVE_SHARPS = 5,
    SIX_SHARPS = 6,
    SEVEN_SHARPS = 7,
    // Flat
    ONE_FLAT = -1,
    TWO_FLATS = -2,
    THREE_FLATS = -3,
    FOUR_FLATS = -4,
    FIVE_FLATS = -5,
    SIX_FLATS = -6,
    SEVEN_FLATS = -7
};
// Type
enum class KeyMode {MINOR, MAJOR};
// Type
enum class Type {SIXTEENTH, EIGHTH, QUARTER, HALF, WHOLE};
// Beams
enum class BeamOption {BEGIN, CONTINUE, END};
// Slurs
enum class SlurPlacement {BELOW, ABOVE};
enum class SlurType {START, CONTINUE, STOP};

std::string enum_to_string(ClefSign sign);
std::string enum_to_string(Alter alter);
std::string enum_to_string(Step step);
std::string enum_to_string(Type type);
std::string enum_to_string(BeamOption beam);
std::string enum_to_string(SlurPlacement placement);
std::string enum_to_string(SlurType type);
std::string enum_to_string(KeyMode mode);


// Element
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
class Element {
public:
    // Constructors & Destructor
    Element(const std::string & version, const std::string & encoding, bool standalone, const std::string & doctype="");
    Element(const std::string & tag, Element *parent=NULL);
    Element(const Element & element);
    virtual ~Element();
    // Methods
    bool isRoot() const;
    void addChild(Element* element);
    void addChild(Element* element, size_t index);
    bool hasChild(const std::string & tag) const;
    void removeChild(size_t index);
    void removeFirstChild(const std::string & tag);
    void removeLastChild(const std::string & tag);
    void removeAllChilds(const std::string & tag);
    Element* subElement(const std::string & tag);
    Element* subElement(const std::string & tag, size_t index);
    Element* subElementAfterFirst(const std::string & tag, const std::string & tag_ref);
    Element* subElementAfterLast(const std::string & tag, const std::string & tag_ref);
    Element* subElementBeforeFirst(const std::string & tag, const std::string & tag_ref);
    Element* subElementBeforeLast(const std::string & tag, const std::string & tag_ref);
    void setText(const std::string & text);
    std::string getText() const;
    void setAttr(const std::map<std::string, std::string> & attributes);
    void addAttr(const std::string & key, const std::string & value);
    std::map<std::string, std::string> getAttr() const;
    Element* getParent() const;
    std::string getTag() const;
    std::vector<Element*> findAllChilds(const std::string & tag) const;
    Element* findFirstChild(const std::string & tag) const;
    Element* findLastChild(const std::string & tag) const;
    size_t findFirstChildIndex(const std::string & tag) const;
    size_t findLastChildIndex(const std::string & tag) const;
    bool hasChilds() const;
    std::string toString(unsigned int nb_space_indent=0) const;
private:
    std::string tag;
    std::vector<Element*> childs;
    Element *parent;
    std::string text;
    std::map<std::string, std::string> attributes;
    // Only for root element
    std::string version;
    std::string encoding;
    bool standalone;
    std::string doctype;
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// AttributesElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
class AttributesElement: public Element {
public:
    // Constructors & Destructor
    AttributesElement(Element *parent=NULL);
    virtual ~AttributesElement();
    // Methods
    int getDivisions() const;
    std::pair<int, int> getTime() const;
    void setDivisions(unsigned int divisions);
    Element* getKeyElement();
    void setFifths(KeyFifths fifths);
    void setMode(KeyMode mode);
    void setKey(KeyFifths fifths, KeyMode mode);
    void setTime(int beats, int beattype);
    void setClef(ClefSign sign, int line=-99);
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// NoteElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
struct SlurOptions {
    SlurPlacement placement;
    SlurType type;
};
typedef std::map<int, BeamOption> Beams;
typedef std::map<int, SlurOptions> Slurs;

const std::vector<std::string> NOTE_TAGS_ORDER = {"pitch", "rest", "duration", "type", "dot", "time-modification", "beam", "slur"};

class NoteElement: public Element {
public:
    // Constructors & Destructor
    NoteElement(bool is_a_note, Element *parent=NULL);
    virtual ~NoteElement();
    // Methods
    Element* subElementRightPlace(const std::string & tag);
    bool isANote() const;
    void setStep(Step step);
    void setAlter(Alter alter);
    void setOctave(int octave);
    void setPitch(Step step, int octave);
    void setPitch(Step step, int octave, Alter alter);
    void setDuration(int duration);
    int getDuration() const;
    void setType(Type type);
    void setDot(bool dotted=true);
    void setTimeModification(int actual_note, int normal_note);
    void setTripletTimeModification(bool triplet=true);
    void setBeams(const Beams & beams);
    void setSlurs(const Slurs & slurs);
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// PartElement
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
class PartElement: public Element {
public:
    // Constructors & Destructor
    PartElement(const std::string & id, Element *parent);
    virtual ~PartElement();
    // Methods
    int getNumberOfMeasures() const;
    int getLastDivisionsInfo() const;
    std::pair<int, int> getLastTimeInfo() const;
    int getDurationLastMeasure() const;
    int getDurationRequired() const;
    bool isLastMeasureComplete() const;
    Element* addMeasure();
    Element* getLastMeasure() const;
    void pushNote(NoteElement* note_el);
    void pushAttributes(AttributesElement* attr_el);
private:
    int nb_measures;
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


// MusicXmlScore
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
const std::string MUSICXML_DOCTYPE = "score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\"";

class MusicXmlScore: public Element {
public:
    // Constructors & Destructor
    MusicXmlScore(const std::string & title="Untitled", const std::string & composer="Unknown");
    MusicXmlScore(const MusicXmlScore & score);
    virtual ~MusicXmlScore();
    // Methods
    void updateTitle(const std::string & title);
    void updateComposer(const std::string & composer);
    PartElement* getPart(const std::string & id_part) const;
    void addPart(const std::string & id_part, const std::string & instrument="piano", const std::string & name="solo");
    void pushNote(NoteElement* note_el, const std::string & id_part);
    void pushAttributes(AttributesElement* attr_el, const std::string & id_part);
private:
    Element* partwise_el;
    Element* worktitle_el;
    Element* composer_el;
    Element* partlist_el;
};
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////


#endif /* MUSIC_XML_SCORE */
